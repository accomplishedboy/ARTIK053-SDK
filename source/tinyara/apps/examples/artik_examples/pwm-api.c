#include <stdio.h>
#include <string.h>
#include <shell/tash.h>

#include <artik_module.h>
#include <artik_pwm.h>

#include "command.h"

static artik_pwm_handle pwm_handle = NULL;

static int pwm_start(int argc, char *argv[]);
static int pwm_stop(int argc, char *argv[]);

const struct command pwm_commands[] = {
	{ "start", "start <pin num> <period> <duty cycle> [invert]", pwm_start },
	{ "stop", "stop", pwm_stop},
	{"", "", NULL}
};

static int pwm_start(int argc, char *argv[])
{
	artik_pwm_module *pwm = NULL;
	int ret = 0;

	artik_pwm_config config;
	char name[16] = "";

	if (argc < 6) {
		fprintf(stderr, "Wrong number of arguments\n");
		usage(argv[1], pwm_commands);
		return -1;
	}

	pwm = (artik_pwm_module *)artik_request_api_module("pwm");
	if (!pwm) {
		fprintf(stderr, "PWM module is not available\n");
		return -1;
	}

	if (pwm_handle) {
		fprintf(stderr, "PW was already started\n");
		ret = -1;
		goto exit;
	}

	memset(&config, 0, sizeof(config));
	config.pin_num = atoi(argv[3]);
	config.period = atoi(argv[4]);
	config.duty_cycle = atoi(argv[5]);
	if ((argc > 5) && !strcmp(argv[6], "invert")) {
		config.polarity = ARTIK_PWM_POLR_INVERT;
	}

	snprintf(name, 16, "pwm%d", config.pin_num);
	config.name = name;

	if (pwm->request(&pwm_handle, &config) != S_OK) {
		fprintf(stderr, "Failed to request PWM %d\n", config.pin_num);
		ret = -1;
		goto exit;
	}

exit:
	artik_release_api_module(pwm);
	return ret;
}

static int pwm_stop(int argc, char *argv[])
{
	int ret = 0;
	artik_pwm_module *pwm = (artik_pwm_module *)artik_request_api_module("pwm");
	if (!pwm) {
		fprintf(stderr, "PWM module is not available\n");
		return -1;
	}

	if (!pwm_handle) {
		fprintf(stderr, "PWM was not started\n");
		ret = -1;
		goto exit;
	}

	pwm->release(pwm_handle);
	pwm_handle = NULL;

exit:
	artik_release_api_module(pwm);
	return ret;
}

int pwm_main(int argc, char *argv[])
{
	return commands_parser(argc, argv, pwm_commands);
}

#ifdef CONFIG_EXAMPLES_ARTIK_PWM
static tash_cmdlist_t pwm_cmds[] = {
    {"pwm", pwm_main, TASH_EXECMD_SYNC},
    {NULL, NULL, 0}
};


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int artik_pwm_main(int argc, char *argv[])
#endif
{
#ifdef CONFIG_TASH
    /* add tash command */
    tash_cmdlist_install(pwm_cmds);
#endif

    return 0;
}
#endif

