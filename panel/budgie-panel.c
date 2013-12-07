/*
 * budgie-panel.c
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <libwnck/libwnck.h>

#include "budgie-panel.h"

G_DEFINE_TYPE(BudgiePanel, budgie_panel, GTK_TYPE_WINDOW)

#define PANEL_HEIGHT 30

/* Boilerplate GObject code */
static void budgie_panel_class_init(BudgiePanelClass *klass);
static void budgie_panel_init(BudgiePanel *self);
static void budgie_panel_dispose(GObject *object);

/* Private methods */
static gboolean update_clock(gpointer userdata);

/* Initialisation */
static void budgie_panel_class_init(BudgiePanelClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &budgie_panel_dispose;
}

static void budgie_panel_init(BudgiePanel *self)
{
        GtkWidget *tasklist;
        GtkWidget *layout;
        GdkScreen *screen;
        GtkWidget *clock;
        int x, y, width, height;
        GtkSettings *settings;

        /* Sort ourselves out visually */
        settings = gtk_widget_get_settings(GTK_WIDGET(self));
        g_object_set(settings, "gtk-application-prefer-dark-theme",
                TRUE, NULL);

        /* tiny bit of padding */
        gtk_container_set_border_width(GTK_CONTAINER(self), 2);

        /* Our main layout is a horizontal box */
        layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_container_add(GTK_CONTAINER(self), layout);

        /* Add a tasklist to the panel */
        tasklist = wnck_tasklist_new();
        self->tasklist = tasklist;
        wnck_tasklist_set_button_relief(WNCK_TASKLIST(tasklist),
                GTK_RELIEF_NONE);
        gtk_box_pack_start(GTK_BOX(layout), tasklist, FALSE, FALSE, 0);

        /* Add a clock at the end */
        clock = gtk_label_new("--");
        self->clock = clock;
        gtk_box_pack_end(GTK_BOX(layout), clock, FALSE, FALSE, 0);

        /* Ensure we close when destroyed */
        g_signal_connect(self, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        /* Set ourselves up to be the correct size and position */
        screen = gdk_screen_get_default();
        width = gdk_screen_get_width(screen);
        height = gdk_screen_get_height(screen);

        x = 0;
        y = height - PANEL_HEIGHT;

        gtk_widget_set_size_request(GTK_WIDGET(self), width, 30);
        gtk_window_move(GTK_WINDOW(self), x, y);

        /* We want to be a dock */
        gtk_window_set_type_hint(GTK_WINDOW(self),
                GDK_WINDOW_TYPE_HINT_DOCK);
        /* And now show ourselves */
        gtk_widget_show_all(GTK_WIDGET(self));

        /* Don't show an empty label */
        update_clock((gpointer)self);
        /* Update the clock every second */
        g_timeout_add(1000, update_clock, (gpointer)self);
}

static void budgie_panel_dispose(GObject *object)
{
        /* Destruct */
        G_OBJECT_CLASS (budgie_panel_parent_class)->dispose (object);
}

/* Utility; return a new BudgiePanel */
BudgiePanel* budgie_panel_new(void)
{
        BudgiePanel *self;

        self = g_object_new(BUDGIE_PANEL_TYPE, NULL);
        return self;
}

static gboolean update_clock(gpointer userdata)
{
        BudgiePanel *self;;
        gchar *date_string;
        GDateTime *dtime;

        self = BUDGIE_PANEL(userdata);

        /* Get the current time */
        dtime = g_date_time_new_now_local();

        /* Format it as a string (24h) */
        date_string = g_date_time_format(dtime, "%H:%M:%S");
        gtk_label_set_markup(GTK_LABEL(self->clock), date_string);
        g_free(date_string);
        g_date_time_unref(dtime);

        return TRUE;
}
