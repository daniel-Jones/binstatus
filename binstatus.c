/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include "components.h"
#include "config.h"

int
main(void)
{
	char status[MAXLENGTH] = "";
	Display *dsp = XOpenDisplay(NULL);
	if(!dsp)
	{
		fprintf(stderr, "cannot open X display\n");
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		memset(status, 0, sizeof(status));
		for (size_t i = 0; i < sizeof(components)/sizeof(components[0]); i++)
		{
			char store[20];
			strncat(status, components[i].function(store, sizeof(store), components[i].flag), sizeof(store));
		}
		XStoreName(dsp, DefaultRootWindow(dsp), status);
		XFlush(dsp);
		sleep(1);
	}
	XCloseDisplay(dsp);
	return 0;
}
