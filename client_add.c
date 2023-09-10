
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "mydbus.h"

static void callback_add_async(GObject* proxy,
                               GAsyncResult* res,
                               gpointer user_data)
{
    printf("callback_add_async called!\n");
    int retval;
    GError* error = NULL;
    if (my_dbus_calculator_call_add_finish(MY_DBUS_CALCULATOR(proxy), &retval, res, &error)) {
        printf("Answer = %d\n", retval);
        exit(0);
    }
    else {
        printf("ERROR: %s\n", error ? error->message : "?");
        if (error) g_error_free(error);
    }
}


int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s num1 num2\n", argv[0]);
        return 0;
    }
    const int a = atoi(argv[1]);
    const int b = atoi(argv[2]);

    GError* error = NULL;
    MyDBusCalculator* proxy = my_dbus_calculator_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, "com.TestXmlInterface", "/", NULL/*GCancellable**/, &error);
    if (NULL == proxy) {
        printf("ERROR: %s\n", error ? error->message : "?");
        if (error) g_error_free(error);
        return -1;
    }

    my_dbus_calculator_call_add(proxy, a, b, NULL/*GCancellable**/, callback_add_async, NULL/*user_data*/);

    GMainLoop* loop = g_main_loop_new(NULL/*GMainContext**/, FALSE/*is_running*/);
    g_main_loop_run(loop);

    g_object_unref(proxy);

    return 0;
}
