/*
 * tps6507x.c  --  TPS6507x chip family multi-function driver
 *
 *  Copyright (c) 2010 RidgeRun (todd.fischer@ridgerun.com)
 *
 * Author: Todd Fischer
 *         todd.fischer@ridgerun.com
 *
 * Credits:
 *
 *    Using code from wm8350-i2c.c, Wolfson Microelectronics PLC.
 *
 * For licencing details see kernel-base/COPYING
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/mfd/tps6507x.h>

struct tps_sub_devices {
	const char *name;
	struct platform_device *pdev;
};

static struct tps_sub_devices tps6507x_sub_devices[] = {
	{ "tps6507x-pmic", NULL },
	{ "tps6507x-ts", NULL },
	{ NULL, NULL }
};

static int tps6507x_i2c_read_device(struct tps6507x_dev *tps6507x, char reg,
				  int bytes, void *dest)
{
	int ret;

	ret = i2c_master_send(tps6507x->i2c_client, &reg, 1);
	if (ret < 0)
		return ret;
	ret = i2c_master_recv(tps6507x->i2c_client, dest, bytes);
	if (ret < 0)
		return ret;
	if (ret != bytes)
		return -EIO;
	return 0;
}

static int tps6507x_i2c_write_device(struct tps6507x_dev *tps6507x, char reg,
				   int bytes, void *src)
{
	/* we add 1 byte for device register */
	u8 msg[(TPS6507X_MAX_REGISTER << 1) + 1];
	int ret;

	if (bytes > ((TPS6507X_MAX_REGISTER << 1) + 1))
		return -EINVAL;

	msg[0] = reg;
	memcpy(&msg[1], src, bytes);
	ret = i2c_master_send(tps6507x->i2c_client, msg, bytes + 1);
	if (ret < 0)
		return ret;
	if (ret != bytes + 1)
		return -EIO;
	return 0;
}

/*
 * Register a client device.  This is non-fatal since there is no need to
 * fail the entire device init due to a single platform device failing.
 */
static void tps6507x_client_dev_register(struct tps6507x_dev *tps6507x,
					 const char *name,
					 struct platform_device **pdev)
{
	int ret;

	*pdev = platform_device_alloc(name, -1);
	if (*pdev == NULL) {
		dev_err(tps6507x->dev, "Failed to allocate %s\n", name);
		return;
	}

	(*pdev)->dev.parent = tps6507x->dev;
	platform_set_drvdata(*pdev, tps6507x);
	ret = platform_device_add(*pdev);
	if (ret != 0) {
		dev_err(tps6507x->dev, "Failed to register %s: %d\n", name, ret);
		platform_device_put(*pdev);
		*pdev = NULL;
	}
}

int tps6507x_device_init(struct tps6507x_dev *tps6507x, int irq,
		       struct tps6507x_board *pdata)
{
	int ret = 0;
	struct tps_sub_devices *d = &tps6507x_sub_devices[0];

	while (d->name) {
		tps6507x_client_dev_register(tps6507x, d->name, &(d->pdev));
		d++;
	}

	return ret;
}

static int tps6507x_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct tps6507x_dev *tps6507x;
	int ret = 0;

	tps6507x = kzalloc(sizeof(struct tps6507x_dev), GFP_KERNEL);
	if (tps6507x == NULL) {
		kfree(i2c);
		return -ENOMEM;
	}

	i2c_set_clientdata(i2c, tps6507x);
	tps6507x->dev = &i2c->dev;
	tps6507x->i2c_client = i2c;
	tps6507x->read_dev = tps6507x_i2c_read_device;
	tps6507x->write_dev = tps6507x_i2c_write_device;
	mutex_init(&tps6507x->tps_lock);

	ret = tps6507x_device_init(tps6507x, i2c->irq, i2c->dev.platform_data);
	if (ret < 0)
		goto err;

	return ret;

err:
	kfree(tps6507x);
	return ret;
}

static int tps6507x_i2c_remove(struct i2c_client *i2c)
{
	struct tps6507x_dev *tps6507x = i2c_get_clientdata(i2c);
	struct tps_sub_devices *d = &tps6507x_sub_devices[0];

	while (d->name) {
		platform_device_unregister(d->pdev);
		d++;
	}

	kfree(tps6507x);

	return 0;
}

static const struct i2c_device_id tps6507x_i2c_id[] = {
       { "tps6507x", 0 },
       { }
};
MODULE_DEVICE_TABLE(i2c, tps6507x_i2c_id);


static struct i2c_driver tps6507x_i2c_driver = {
	.driver = {
		   .name = "tps6507x",
		   .owner = THIS_MODULE,
	},
	.probe = tps6507x_i2c_probe,
	.remove = tps6507x_i2c_remove,
	.id_table = tps6507x_i2c_id,
};

static int __init tps6507x_i2c_init(void)
{
	return i2c_add_driver(&tps6507x_i2c_driver);
}
/* init early so consumer devices can complete system boot */
subsys_initcall(tps6507x_i2c_init);

static void __exit tps6507x_i2c_exit(void)
{
	i2c_del_driver(&tps6507x_i2c_driver);
}
module_exit(tps6507x_i2c_exit);

MODULE_DESCRIPTION("TPS6507x chip family multi-function driver")
MODULE_LICENSE("GPL");
