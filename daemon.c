#include <fcntl.h>
#include <gio/gunixsocketaddress.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vte/vte.h>


#define MSG_SIZE 4096

struct term_options
{
    char **argv;
};


void
child_exited(GObject *obj, GParamSpec *pspec, gpointer data)
{
    (void)obj;
    (void)pspec;
    GtkWidget *win = (GtkWidget *)data;

    gtk_widget_destroy(win);
}

gboolean
setup_term(GtkWidget *win, GtkWidget *term, struct term_options *to)
{
    PangoFontDescription *font_desc = NULL;
    gboolean r;
    char *args_default[] = { "/bin/bash", "-l", "-i", NULL };
    char **args_use;

    if (to->argv != NULL)
        args_use = to->argv;
    else
        args_use = args_default;

    /* Appearance */
    font_desc = pango_font_description_from_string("Ttyp0 10");
    vte_terminal_set_font(VTE_TERMINAL(term), font_desc);

    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term), VTE_CURSOR_BLINK_OFF);

    /* Spawn child */
    g_signal_connect(G_OBJECT(term), "child-exited",
                     G_CALLBACK(child_exited), win);
    r = vte_terminal_spawn_sync(VTE_TERMINAL(term), VTE_PTY_DEFAULT, NULL,
                                args_use, NULL, G_SPAWN_DEFAULT, NULL, NULL,
                                NULL, NULL, NULL);
    return r;
}

void
setup_css(void)
{
    /* Style provider for this screen. */
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen,
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider,
                                    "GtkWindow { background-color: #000000; }",
                                    -1, NULL);
    g_object_unref(provider);
}

void
setup_window(GtkWidget *win)
{
    g_signal_connect(G_OBJECT(win), "delete-event",
                     G_CALLBACK(gtk_main_quit), NULL);
}

gboolean
term_new(gpointer user_data)
{
    GtkWidget *term, *win;
    struct term_options *to = (struct term_options *)user_data;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    setup_window(win);
    term = vte_terminal_new();
    setup_term(win, term, to);
    gtk_container_add(GTK_CONTAINER(win), term);
    gtk_widget_show_all(win);

    free(to);

    /* Remove this source. */
    return FALSE;
}

gboolean
sock_incoming(GSocketService *service, GSocketConnection *connection,
              GObject *source_object, gpointer user_data)
{
    (void)service;
    (void)source_object;
    (void)user_data;

    gchar message[MSG_SIZE] = "", *p;
    GInputStream* s;
    gssize sz_read;
    GSList *args = NULL;
    struct term_options *to = NULL;
    guint args_i;

    to = calloc(sizeof(struct term_options), 1);
    
    s = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    sz_read = g_input_stream_read(s, message, MSG_SIZE, NULL, NULL);

    for (p = message; (p - message) < sz_read; p++)
    {
        switch (*p)
        {
            case 'O':
                /* Set options. Not yet implemented. */
                break;
            case 'A':
                /* After the 'A' follows a NUL terminated string. Add
                 * this string to our list of arguments. */
                p++;
                args = g_slist_append(args, p);
                /* Jump over the string. */
                while (*p != 0 && (p - message) < sz_read)
                    p++;
                break;
        }
    }

    if (args != NULL)
    {
        to->argv = calloc(g_slist_length(args) + 1, sizeof(char *));
        for (args_i = 0; args_i < g_slist_length(args); args_i++)
            to->argv[args_i] = (char *)(g_slist_nth(args, args_i)->data);
    }

    /* We're not on the main thread. */
    g_main_context_invoke(NULL, term_new, to);

    /* Other handlers must not be called. */
    return TRUE;
}

void
socket_listen(char *suffix)
{
    GSocketService *sock;
    GSocketAddress *sa;
    GError *err = NULL;
    char *name, *path;
    GdkDisplay *display = gdk_display_get_default();

    name = g_strdup_printf("%s-%s-%s", __NAME__,
                           gdk_display_get_name(display), suffix);
    path = g_build_filename(g_get_user_runtime_dir(), name, NULL);
    g_free(name);
    unlink(path);
    sa = g_unix_socket_address_new(path);
    g_free(path);

    sock = g_threaded_socket_service_new(-1);
    if (!g_socket_listener_add_address(G_SOCKET_LISTENER(sock), sa, G_SOCKET_TYPE_STREAM,
                                       G_SOCKET_PROTOCOL_DEFAULT,
                                       NULL, NULL, &err))
    {
        fprintf(stderr, "Failed to set up socket: '%s'\n", err->message);
        exit(EXIT_FAILURE);
    }

    g_signal_connect(G_OBJECT(sock), "run",
                     G_CALLBACK(sock_incoming), NULL);

    g_socket_service_start(sock);
}

int
main(int argc, char **argv)
{
    gtk_init(&argc, &argv);
    setup_css();
    socket_listen("main");
    gtk_main();
}