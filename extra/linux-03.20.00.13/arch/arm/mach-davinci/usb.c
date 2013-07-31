/*
 * USB
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/string.h>

#include <linux/usb/musb.h>
#include <linux/usb/otg.h>
#include <mach/usb_musb.h>

static struct musb_hdrc_eps_bits musb_eps[] = {
	{ "ep1_tx", 8, },
	{ "ep1_rx", 8, },
	{ "ep2_tx", 8, },
	{ "ep2_rx", 8, },
	{ "ep3_tx", 5, },
	{ "ep3_rx", 5, },
	{ "ep4_tx", 5, },
	{ "ep4_rx", 5, },
};

static struct musb_hdrc_config musb_config = {
	.multipoint	= true,
	.dyn_fifo	= true,
	.soft_con	= true,
	.dma		= true,

	.num_eps	= 5,
	.dma_channels	= 8,
	.ram_bits	= 10,
	.eps_bits	= musb_eps,
};

static u64 usb_dmamask = DMA_BIT_MASK(32);

void __init setup_usb(struct usb_plat_data *pdata)
{
	u8	ninst = pdata->num_inst;
	u8	inst, i = 0;
	char	name[20] = "musb_hdrc";
	struct	platform_device *pdev;
	struct	plat_res_data *plat_res_data;

	do {
		plat_res_data = &pdata->prdata[i++];
		inst = plat_res_data->plat_data->inst;
		if (pdata->num_inst > 1)
			sprintf(name, "musb_hdrc%d", inst);

		pdev = platform_device_alloc(name, -1);

		if (!pdev) {
			pr_warning("WARNING: USB platform Alloc Failed\n");
			break;
}

		/* Verify whether the clock information has already been
		 * specified if so do not override it with generic definition.
		 */
		if (!plat_res_data->plat_data->clock)
			plat_res_data->plat_data->clock = "usb";
		pdev->dev.platform_data = plat_res_data->plat_data;
		pdev->dev.dma_mask = &usb_dmamask,
		pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
		pdev->resource = plat_res_data->res_data;
		pdev->num_resources = plat_res_data->num_res;

		/* Associate the default configuration if not specified */
		if (!plat_res_data->plat_data->config)
			plat_res_data->plat_data->config = &musb_config;
		platform_device_add(pdev);
	} while (--ninst);
}
