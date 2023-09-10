
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "mydbus.h"

static void beep_cb(MyDBusAlarm *proxy, gpointer user_data)
{
    printf("Beep!\a\n");
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s seconds\n", argv[0]);
        return 0;
    }
    const int seconds = atoi(argv[1]);

    GError* error = NULL;
    MyDBusAlarm* proxy = my_dbus_alarm_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, "com.TestXmlInterface", "/", NULL/*GCancellable**/, &error);
    if (NULL == proxy) {
        printf("ERROR: %s\n", error ? error->message : "?");
        if (error) g_error_free(error);
        return -1;
    }

    g_signal_connect(proxy, "beep", G_CALLBACK(beep_cb) , NULL/*user_data*/);

    if (my_dbus_alarm_call_configure_alarm_sync(proxy, seconds, NULL, &error)) {
        printf("Alarm set for after %d seconds. Waiting for it to ring...\n", seconds);
        GMainLoop* loop = g_main_loop_new(NULL/*GMainContext**/, FALSE/*is_running*/);
        g_main_loop_run(loop);
    }
    else {
        printf("Error setting alarm!\n");
    }

    return 0;
}
