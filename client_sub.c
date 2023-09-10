
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "mydbus.h"

int main(int argc , char **argv)
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

    int retval;
    if (my_dbus_calculator_call_sub_sync(proxy, a, b, &retval, NULL/*GCancellable**/, &error)) {
        printf("%d - %d = %d\n", a, b, retval);
    }
    else {
        printf("ERROR: %s\n", error ? error->message : "?");
        if (error) g_error_free(error);
    }

    g_object_unref(proxy);

    return 0;
}
