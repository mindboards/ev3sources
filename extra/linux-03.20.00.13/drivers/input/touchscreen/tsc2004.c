/*
 * drivers/input/touchscreen/tsc2004.c
 *
 * Copyright (C) 2009 Texas Instruments Inc
 * Author: Vaibhav Hiremath <hvaibhav@ti.com>
 *
 * Using code from:
 *  - tsc2007.c
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/i2c/tsc2004.h>


#define TS_POLL_DELAY			1 /* ms delay between samples */
#define TS_POLL_PERIOD			1 /* ms delay between samples */

/* Control byte 0 */
#define TSC2004_CMD0(addr, pnd, rw) ((addr<<3)|(pnd<<1)|rw)
/* Control byte 1 */
#define TSC2004_CMD1(cmd, mode, rst) ((1<<7)|(cmd<<4)|(mode<<2)|(rst<<1))

/* Command Bits */
#define READ_REG	1
#define WRITE_REG	0
#define SWRST_TRUE	1
#define SWRST_FALSE	0
#define PND0_TRUE	1
#define PND0_FALSE	0

/* Converter function mapping */
enum convertor_function {
	MEAS_X_Y_Z1_Z2,	/* Measure X,Y,z1 and Z2:	0x0 */
	MEAS_X_Y,	/* Measure X and Y only:	0x1 */
	MEAS_X,		/* Measure X only:		0x2 */
	MEAS_Y,		/* Measure Y only:		0x3 */
	MEAS_Z1_Z2,	/* Measure Z1 and Z2 only:	0x4 */
	MEAS_AUX,	/* Measure Auxillary input:	0x5 */
	MEAS_TEMP1,	/* Measure Temparature1:	0x6 */
	MEAS_TEMP2,	/* Measure Temparature2:	0x7 */
	MEAS_AUX_CONT,	/* Continuously measure Auxillary input: 0x8 */
	X_DRV_TEST,	/* X-Axis drivers tested 	0x9 */
	Y_DRV_TEST,	/* Y-Axis drivers tested 	0xA */
	/*Command Reserved*/
	SHORT_CKT_TST = 0xC,	/* Short circuit test:	0xC */
	XP_XN_DRV_STAT,	/* X+,Y- drivers status:	0xD */
	YP_YN_DRV_STAT,	/* X+,Y- drivers status:	0xE */
	YP_XN_DRV_STAT	/* Y+,X- drivers status:	0xF */
};

/* Register address mapping */
enum register_address {
	X_REG,		/* X register:		0x0 */
	Y_REG,		/* Y register:		0x1 */
	Z1_REG,		/* Z1 register:		0x2 */
	Z2_REG,		/* Z2 register:		0x3 */
	AUX_REG,	/* AUX register:	0x4 */
	TEMP1_REG,	/* Temp1 register:	0x5 */
	TEMP2_REG,	/* Temp2 register:	0x6 */
	STAT_REG,	/* Status Register:	0x7 */
	AUX_HGH_TH_REG,	/* AUX high threshold register:	0x8 */
	AUX_LOW_TH_REG,	/* AUX low threshold register:	0x9 */
	TMP_HGH_TH_REG,	/* Temp high threshold register:0xA */
	TMP_LOW_TH_REG,	/* Temp low threshold register:	0xB */
	CFR0_REG,	/* Configuration register 0:	0xC */
	CFR1_REG,	/* Configuration register 1:	0xD */
	CFR2_REG,	/* Configuration register 2:	0xE */
	CONV_FN_SEL_STAT	/* Convertor function select register:	0xF */
};

/* Supported Resolution modes */
enum resolution_mode {
	MODE_10BIT,	/* 10 bit resolution */
	MODE_12BIT		/* 12 bit resolution */
};

/* Configuraton register bit fields */
/* CFR0 */
#define PEN_STS_CTRL_MODE	(1<<15)
#define ADC_STS			(1<<14)
#define RES_CTRL		(1<<13)
#define ADC_CLK_4MHZ		(0<<11)
#define ADC_CLK_2MHZ		(1<<11)
#define ADC_CLK_1MHZ		(2<<11)
#define PANEL_VLTG_STB_TIME_0US		(0<<8)
#define PANEL_VLTG_STB_TIME_100US	(1<<8)
#define PANEL_VLTG_STB_TIME_500US	(2<<8)
#define PANEL_VLTG_STB_TIME_1MS		(3<<8)
#define PANEL_VLTG_STB_TIME_5MS		(4<<8)
#define PANEL_VLTG_STB_TIME_10MS	(5<<8)
#define PANEL_VLTG_STB_TIME_50MS	(6<<8)
#define PANEL_VLTG_STB_TIME_100MS	(7<<8)

/* CFR2 */
#define PINTS1			(1<<15)
#define PINTS0			(1<<14)
#define MEDIAN_VAL_FLTR_SIZE_1	(0<<12)
#define MEDIAN_VAL_FLTR_SIZE_3	(1<<12)
#define MEDIAN_VAL_FLTR_SIZE_7	(2<<12)
#define MEDIAN_VAL_FLTR_SIZE_15	(3<<12)
#define AVRG_VAL_FLTR_SIZE_1	(0<<10)
#define AVRG_VAL_FLTR_SIZE_3_4	(1<<10)
#define AVRG_VAL_FLTR_SIZE_7_8	(2<<10)
#define AVRG_VAL_FLTR_SIZE_16	(3<<10)
#define MAV_FLTR_EN_X		(1<<4)
#define MAV_FLTR_EN_Y		(1<<3)
#define MAV_FLTR_EN_Z		(1<<2)

#define	MAX_12BIT		((1 << 12) - 1)
#define MEAS_MASK		0xFFF

struct ts_event {
	u16	x;
	u16	y;
	u16	z1, z2;
};

struct tsc2004 {
	struct input_dev	*input;
	char			phys[32];
	struct delayed_work	work;

	struct i2c_client	*client;

	u16			model;
	u16			x_plate_ohms;

	bool			pendown;
	int			irq;

	int			(*get_pendown_state)(void);
	void			(*clear_penirq)(void);
};

static inline int tsc2004_read_word_data(struct tsc2004 *tsc, u8 cmd)
{
	s32 data;
	u16 val;

	data = i2c_smbus_read_word_data(tsc->client, cmd);
	if (data < 0) {
		dev_err(&tsc->client->dev, "i2c io (read) error: %d\n", data);
		return data;
	}

	/* The protocol and raw data format from i2c interface:
	 * S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P
	 * Where DataLow has [D11-D4], DataHigh has [D3-D0 << 4 | Dummy 4bit].
	 */
	val = swab16(data) >> 4;

	dev_dbg(&tsc->client->dev, "data: 0x%x, val: 0x%x\n", data, val);

	return val;
}

static inline int tsc2004_write_word_data(struct tsc2004 *tsc, u8 cmd, u16 data)
{
	u16 val;

	val = swab16(data);
	return i2c_smbus_write_word_data(tsc->client, cmd, val);
}

static inline int tsc2004_write_cmd(struct tsc2004 *tsc, u8 value)
{
	return i2c_smbus_write_byte(tsc->client, value);
}

static int tsc2004_prepare_for_reading(struct tsc2004 *ts)
{
	int err;
	int cmd, data;

	/* Reset the TSC, configure for 12 bit */
	cmd = TSC2004_CMD1(MEAS_X_Y_Z1_Z2, MODE_12BIT, SWRST_TRUE);
	err = tsc2004_write_cmd(ts, cmd);
	if (err < 0)
		return err;

	/* Enable interrupt for PENIRQ and DAV */
	cmd = TSC2004_CMD0(CFR2_REG, PND0_FALSE, WRITE_REG);
	data = PINTS1 | PINTS0 | MEDIAN_VAL_FLTR_SIZE_15 |
		AVRG_VAL_FLTR_SIZE_7_8 | MAV_FLTR_EN_X | MAV_FLTR_EN_Y |
		MAV_FLTR_EN_Z;
	err = tsc2004_write_word_data(ts, cmd, data);
	if (err < 0)
		return err;

	/* Configure the TSC in TSMode 1 */
	cmd = TSC2004_CMD0(CFR0_REG, PND0_FALSE, WRITE_REG);
	data = PEN_STS_CTRL_MODE | ADC_CLK_2MHZ | PANEL_VLTG_STB_TIME_1MS;
	err = tsc2004_write_word_data(ts, cmd, data);
	if (err < 0)
		return err;

	/* Enable x, y, z1 and z2 conversion functions */
	cmd = TSC2004_CMD1(MEAS_X_Y_Z1_Z2, MODE_12BIT, SWRST_FALSE);
	err = tsc2004_write_cmd(ts, cmd);
	if (err < 0)
		return err;

	return 0;
}

static void tsc2004_read_values(struct tsc2004 *tsc, struct ts_event *tc)
{
	int cmd;

	/* Read X Measurement */
	cmd = TSC2004_CMD0(X_REG, PND0_FALSE, READ_REG);
	tc->x = tsc2004_read_word_data(tsc, cmd);

	/* Read Y Measurement */
	cmd = TSC2004_CMD0(Y_REG, PND0_FALSE, READ_REG);
	tc->y = tsc2004_read_word_data(tsc, cmd);

	/* Read Z1 Measurement */
	cmd = TSC2004_CMD0(Z1_REG, PND0_FALSE, READ_REG);
	tc->z1 = tsc2004_read_word_data(tsc, cmd);

	/* Read Z2 Measurement */
	cmd = TSC2004_CMD0(Z2_REG, PND0_FALSE, READ_REG);
	tc->z2 = tsc2004_read_word_data(tsc, cmd);


	tc->x &= MEAS_MASK;
	tc->y &= MEAS_MASK;
	tc->z1 &= MEAS_MASK;
	tc->z2 &= MEAS_MASK;

	/* Prepare for touch readings */
	if (tsc2004_prepare_for_reading(tsc) < 0)
		dev_dbg(&tsc->client->dev, "Failed to prepare TSC for next"
				"reading\n");
}

static u32 tsc2004_calculate_pressure(struct tsc2004 *tsc, struct ts_event *tc)
{
	u32 rt = 0;

	/* range filtering */
	if (tc->x == MAX_12BIT)
		tc->x = 0;

	if (likely(tc->x && tc->z1)) {
		/* compute touch pressure resistance using equation #1 */
		rt = tc->z2 - tc->z1;
		rt *= tc->x;
		rt *= tsc->x_plate_ohms;
		rt /= tc->z1;
		rt = (rt + 2047) >> 12;
	}

	return rt;
}

static void tsc2004_send_up_event(struct tsc2004 *tsc)
{
	struct input_dev *input = tsc->input;

	dev_dbg(&tsc->client->dev, "UP\n");

	input_report_key(input, BTN_TOUCH, 0);
	input_report_abs(input, ABS_PRESSURE, 0);
	input_sync(input);
}

static void tsc2004_work(struct work_struct *work)
{
	struct tsc2004 *ts =
		container_of(to_delayed_work(work), struct tsc2004, work);
	struct ts_event tc;
	u32 rt;

	/*
	 * NOTE: We can't rely on the pressure to determine the pen down
	 * state, even though this controller has a pressure sensor.
	 * The pressure value can fluctuate for quite a while after
	 * lifting the pen and in some cases may not even settle at the
	 * expected value.
	 *
	 * The only safe way to check for the pen up condition is in the
	 * work function by reading the pen signal state (it's a GPIO
	 * and IRQ). Unfortunately such callback is not always available,
	 * in that case we have rely on the pressure anyway.
	 */
	if (ts->get_pendown_state) {
		if (unlikely(!ts->get_pendown_state())) {
			tsc2004_send_up_event(ts);
			ts->pendown = false;
			goto out;
		}

		dev_dbg(&ts->client->dev, "pen is still down\n");
	}

	tsc2004_read_values(ts, &tc);

	rt = tsc2004_calculate_pressure(ts, &tc);
	if (rt > MAX_12BIT) {
		/*
		 * Sample found inconsistent by debouncing or pressure is
		 * beyond the maximum. Don't report it to user space,
		 * repeat at least once more the measurement.
		 */
		dev_dbg(&ts->client->dev, "ignored pressure %d\n", rt);
		goto out;

	}

	if (rt) {
		struct input_dev *input = ts->input;

		if (!ts->pendown) {
			dev_dbg(&ts->client->dev, "DOWN\n");

			input_report_key(input, BTN_TOUCH, 1);
			ts->pendown = true;
		}

		input_report_abs(input, ABS_X, tc.x);
		input_report_abs(input, ABS_Y, tc.y);
		input_report_abs(input, ABS_PRESSURE, rt);

		input_sync(input);

		dev_dbg(&ts->client->dev, "point(%4d,%4d), pressure (%4u)\n",
			tc.x, tc.y, rt);

	} else if (!ts->get_pendown_state && ts->pendown) {
		/*
		 * We don't have callback to check pendown state, so we
		 * have to assume that since pressure reported is 0 the
		 * pen was lifted up.
		 */
		tsc2004_send_up_event(ts);
		ts->pendown = false;
	}

 out:
	if (ts->pendown)
		schedule_delayed_work(&ts->work,
				      msecs_to_jiffies(TS_POLL_PERIOD));
	else
		enable_irq(ts->irq);
}

static irqreturn_t tsc2004_irq(int irq, void *handle)
{
	struct tsc2004 *ts = handle;

	if (!ts->get_pendown_state || likely(ts->get_pendown_state())) {
		disable_irq_nosync(ts->irq);
		schedule_delayed_work(&ts->work,
				      msecs_to_jiffies(TS_POLL_DELAY));
	}

	if (ts->clear_penirq)
		ts->clear_penirq();

	return IRQ_HANDLED;
}

static void tsc2004_free_irq(struct tsc2004 *ts)
{
	free_irq(ts->irq, ts);
	if (cancel_delayed_work_sync(&ts->work)) {
		/*
		 * Work was pending, therefore we need to enable
		 * IRQ here to balance the disable_irq() done in the
		 * interrupt handler.
		 */
		enable_irq(ts->irq);
	}
}

static int __devinit tsc2004_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct tsc2004 *ts;
	struct tsc2004_platform_data *pdata = pdata = client->dev.platform_data;
	struct input_dev *input_dev;
	int err;

	if (!pdata) {
		dev_err(&client->dev, "platform data is required!\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_READ_WORD_DATA))
		return -EIO;

	ts = kzalloc(sizeof(struct tsc2004), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!ts || !input_dev) {
		err = -ENOMEM;
		goto err_free_mem;
	}

	ts->client = client;
	ts->irq = client->irq;
	ts->input = input_dev;
	INIT_DELAYED_WORK(&ts->work, tsc2004_work);

	ts->model             = pdata->model;
	ts->x_plate_ohms      = pdata->x_plate_ohms;
	ts->get_pendown_state = pdata->get_pendown_state;
	ts->clear_penirq      = pdata->clear_penirq;

	snprintf(ts->phys, sizeof(ts->phys),
		 "%s/input0", dev_name(&client->dev));

	input_dev->name = "TSC2004 Touchscreen";
	input_dev->phys = ts->phys;
	input_dev->id.bustype = BUS_I2C;

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

	input_set_abs_params(input_dev, ABS_X, 0, MAX_12BIT, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, MAX_12BIT, 0, 0);
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, MAX_12BIT, 0, 0);

	if (pdata->init_platform_hw)
		pdata->init_platform_hw();

	err = request_irq(ts->irq, tsc2004_irq, IRQF_TRIGGER_FALLING,
			client->dev.driver->name, ts);
	if (err < 0) {
		dev_err(&client->dev, "irq %d busy?\n", ts->irq);
		goto err_free_mem;
	}

	/* Prepare for touch readings */
	err = tsc2004_prepare_for_reading(ts);
	if (err < 0)
		goto err_free_irq;

	err = input_register_device(input_dev);
	if (err)
		goto err_free_irq;

	i2c_set_clientdata(client, ts);

	return 0;

 err_free_irq:
	tsc2004_free_irq(ts);
	if (pdata->exit_platform_hw)
		pdata->exit_platform_hw();
 err_free_mem:
	input_free_device(input_dev);
	kfree(ts);
	return err;
}

static int __devexit tsc2004_remove(struct i2c_client *client)
{
	struct tsc2004	*ts = i2c_get_clientdata(client);
	struct tsc2004_platform_data *pdata = client->dev.platform_data;

	tsc2004_free_irq(ts);

	if (pdata->exit_platform_hw)
		pdata->exit_platform_hw();

	input_unregister_device(ts->input);
	kfree(ts);

	return 0;
}

static struct i2c_device_id tsc2004_idtable[] = {
	{ "tsc2004", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, tsc2004_idtable);

static struct i2c_driver tsc2004_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "tsc2004"
	},
	.id_table	= tsc2004_idtable,
	.probe		= tsc2004_probe,
	.remove		= __devexit_p(tsc2004_remove),
};

static int __init tsc2004_init(void)
{
	return i2c_add_driver(&tsc2004_driver);
}

static void __exit tsc2004_exit(void)
{
	i2c_del_driver(&tsc2004_driver);
}

module_init(tsc2004_init);
module_exit(tsc2004_exit);

MODULE_AUTHOR("Vaibhav Hiremath <hvaibhav@ti.com>");
MODULE_DESCRIPTION("TSC2004 TouchScreen Driver");
MODULE_LICENSE("GPL");
