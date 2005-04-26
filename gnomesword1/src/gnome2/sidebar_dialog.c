/*
 * GnomeSword Bible Study Tool
 * sidebar_dialog.c - dialog for detached sidebar
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gui/sidebar.h"
#include "gui/sidebar_dialog.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/settings.h"

static GtkWidget* create_sidebar_dialog(void);
static GtkWidget * vbox_dock; 

/******************************************************************************
 * Name
 *   gui_attach_detach_sidebar
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *   
 *   void gui_attach_detach_sidebar(void)
 *
 * Description
 *   dock/undock shortcut bar
 *   
 *
 * Return value
 *   
 */

void gui_attach_detach_sidebar(void)
{
	gint biblepanesize;

	if (settings.docked) {
		settings.docked = FALSE;
		biblepanesize = settings.gs_width / 2;
		widgets.dock_sb = create_sidebar_dialog();
		gtk_widget_reparent(widgets.shortcutbar, vbox_dock);
		settings.showshortcutbar = TRUE;
		gtk_widget_show(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned), 0);
		gtk_paned_set_position(GTK_PANED
				     (widgets.hpaned),
				     biblepanesize);
		gtk_widget_show(widgets.dock_sb);
	} else {
		settings.docked = TRUE;
		biblepanesize =
		    (settings.gs_width - settings.sidebar_width) / 2;
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				     settings.sidebar_width);
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				     biblepanesize);
		gtk_widget_reparent(widgets.shortcutbar,
				    widgets.epaned);
		gtk_widget_destroy(widgets.dock_sb);
	}
}

/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *   
 *   void on_dialog_destroy(GtkObject *object, gpointer user_data)
 *
 * Description
 *   send the sidebar back to the main window before we 
 *   destroy the dialog
 *   
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GtkObject *object, gpointer user_data)
{
	/* we need the if to prevent a loop */
	if(!settings.docked)
		gui_attach_detach_sidebar();
}

/******************************************************************************
 * Name
 *   create_sidebar_dialog
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *   
 *   GtkWidget*  create_sidebar_dialog(void)
 *
 * Description
 *   create dialog to hold the sidebar when detached
 *   
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget* create_sidebar_dialog(void)
{
	GtkWidget *dlgDock; 

	dlgDock = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(dlgDock), "dlgDock", dlgDock);
	gtk_window_set_title(GTK_WINDOW(dlgDock), _("Sidebar"));
	gtk_window_set_resizable(GTK_WINDOW(dlgDock), TRUE);
	gtk_widget_set_usize(dlgDock, settings.sidebar_width,
			settings.gs_hight);

	vbox_dock = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_dock);
	gtk_container_add(GTK_CONTAINER(dlgDock), vbox_dock);

	g_signal_connect(G_OBJECT(dlgDock), "destroy",
			G_CALLBACK(on_dialog_destroy), NULL);

	return dlgDock;
}
