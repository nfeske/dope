/*
 * \brief  DOpE-embedded demo application
 * \author Matthias Alles
 * \author Norman Feske
 * \date   2008-08-17
 */

/*
 * Copyright (C) 2009-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include <base/printf.h>
#include <dope/dopelib.h>
#include <dope/vscreen.h>
#include <timer_session/connection.h>

/* local */
#include "util.h"
#include "disp_img.h"
#include "dopecmd.h"
#include "settings.h"
#include "grid.h"
#include "colors.h"
#include "genode-labs-banner.h"

using Genode::printf;


static int app_id_genode_img, app_id_colors;

enum {
	MENU_ITEM_GENODE_LABS_LOGO,
	MENU_ITEM_SETTINGS,
	MENU_ITEM_GRID_DEMO,
	MENU_ITEM_TERMINAL,
	MENU_ITEM_COLORS,
};

static int menu_app_id;

static void menu_callback(Event_union *e, void *arg)
{
	switch ((int)arg) {
	case MENU_ITEM_GENODE_LABS_LOGO:
		open_image_window(app_id_genode_img);
		break;
	case MENU_ITEM_SETTINGS:
		open_settings_window();
		break;
	case MENU_ITEM_GRID_DEMO:
		open_grid_window();
		break;
	case MENU_ITEM_TERMINAL:
		open_dopecmd_window();
		break;
	case MENU_ITEM_COLORS:
		open_colors_window(app_id_colors);
		break;
	}
}

static void init_menu_window(void)
{
	menu_app_id = dope_init_app("Menu");

	dope_cmd_seq(menu_app_id,
		"g = new Grid()",
		"b_genode_labs_logo = new Button(-text \"Genode Labs logo\")",
		"b_settings         = new Button(-text \"Settings\")",
		"b_grid_demo        = new Button(-text \"Grid demo\")",
		"b_terminal         = new Button(-text \"Terminal\")",
		"b_colors           = new Button(-text \"Colors\")",
		"g.place(b_genode_labs_logo, -column 1 -row 1)",
		"g.place(b_settings,         -column 1 -row 2)",
		"g.place(b_grid_demo,        -column 1 -row 3)",
		"g.place(b_terminal,         -column 1 -row 4)",
		"g.place(b_colors,           -column 1 -row 5)",
		"w = new Window(-x 24 -y 32 -content g)",
		"w.open()",
		0
	);

	dope_bind(menu_app_id, "b_genode_labs_logo", "commit", menu_callback, (void *)MENU_ITEM_GENODE_LABS_LOGO);
	dope_bind(menu_app_id, "b_settings",         "commit", menu_callback, (void *)MENU_ITEM_SETTINGS);
	dope_bind(menu_app_id, "b_grid_demo",        "commit", menu_callback, (void *)MENU_ITEM_GRID_DEMO);
	dope_bind(menu_app_id, "b_terminal",         "commit", menu_callback, (void *)MENU_ITEM_TERMINAL);
	dope_bind(menu_app_id, "b_colors",           "commit", menu_callback, (void *)MENU_ITEM_COLORS);
}


int main(int argc, char **argv)
{
	/* initialize the GUI */
	printf("Initializing GUI...\n");

	dope_init();

	/* setup windows */
	printf("Setup windows...\n");

	app_id_genode_img = create_image_window("Genode Labs", pixel_data_genode,
	                                        420,  50, 232,  84);

	init_grid_window();
	init_dopecmd();
	init_settings();
	app_id_colors = init_colors_window();
	init_menu_window();

	/* default window configuration */
	open_image_window(app_id_genode_img);

	/* run the GUI */
	printf("Running GUI...\n");

	Timer::Connection timer;
	for (;;) {
		dope_process_event(0);
		timer.msleep(10);
	}

	return 0;
}
