/* wmclipchar - Window Maker dockapp for copying characters to the clipboard
 * Copyright (C) 2017 Doug Torrance <dtorrance@piedmont.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <libdockapp/dockapp.h>

#define BUTTON_SIZE 18
#define NUM_ROWS 3
#define NUM_COLS 3
#define DOCKAPP_WIDTH NUM_COLS * BUTTON_SIZE
#define DOCKAPP_HEIGHT NUM_ROWS * BUTTON_SIZE
#define TIMEOUT 100

Pixmap pixmap;
GC button_gc;
GC highlight_gc;
GC shadow_gc;

char chars[3][3] = {{'a', 'b', 'c'}, {'d', 'e', 'f'}, {'g', 'h', 'i'}};

void draw_northwest_button(int i, int j, GC gc)
{
	XDrawLine(DADisplay, pixmap, gc,
		  i * BUTTON_SIZE, j * BUTTON_SIZE,
		  i * BUTTON_SIZE, (j + 1) * BUTTON_SIZE - 1);
	XDrawLine(DADisplay, pixmap, gc,
		  i * BUTTON_SIZE, j * BUTTON_SIZE,
		  (i + 1) * BUTTON_SIZE - 1, j * BUTTON_SIZE);
}

void draw_southeast_button(int i, int j, GC gc)
{
	XDrawLine(DADisplay, pixmap, gc,
		  i * BUTTON_SIZE, (j + 1) * BUTTON_SIZE - 1,
		  (i + 1) * BUTTON_SIZE - 1, (j + 1) * BUTTON_SIZE - 1);
	XDrawLine(DADisplay, pixmap, gc,
		  (i + 1) * BUTTON_SIZE - 1, j * BUTTON_SIZE,
		  (i + 1) * BUTTON_SIZE - 1, (j + 1) * BUTTON_SIZE - 1);
}

void draw_unpressed_button(int i, int j)
{
	draw_northwest_button(i, j, highlight_gc);
	draw_southeast_button(i, j, shadow_gc);
}

void draw_pressed_button(int i, int j)
{
	draw_northwest_button(i, j, shadow_gc);
	draw_southeast_button(i, j, highlight_gc);
}

void draw_char(int i, int j)
{
	char s[2];

	sprintf(s, "%c", chars[i][j]);
	/* TODO: center this better */
	XDrawString(DADisplay, pixmap, DAGC,
		    (i + 0.5) * BUTTON_SIZE, (j + 0.5) * BUTTON_SIZE, s, 1);
}

void button_press(int button, int state, int x, int y)
{
	Atom selection;
	int i, j;
	char s[2];

	i = x / BUTTON_SIZE;
	j = y / BUTTON_SIZE;

	draw_pressed_button(i, j);
	DASetPixmap(pixmap);
}

void button_release(int button, int state, int x, int y)
{
	draw_unpressed_button(x / BUTTON_SIZE, y / BUTTON_SIZE);
	DASetPixmap(pixmap);
}

int main(int argc, char **argv)
{
	int i, j;
	XGCValues values;

	DAProgramOption options[] = {};

	DACallbacks callbacks = {
		NULL,
		button_press,
		button_release,
		NULL,
		NULL,
		NULL,
		NULL};

	DAParseArguments(
		argc, argv, options, 0,
		"Window Maker dockapp for copying characters to the clipboard",
		PACKAGE_STRING);
	DAInitialize(NULL, PACKAGE_NAME, DOCKAPP_WIDTH, DOCKAPP_HEIGHT,
		     argc, argv);
	DASetCallbacks(&callbacks);

	pixmap = DAMakePixmap();

	values.foreground = DAGetColor("gray");
	button_gc = XCreateGC(DADisplay, pixmap, GCForeground, &values);

	values.foreground = DAGetColor("white");
	highlight_gc = XCreateGC(DADisplay, pixmap, GCForeground, &values);

	values.foreground = DAGetColor("black");
	shadow_gc = XCreateGC(DADisplay, pixmap, GCForeground, &values);

	XFillRectangle(DADisplay, pixmap, button_gc,
		       0, 0, DOCKAPP_WIDTH, DOCKAPP_HEIGHT);
	for (i = 0; i < NUM_ROWS; i++)
		for (j = 0; j < NUM_COLS; j++) {
			draw_unpressed_button(i, j);
			draw_char(i, j);
		}

	DASetPixmap(pixmap);

	DASetTimeout(TIMEOUT);
	DAShow();
	DAEventLoop();

	return 0;
}
