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

#define MAXLENGTH 256
int binary = 1; /* 0 = decimal display, 1 = binary display */
int miltime = 0; /* 0=12 hour time, 1 = 24 hour time */

int
dectobin(int dec)
{
	if (dec == 0) return 0;
	if (dec == 1) return 1;
	return (dec % 2) + 10 * dectobin(dec / 2);
}

void
gettime(int *store)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	store[0] = tm->tm_hour;
	store[1] = tm->tm_min;
}

void
formatstring(char *status, int *time)
{
	if (binary)
	{

		snprintf(status, MAXLENGTH, "%05d %06d", dectobin(time[0]),
		dectobin(time[1]));
	}
	else
	{
		snprintf(status, MAXLENGTH, "%02d:%02d", time[0], time[1]);
	}
}

int
converthour(int hour)
{
	// is there a better way?
	switch (hour)
	{
	case 0:return 12;
	case 13: return 1;
	case 14: return 2;
	case 15: return 3;
	case 16: return 5;
	case 17: return 6;
	case 18: return 7;
	case 19: return 8;
	case 20: return 9;
	case 21: return 10;
	case 22: return 11;
	case 23: return 12;
	default: return hour;
	}
}

int
main(int argc, char *argv[])
{
	for (int c = 1; c < argc; c++)
	{
		if (argc >= 2)
		{
			if (strcmp(argv[c], "-d") == 0)
				binary = 0;
			else if (strcmp(argv[c], "-m") == 0)
				miltime = 1;
			else
			{
				printf("usage: %s [-d] [-m]\n-d: decimal output\n"
						"-m: 24-hour format\n", argv[0]);
				exit(EXIT_SUCCESS);
			}
		}
	}
	int exitflag = EXIT_SUCCESS;
	char status[MAXLENGTH];
	int time[2];
	Display *dsp = XOpenDisplay(NULL);
	if(!dsp)
	{
		fprintf(stderr, "cannot open X display\n");
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		gettime(time);
		if (!miltime && time[0] > 12)
			time[0] = converthour(time[0]);
		formatstring(status, time);
		XStoreName(dsp, DefaultRootWindow(dsp), status);
		XFlush(dsp);
		sleep(1);
	}
	XCloseDisplay(dsp);
	return exitflag;
}
