/*
 * dm365evm_keys.c - support IR remote on DM365 EVM board
 *
 * Copyright (c) 2009 by David Brownell
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>


/*
 * The MSP430 firmware on the DM365 EVM monitors an IR receptor used for
 * the remote control.  When any key is pressed, or its autorepeat kicks
 * in, an event is sent.  This driver read those events from the small
 * event queue and reports them.
 *
 * Compared to the DM355 EVM:  the MSP firmware does *only* IR.  So this
 * driver is simpler, and doesn't even need to use the MFD model.
 *
 * Note that physically there can only be one of these devices.
 *
 * This driver was tested with firmware revision 0xA1 (beta board).
 */
struct dm365evm_keys {
	struct input_dev	*input;
	struct i2c_client	*i2c;
};

#define DM365EVM_MSP_FIRMREV		0
#define DM365EVM_MSP_INPUT_LOW		1
#define DM365EVM_MSP_INPUT_HIGH		2
#define DM365EVM_MSP_INPUT_COUNT	3
#define DM365EVM_MSP_0x43		4

#define MSP430_GPIO			0

/* These initial keycodes can be remapped by dm365evm_setkeycode(). */
static struct {
	u16	event;
	u16	keycode;
} dm365evm_keys[] = {

	/*
	 * IR buttons ... codes assigned to match the universal remote
	 * provided with the EVM (Philips PM4S) using DVD code 0020.
	 *
	 * These event codes match firmware documentation, but other
	 * remote controls could easily send more RC5-encoded events.
	 * The PM4S manual was used in several cases to help select
	 * a keycode reflecting the intended usage.
	 *
	 * RC5 codes are 14 bits, with two start bits (0x3 prefix)
	 * and a toggle bit (masked out below).
	 */
	{ 0x300c, KEY_POWER, },		/* NOTE: docs omit this */
	{ 0x3000, KEY_NUMERIC_0, },
	{ 0x3001, KEY_NUMERIC_1, },
	{ 0x3002, KEY_NUMERIC_2, },
	{ 0x3003, KEY_NUMERIC_3, },
	{ 0x3004, KEY_NUMERIC_4, },
	{ 0x3005, KEY_NUMERIC_5, },
	{ 0x3006, KEY_NUMERIC_6, },
	{ 0x3007, KEY_NUMERIC_7, },
	{ 0x3008, KEY_NUMERIC_8, },
	{ 0x3009, KEY_NUMERIC_9, },
	{ 0x3022, KEY_ENTER, },
	{ 0x30ec, KEY_MODE, },		/* "tv/vcr/..." */
	{ 0x300f, KEY_SELECT, },	/* "info" */
	{ 0x3020, KEY_CHANNELUP, },	/* "up" */
	{ 0x302e, KEY_MENU, },		/* "in/out" */
	{ 0x3011, KEY_VOLUMEDOWN, },	/* "left" */
	{ 0x300d, KEY_MUTE, },		/* "ok" */
	{ 0x3010, KEY_VOLUMEUP, },	/* "right" */
	{ 0x301e, KEY_SUBTITLE, },	/* "cc" */
	{ 0x3021, KEY_CHANNELDOWN, },	/* "down" */
	{ 0x3022, KEY_PREVIOUS, },
	{ 0x3026, KEY_SLEEP, },
	{ 0x3172, KEY_REWIND, },
	{ 0x3175, KEY_PLAY, },
	{ 0x3174, KEY_FASTFORWARD, },
	{ 0x3177, KEY_RECORD, },
	{ 0x3176, KEY_STOP, },
	{ 0x3169, KEY_PAUSE, },

	/* NOTE:  SW22, a pushbutton next to the IR sensor, is also
	 * wired to the MSP430 but is currently ignored; some other
	 * I/O pins are likewise wired but ignored.
	 */
};

static irqreturn_t dm365evm_keys_irq(int irq, void *_keys)
{
	struct dm365evm_keys	*keys = _keys;

	/* For simplicity we ignore INPUT_COUNT and just read
	 * events until the GPIO stops signaling data ready.
	 */
	while (gpio_get_value(MSP430_GPIO) == 0) {
		static u16	last_event;
		u16		event;
		int		keycode;
		int		i;

		/* Read low, then high bytes; reading the high byte
		 * removes an entry from the input buffer.
		 */
		event = i2c_smbus_read_word_data(keys->i2c,
					DM365EVM_MSP_INPUT_LOW);
		if (event < 0) {
			dev_dbg(&keys->i2c->dev, "input err %d\n", event);
			break;
		}
		event = event;

		/* Press and release a button:  two events, same code.
		 * Press and hold (autorepeat), then release: N events
		 * (N > 2), same code.  For RC5 buttons the toggle bits
		 * distinguish (for example) "1-autorepeat" from "1 1";
		 * but PCB buttons don't support that bit.
		 *
		 * So we must synthesize release events.  We do that by
		 * mapping events to a press/release event pair; then
		 * to avoid adding extra events, skip the second event
		 * of each pair.
		 */
		if (event == last_event) {
			last_event = 0;
			continue;
		}
		last_event = event;

		/* ignore the RC5 toggle bit */
		event &= ~0x0800;

		/* find the key, or leave it as unknown */
		keycode = KEY_UNKNOWN;
		for (i = 0; i < ARRAY_SIZE(dm365evm_keys); i++) {
			if (dm365evm_keys[i].event != event)
				continue;
			keycode = dm365evm_keys[i].keycode;
			break;
		}
		dev_dbg(&keys->i2c->dev,
			"input event 0x%04x--> keycode %d\n",
			event, keycode);

		/* report press + release */
		input_report_key(keys->input, keycode, 1);
		input_sync(keys->input);
		input_report_key(keys->input, keycode, 0);
		input_sync(keys->input);
	}
	return IRQ_HANDLED;
}

/*
 * Since we talk to the MSP using I2C, we need to delegate all real
 * IRQ handling work to some task context.  We'll use an IRQ thread.
 */
static irqreturn_t dm365evm_keys_hardirq(int irq, void *_keys)
{
	return IRQ_WAKE_THREAD;
}

static int dm365evm_setkeycode(struct input_dev *dev, int index, int keycode)
{
	u16		old_keycode;
	unsigned	i;

	if (((unsigned)index) >= ARRAY_SIZE(dm365evm_keys))
		return -EINVAL;

	old_keycode = dm365evm_keys[index].keycode;
	dm365evm_keys[index].keycode = keycode;
	set_bit(keycode, dev->keybit);

	for (i = 0; i < ARRAY_SIZE(dm365evm_keys); i++) {
		if (dm365evm_keys[index].keycode == old_keycode)
			goto done;
	}
	clear_bit(old_keycode, dev->keybit);
done:
	return 0;
}

static int dm365evm_getkeycode(struct input_dev *dev, int index, int *keycode)
{
	if (((unsigned)index) >= ARRAY_SIZE(dm365evm_keys))
		return -EINVAL;

	return dm365evm_keys[index].keycode;
}

/*----------------------------------------------------------------------*/

static int __devinit
dm365evm_keys_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
	struct dm365evm_keys	*keys;
	struct input_dev	*input;
	int			status;
	int			i;

	status = gpio_request(MSP430_GPIO, id->name);
	if (status < 0)
		return status;

	status = gpio_direction_input(MSP430_GPIO);
	if (status < 0) {
		gpio_free(MSP430_GPIO);
		return status;
	}

	/* allocate instance struct and input dev */
	keys = kzalloc(sizeof *keys, GFP_KERNEL);
	input = input_allocate_device();
	if (!keys || !input) {
		status = -ENOMEM;
		goto fail1;
	}

	keys->i2c = i2c;
	keys->input = input;

	input_set_drvdata(input, keys);

	input->name = "DM365 EVM Controls";
	input->phys = "dm365evm/input0";
	input->dev.parent = &i2c->dev;

	input->id.bustype = BUS_I2C;
	input->id.product = 0x0365;
	input->id.version = i2c_smbus_read_byte_data(i2c,
					DM365EVM_MSP_FIRMREV);

	input->evbit[0] = BIT(EV_KEY);
	for (i = 0; i < ARRAY_SIZE(dm365evm_keys); i++)
		__set_bit(dm365evm_keys[i].keycode, input->keybit);

	input->setkeycode = dm365evm_setkeycode;
	input->getkeycode = dm365evm_getkeycode;

	/* FIXME:  flush the event queue */

	status = request_threaded_irq(gpio_to_irq(MSP430_GPIO),
			dm365evm_keys_hardirq, dm365evm_keys_irq,
			IRQF_TRIGGER_FALLING,
			id->name, keys);
	if (status < 0)
		goto fail1;

	/* register */
	status = input_register_device(input);
	if (status < 0)
		goto fail2;

	i2c_set_clientdata(i2c, keys);

	return 0;

fail2:
	free_irq(gpio_to_irq(MSP430_GPIO), keys);
fail1:
	input_free_device(input);
	kfree(keys);
	gpio_free(MSP430_GPIO);
	dev_err(&i2c->dev, "can't register, err %d\n", status);

	return status;
}

static int __devexit dm365evm_keys_remove(struct i2c_client *i2c)
{
	struct dm365evm_keys	*keys = i2c_get_clientdata(i2c);

	free_irq(gpio_to_irq(MSP430_GPIO), keys);
	input_unregister_device(keys->input);
	kfree(keys);

	gpio_free(MSP430_GPIO);

	return 0;
}

/* REVISIT:  add suspend/resume when DaVinci supports it.  The IRQ should
 * be able to wake up the system.  When device_may_wakeup(&i2c->dev), call
 * enable_irq_wake() on suspend, and disable_irq_wake() on resume.
 */

static const struct i2c_device_id ids[] = {
	{ .name = "dm365evm_keys", },
	{ },
};
MODULE_DEVICE_TABLE(i2c, ids);

static struct i2c_driver dm365evm_keys_driver = {
	.driver.name	= "dm365evm_keys",
	.id_table	= ids,
	.probe		= dm365evm_keys_probe,
	.remove		= __devexit_p(dm365evm_keys_remove),
};

static int __init dm365evm_keys_init(void)
{
	return i2c_add_driver(&dm365evm_keys_driver);
}
module_init(dm365evm_keys_init);

static void __exit dm365evm_keys_exit(void)
{
	i2c_del_driver(&dm365evm_keys_driver);
}
module_exit(dm365evm_keys_exit);

MODULE_LICENSE("GPL");
