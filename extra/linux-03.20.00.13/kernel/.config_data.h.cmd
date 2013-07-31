cmd_kernel/config_data.h := (echo "static const char kernel_config_data[] __used = MAGIC_START"; cat kernel/config_data.gz | scripts/bin2c; echo "MAGIC_END;") > kernel/config_data.h
