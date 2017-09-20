#include <stdio.h>
#include <string.h>
#include <shell/tash.h>

#include <artik_module.h>
#include <artik_gpio.h>

#include "command.h"

static int gpio_read(int argc, char *argv[]);
static int gpio_write(int argc, char *argv[]);

const struct command gpio_commands[] = {
	{ "read", "read <num>", gpio_read },
	{ "write", "write <num> <0|1>", gpio_write },
	{ "", "", NULL}
};

static int gpio_io(artik_gpio_dir_t dir, artik_gpio_id id, int new_value)
{
	artik_gpio_module *gpio;
	artik_gpio_config config;
	artik_gpio_handle handle;
	char name[16] = "";
	int ret = 0;

	gpio = (artik_gpio_module *)artik_request_api_module("gpio");
	if (!gpio) {
		fprintf(stderr, "GPIO module is not available\n");
		return -1;
	}

	memset(&config, 0, sizeof(config));
	config.dir = dir;
	config.id = id;
	snprintf(name, 16, "gpio%d", config.id);
	config.name = name;

	if (gpio->request(&handle, &config) != S_OK) {
		fprintf(stderr, "Failed to request GPIO %d\n", config.id);
		ret = -1;
		goto exit;
	}

	if (dir == GPIO_IN) {
		ret = gpio->write(handle, new_value);
		if (ret != S_OK) {
			fprintf(stderr, "Failed to write GPIO %d [err %d]\n", config.id, ret);
		} else {
			fprintf(stdout, "Write %d to GPIO %d\n", new_value, config.id);
		}
	}
	else {
		ret = gpio->read(handle);
		if (ret < 0) {
			fprintf(stderr, "Failed to read GPIO %d [err %d]\n", config.id, ret);
		} else {
			fprintf(stdout, "The value read in GPIO %d is %d\n", config.id, ret);
		}
	}

	gpio->release(handle);

exit:
	artik_release_api_module(gpio);
	return ret;
}

static int gpio_read(int argc, char *argv[])
{
	if (argc < 4) {
		usage(argv[1], gpio_commands);
		return -1;
	}

	return gpio_io(GPIO_OUT, atoi(argv[3]), 0);
}

static int gpio_write(int argc, char *argv[])
{
	if (argc < 5) {
		usage(argv[1], gpio_commands);
		return -1;
	}

	return gpio_io(GPIO_IN, atoi(argv[3]), atoi(argv[4]));
}

int gpio_main(int argc, char *argv[])
{
	return commands_parser(argc, argv, gpio_commands);
}

#ifdef CONFIG_EXAMPLES_ARTIK_GPIO
static tash_cmdlist_t gpio_cmds[] = {
    {"gpio", gpio_main, TASH_EXECMD_SYNC},
    {NULL, NULL, 0}
};


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int artik_gpio_main(int argc, char *argv[])
#endif
{
#ifdef CONFIG_TASH
    /* add tash command */
    tash_cmdlist_install(gpio_cmds);
#endif

    return 0;
}
#endif

