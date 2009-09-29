/*
 * Xiphos Bible Study Tool
 * main.c - In the beginning... ;o)
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <string.h>

#include "gui/dialog.h"
#include "gui/gui.h"
#include "gui/main_window.h"
#include "gui/splash.h"
#include "gui/tabbed_browser.h"
#include "gui/xiphos.h"
//#include "gui/widgets.h"

#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif
#endif
//#include "backend/sword.h"

#include "gui/debug_glib_null.h"

#include <glib/gstdio.h>
 
/******************************************************************************
 * Name
 *   main
 *
 * Synopsis
 *   
 *   int main(int argc, char *argv[])
 *
 * Description
 *   Starting point of Xiphos.
 *
 * Return value
 *   int
 */

int main(int argc, char *argv[])
{
	int newconfigs = FALSE;
	int newbookmarks = FALSE;
	int have_sword_url = FALSE;
	gint base_step = 0; //needed for splash
	
	g_thread_init(NULL);
	g_type_init();

#ifdef WIN32
	/*
	 * WIN32 perversity: We very much want to be *in* the
	 * the directory where Xiphos exists, to keep gconf and
	 * gspawn happy.  Glib provides this nice function for us
	 * to determine where we are (we can't depend on argv).
	 */
	gchar *install_dir = g_win32_get_package_installation_directory_of_module(NULL);
	install_dir = g_strconcat(install_dir, "\0", NULL);
	install_dir = g_build_filename (install_dir, "bin\0");
	g_chdir (install_dir);
	
	/* add this directory to $PATH for other stuff, e.g. zip */
	g_setenv("PATH", g_strdup_printf("%s;%s", install_dir,
					        g_getenv("PATH")),
		                                TRUE);

	/* This will not overwrite SWORD_PATH in case it has
	   been set by some other program or set manually. In the case that
	   it hasn't been set, it is set to the localized equivalent of 
	   C:/Documents and Settings/All Users/Application Data/Sword */
	const gchar * const * strings;
	strings = g_get_system_data_dirs();
	g_setenv("SWORD_PATH", g_strdup_printf("%s\\Sword",
					       //g_getenv("ALLUSERSPROFILE"),
					       strings[0]),
		                               FALSE);

	/*it is necessary to explicitly set LANG, because we depend on that
	  variable to set the SWORD locale */
	g_setenv("LANG", g_win32_getlocale(), FALSE);

	/* we need an idea of $HOME that's convenient. */
	/* this gives us linux-equivalent semantics. */
	g_setenv("HOME", g_getenv("APPDATA"), TRUE);
#endif /* WIN32 */

	if (argc > 2) {
		gui_generic_warning(_("Xiphos does not understand more than one argument."));
		exit(1);
	}

	if (argc > 1) {
		/*
		 * these args are for broken configs or bookmarks -
		 * ie xiphos will not start
		 */
		if (!strcmp(argv[1], "newconfigs")) {
			newconfigs = TRUE;
		}
		if (!strcmp(argv[1], "newbookmarks")) {
			newbookmarks = TRUE;
		}
		if (!strcmp(argv[1], "newfiles")) {
			newconfigs = TRUE;
			newbookmarks = TRUE;
		}
		/*
		 * this arg is a sword uri
		 */
		if ((!strncmp(argv[1], "sword:/", 7)) ||
		    (!strncmp(argv[1], "bible:/", 7)) ||
		    (strstr(argv[1], "studypad"))) {
			if (!strncmp(argv[1], "bible:/", 7))
				memcpy(argv[1], "sword", 5); /* equivalent */
			have_sword_url = TRUE;
		}
	}

	/* 
	 * check for directories and files
	 */   
    	settings_init(newconfigs,newbookmarks);

	gui_init(argc, argv);
	
	gui_splash_init();
	
#ifdef USE_GTKMOZEMBED
	
	gui_splash_step(_("Initiating Gecko"), 0.0, 0);
	
	gecko_html_initialize();

	base_step = 1;
#endif
	gui_splash_step(_("Building Interface"), 0.2, 0 + base_step);

	create_mainwindow();

	gui_splash_step(_("Starting Sword"), 0.5, 1 + base_step);

	main_init_backend();
	
	gui_splash_step(_("Loading Settings"), 0.8, 2 + base_step);

	frontend_init();
	
	gui_splash_step(_("Displaying Xiphos"), 1.0, 3 + base_step);
	
	/* need to get rid of wrongly-formatted annotation labels. */
	xml_convert_to_osisref();
	
	frontend_display();
	
	gui_splash_done();

	if (have_sword_url) {
		if (!strncmp(argv[1], "sword:/", 7)) {
			char *key = strchr(argv[1]+8, '/');
			if (key && *(key+1) != '\0')
				key++;
			else
				key = "Gen 1:1";
			gui_open_passage_in_new_tab(key);
		}
		main_url_handler(argv[1], TRUE);
	}

	/*gtk_notebook_set_current_page(GTK_NOTEBOOK
					(widgets.notebook_comm_book), 0);*/
	gui_main();
	return 0;
}
