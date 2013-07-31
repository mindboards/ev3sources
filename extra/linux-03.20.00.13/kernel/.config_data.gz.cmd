cmd_kernel/config_data.gz := (cat .config | gzip -f -9 > kernel/config_data.gz) || (rm -f kernel/config_data.gz ; false)
