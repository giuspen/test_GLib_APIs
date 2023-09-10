
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "mydbus.h"

static gboolean on_handle_add(MyDBusCalculator* interface,
                              GDBusMethodInvocation* invocation,
                              const gint num1,
                              const gint num2,
                              gpointer user_data)
{
    const int ans = num1 + num2;
    printf("Added %d and %d.\n", num1, num2);
    printf("Press enter to send message to client ...");
    getchar();
    my_dbus_calculator_complete_add(interface, invocation, ans);
    printf("...Message sent!\n");
    return TRUE;
}

static gboolean on_handle_sub(MyDBusCalculator* interface,
                              GDBusMethodInvocation* invocation,
                              const gint num1,
                              const gint num2,
                              gpointer user_data)
{
    const int ans = num1 - num2;
    my_dbus_calculator_complete_sub(interface, invocation, ans);
    printf("Subtracted %d from %d.\n", num2, num1);
    return TRUE;
}

static gboolean emit_alarm_cb(gpointer interface)
{
    my_dbus_alarm_emit_beep(MY_DBUS_ALARM(interface));
    my_dbus_alarm_set_activated(MY_DBUS_ALARM(interface), FALSE);
    return FALSE;
}

static gboolean on_handle_configure_alarm(MyDBusAlarm* interface,
                                          GDBusMethodInvocation* invocation,
                                          const guint seconds,
                                          gpointer user_data)
{
    if (my_dbus_alarm_get_activated(interface)) {
        g_dbus_method_invocation_return_error_literal(invocation, G_IO_ERROR, G_IO_ERROR_EXISTS, "Alarm already exists");
        return TRUE;
    }
    my_dbus_alarm_set_activated(interface, TRUE);
    g_timeout_add_seconds(seconds, emit_alarm_cb, interface);
    my_dbus_alarm_complete_configure_alarm(interface, invocation);
    return TRUE;
}

static void on_name_acquired(GDBusConnection* connection,
                             const gchar* name,
                             gpointer user_data)
{
    {
        MyDBusCalculator* interface = my_dbus_calculator_skeleton_new();
        g_signal_connect(interface, "handle-add", G_CALLBACK(on_handle_add), NULL/*user_data*/);
        g_signal_connect(interface, "handle-sub", G_CALLBACK(on_handle_sub), NULL/*user_data*/);
        GError* error = NULL;
        if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, "/", &error)) {
            printf("ERROR: %s\n", error ? error->message : "?");
            if (error) g_error_free(error);
        }
    }
    {
        MyDBusAlarm* alarm_interface = my_dbus_alarm_skeleton_new();
        g_signal_connect(alarm_interface, "handle-configure-alarm", G_CALLBACK(on_handle_configure_alarm), NULL/*user_data*/);
        GError* error = NULL;
        if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(alarm_interface), connection, "/", &error)) {
            printf("ERROR: %s\n", error ? error->message : "?");
            if (error) g_error_free(error);
        }
    }
}

int main(int argc, char** argv)
{
    GMainLoop* loop = g_main_loop_new(NULL/*GMainContext**/, FALSE/*is_running*/);
    g_bus_own_name(G_BUS_TYPE_SESSION, "com.TestXmlInterface", G_BUS_NAME_OWNER_FLAGS_NONE, NULL/*bus_acquired_handler*/, on_name_acquired, NULL/*name_lost_handler*/, NULL/*user_data*/, NULL/*user_data_free_func*/);
    g_main_loop_run(loop);
    return 0;
}
