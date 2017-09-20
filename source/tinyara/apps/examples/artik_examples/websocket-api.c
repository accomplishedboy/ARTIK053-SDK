#include <stdio.h>
#include <string.h>
#include <shell/tash.h>

#include <artik_module.h>
#include <artik_websocket.h>

#include "command.h"

#define	FAIL_AND_EXIT(x)		{ fprintf(stderr, x); usage(); ret = -1; goto exit; }

static int websocket_connect(int argc, char *argv[]);
static int websocket_disconnect(int argc, char *argv[]);
static int websocket_send(int argc, char *argv[]);

static artik_websocket_handle g_handle = NULL;
static artik_websocket_module *g_websocket = NULL;
static artik_websocket_config g_config;

const struct command websocket_commands[] = {
	{ "connect", "connect <uri>\n\t\t <uri> - example: wss://echo.websocket.org/", websocket_connect },
	{ "disconnect", "disconnect", websocket_disconnect },
	{ "send", "send <message>", websocket_send },
	{ "", "", NULL }
};

static void websocket_rx_callback(void *user_data, void *result)
{
    if (result) {
        fprintf(stderr, "RX: %s\n", (char*)result);
        free(result);
    }
}

static void websocket_connection_callback(void *user_data, void *result)
{
    artik_websocket_connection_state state = (artik_websocket_connection_state)result;

    if (state == ARTIK_WEBSOCKET_CLOSED)
        fprintf(stderr, "Websocket connection has been closed\n");
    else if (state == ARTIK_WEBSOCKET_CONNECTED)
        fprintf(stderr, "Websocket is connected\n");
}

static int websocket_connect(int argc, char *argv[])
{
    artik_error ret = S_OK;

    if (argc < 4) {
		fprintf(stderr, "Missing uri parameter\n");
		usage(argv[1], websocket_commands);
		return -1;
	}

    g_websocket = (artik_websocket_module *)artik_request_api_module("websocket");
    if (!g_websocket) {
        fprintf(stderr, "Websocket module is not available\n");
        return -1;
    }

    memset(&g_config, 0, sizeof(artik_websocket_config));
    g_config.uri = argv[3];

    ret = g_websocket->websocket_request(&g_handle, &g_config);
    if (ret != S_OK) {
        fprintf(stderr, "Failed to request websocket (ret=%d)\n", ret);
        return -1;
    }

    ret = g_websocket->websocket_open_stream(g_handle);
    if (ret != S_OK) {
        fprintf(stderr, "Failed to open websocket (ret=%d)\n", ret);
        return -1;
    }

    ret = g_websocket->websocket_set_receive_callback(g_handle, websocket_rx_callback, NULL);
    if (ret != S_OK) {
        fprintf(stderr, "Failed to register rx callback (ret=%d)\n", ret);
        return -1;
    }

    ret = g_websocket->websocket_set_connection_callback(g_handle, websocket_connection_callback, NULL);
    if (ret != S_OK) {
        fprintf(stderr, "Failed to register connection callback (ret=%d)\n", ret);
        return -1;
    }

    return 0;
}

static int websocket_disconnect(int argc, char *argv[])
{
    if (!g_websocket || !g_handle) {
        fprintf(stderr, "Websocket is not connected\n");
        return -1;
    }

    g_websocket->websocket_close_stream(g_handle);

    artik_release_api_module(g_websocket);
    g_websocket = NULL;
    g_handle = NULL;

    return 0;
}

static int websocket_send(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Missing message parameter\n");
		usage(argv[1], websocket_commands);
		return -1;
	}

    if (!g_websocket || !g_handle) {
        fprintf(stderr, "Websocket is not connected\n");
        return -1;
    }

    g_websocket->websocket_write_stream(g_handle, argv[3]);

    return 0;
}

int ws_main(int argc, char *argv[])
{
	return commands_parser(argc, argv, websocket_commands);
}

#ifdef CONFIG_EXAMPLES_ARTIK_WEBSOCKET

void StartWifiConnection(void);

static tash_cmdlist_t ws_cmds[] = {
    {"websocket", ws_main, TASH_EXECMD_SYNC},
    {NULL, NULL, 0}
};


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int artik_websocket_main(int argc, char *argv[])
#endif
{
#ifdef CONFIG_TASH
    /* add tash command */
    tash_cmdlist_install(ws_cmds);
#endif

    StartWifiConnection();

    return 0;
}
#endif

