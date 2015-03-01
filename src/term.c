/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2014 Vincent Bernat <bernat@luffy.cx>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "term.h"

#include <stdio.h>
#include <unistd.h>
#include <vte/vte.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms-compat.h>

GtkWidget *window, *terminal;

static void
set_font_size(gint delta)
{
	PangoFontDescription *descr;
	descr = pango_font_description_copy(vte_terminal_get_font(VTE_TERMINAL(terminal)));
	if (!descr) return;

	gint current = pango_font_description_get_size(descr);
	pango_font_description_set_size(descr, current + delta * PANGO_SCALE);
	vte_terminal_set_font(VTE_TERMINAL(terminal), descr);
	pango_font_description_free(descr);
}

static gboolean
on_dpi_changed(GtkSettings *settings,
    GParamSpec *pspec,
    gpointer user_data)
{
	set_font_size(0);
	return TRUE;
}

static gboolean
on_char_size_changed(GtkWidget *terminal, guint width, guint height, gpointer user_data)
{
	set_font_size(0);
	return TRUE;
}

static gboolean
on_title_changed(GtkWidget *terminal, gpointer user_data)
{
	gtk_window_set_title(GTK_WINDOW(window),
	    vte_terminal_get_window_title(VTE_TERMINAL(terminal))?:PACKAGE_NAME);
	return TRUE;
}

static gboolean
on_key_press(GtkWidget *terminal, GdkEventKey *event)
{
	if (event->state & GDK_CONTROL_MASK) {
		switch (event->keyval) {
		case GDK_plus:
			set_font_size(1);
			return TRUE;
		case GDK_minus:
			set_font_size(-1);
			return TRUE;
		case GDK_equal:
			set_font_size(0);
			return TRUE;
		}
	}
	return FALSE;
}

static gchar**
get_child_environment(void)
{
	guint n;
	gchar **env, **result, **p;
	const gchar *value;

	/* Copy the current environment */
	env = g_listenv();
	n = g_strv_length (env);
	result = g_new (gchar *, n + 2);
	for (n = 0, p = env; *p != NULL; ++p) {
		if (g_strcmp0(*p, "COLORTERM") == 0) continue;
		value = g_getenv (*p);
		if (G_LIKELY(value != NULL))
			result[n++] = g_strconcat (*p, "=", value, NULL);
	}
	g_strfreev(env);

	/* Setup COLORTERM */
	result[n++] = g_strdup_printf("COLORTERM=%s", PACKAGE_NAME);
	result[n] = NULL;
	return result;
}


int
main(int argc, char *argv[])
{
    GdkColor fg;
    GdkColor bg;
    GdkColor palette[16];

	/* Initialise GTK and the widgets */
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	terminal = vte_terminal_new();
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	gtk_container_add(GTK_CONTAINER(window), terminal);
	gtk_widget_set_visual(window, gdk_screen_get_rgba_visual(gtk_widget_get_screen(window)));
	gtk_widget_show_all(window);
	gtk_window_set_focus(GTK_WINDOW(window), terminal);

	/* Connect some signals */
	g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
	g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);
	g_signal_connect(terminal, "window-title-changed", G_CALLBACK(on_title_changed), NULL);
	g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL);
	g_signal_connect(terminal, "char-size-changed", G_CALLBACK(on_char_size_changed), NULL);
	g_signal_connect(gtk_settings_get_default(), "notify::gtk-xft-dpi",
	    G_CALLBACK(on_dpi_changed), NULL);

	/* Configure terminal */
	vte_terminal_set_word_chars(VTE_TERMINAL(terminal),
	    TERM_WORD_CHARS);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal),
	    0);
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal),
	    FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal),
	    TRUE);
	vte_terminal_set_rewrap_on_resize(VTE_TERMINAL(terminal),
	    TRUE);

#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_16(x)  (((x) << 8) | (x))
#define CLR_GDK(x) (const GdkColor){ 0, CLR_16(CLR_R(x)), CLR_16(CLR_G(x)), CLR_16(CLR_B(x)) }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    gdk_color_parse("#838394949696", &fg);
    gdk_color_parse("#00002b2b3636", &bg);
    gdk_color_parse("#070736364242", &palette[0]);
    gdk_color_parse("#dcdc32322f2f", &palette[1]);
    gdk_color_parse("#858599990000", &palette[2]);
    gdk_color_parse("#b5b589890000", &palette[3]);
    gdk_color_parse("#26268b8bd2d2", &palette[4]);
    gdk_color_parse("#d3d336368282", &palette[5]);
    gdk_color_parse("#2a2aa1a19898", &palette[6]);
    gdk_color_parse("#eeeee8e8d5d5", &palette[7]);
    gdk_color_parse("#00002b2b3636", &palette[8]);
    gdk_color_parse("#cbcb4b4b1616", &palette[9]);
    gdk_color_parse("#58586e6e7575", &palette[10]);
    gdk_color_parse("#65657b7b8383", &palette[11]);
    gdk_color_parse("#838394949696", &palette[12]);
    gdk_color_parse("#6c6c7171c4c4", &palette[13]);
    gdk_color_parse("#9393a1a1a1a1", &palette[14]);
    gdk_color_parse("#fdfdf6f6e3e3", &palette[15]);
#pragma GCC diagnostic pop
	vte_terminal_set_colors(VTE_TERMINAL(terminal),
        &fg,
        &bg,
        palette,
        16);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	vte_terminal_set_opacity(VTE_TERMINAL(terminal),
	    TERM_OPACITY * 65535);
#pragma GCC diagnostic pop
	vte_terminal_set_color_cursor(VTE_TERMINAL(terminal),
	    &CLR_GDK(0x008800));
	vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal),
	    VTE_CURSOR_BLINK_OFF);
	vte_terminal_set_allow_bold(VTE_TERMINAL(terminal),
	    FALSE);
	vte_terminal_set_font_from_string(VTE_TERMINAL(terminal),
	    TERM_FONT);
	set_font_size(0);

	vte_terminal_set_audible_bell(VTE_TERMINAL(terminal),
	    FALSE);
	vte_terminal_set_visible_bell(VTE_TERMINAL(terminal),
	    FALSE);

	/* Start a new shell */
	gchar **env;
	env = get_child_environment();
	vte_terminal_fork_command_full(VTE_TERMINAL (terminal),
	    VTE_PTY_DEFAULT,
	    NULL,		/* working directory */
	    (gchar *[]){ g_strdup(g_getenv("SHELL")), 0 },
	    env,		/* envv */
	    0,			/* spawn flags */
	    NULL, NULL,		/* child setup */
	    NULL,		/* child pid */
	    NULL);
	g_strfreev(env);

	/* Pack widgets and start the terminal */
	gtk_main();
	return FALSE;
}
