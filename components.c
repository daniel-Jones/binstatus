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

#include "components.h"
#include "config.h"

int
dectobin(int dec)
{
	if (dec == 0) return 0;
	if (dec == 1) return 1;
	return (dec % 2) + 10 * dectobin(dec / 2);
}

char *eat(char *food, size_t len)
{
	if (len == 0) return food;
	while (food[len-1] == '\n')
	{
		food[len-1] = '\0';
	}
	return food;
}

void
gettime(int *store)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	store[0] = tm->tm_hour;
	store[1] = tm->tm_min;
}

int
converthour(int hour)
{
	return (hour - 12 < 0) ? 0 : hour - 12;
}

char *currenttime(char *store, size_t size, int flag)
{
	int time[2];
	char meridiem[3];
	gettime(time);
	if (flag & SHOWMERIDIEM)
	{
		meridiem[1] = 'M';
		meridiem[2] = '\0';
		if (time[0] < 12)
			meridiem[0] = 'A';
		else
			meridiem[0] = 'P';

	}
	if (flag & NORMALTIME)
	{
		if (time[0] == 0)
			time[0] = 12;
		else if (time[0] > 12)
			time[0] = converthour(time[0]);
	}
	if (flag & BINARYTIME)
	{
		snprintf(store, size, "%05d:%05d %s ", dectobin(time[0]),
						    dectobin(time[1]),
						    meridiem);
	}
	/* military time is the default, so we need not do anything */
	else
	{
		snprintf(store, size, "%02d:%02d %s ", time[0], time[1],
						    meridiem);
	}
	return store;
}

char *battery(char *store, size_t size, int flag)
{
	FILE *capacity;
	char cap[4];
	capacity = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	if (!capacity)
	{
		fprintf(stderr, "cannot read battery percent\n");
		return "error ";
	}
	fread(cap, 1, 4, capacity);
	fclose(capacity);
	snprintf(store, size, "%d%% ", atoi(cap));
	return store;
}

char *charging(char *store, size_t size, int flag)
{
	FILE *state;
	char cap[15];
	state = fopen("/sys/class/power_supply/BAT0/status", "r");
	if (!state)
	{
		fprintf(stderr, "cannot read battery state\n");
		return "error ";
	}
	fgets(cap, 15, state);
	eat(cap, strlen(cap));
	/* my thinkpad says "Unknown" when charged ... */
	if (strcmp(cap, "Unknown") == 0) strcpy(cap, "⚡F");
	if (strcmp(cap, "Charged") == 0) strcpy(cap, "⚡F");
	if (strcmp(cap, "Charging") == 0) strcpy(cap, "⚡C");
	if (strcmp(cap, "Discharging") == 0) strcpy(cap, "⚡D");
	fclose(state);
	snprintf(store, size, "%s ", cap);
	return store;
}

char *remainingtime(char *store, size_t size, int flag)
{
	return store;
}
