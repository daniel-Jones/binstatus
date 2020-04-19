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

#include <unistd.h>
#include <signal.h>
#include "components.h"
#include "config.h"

int
run(const char *program)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		// child process, we don't want to ignore signals
		signal(SIGCHLD, SIG_DFL);
		/*
		 * we don't want std{out,err} to be associated with the terminal,
		 * but we also don't want to close it to avoid the file descriptors
		 * being re-used potentially leading to problems, so reopen them to /dev/null
		 */
		freopen("/dev/null", "w", stdout);
		freopen("/dev/null", "w", stderr);
		char *args[ARG_LIMIT];
		char *buff = malloc(BUFF_SIZE);
		if (buff == NULL)
		{
			perror("malloc");
			return -1;
		}
		/*
		 * the program we're calling might have arguments,
		 * so we tokenise the string and add each part to an array
		 * that we will use in execvp
		 */
		strncpy(buff, program, BUFF_SIZE-1);
		char *t = strtok(buff, " ");
		int z = 0;
		while (t != NULL && z < ARG_LIMIT-1) // save a position for NULL
		{
			args[z] = t;
			t = strtok(NULL, " ");
			z++;
		}
		args[z] = (char *)0;
		execvp(args[0], args);
		_exit(1);
	}
	else if (pid == -1)
	{
		perror("fork");
		return -1;
	}
	return 1;
}

int
dectobin(int dec)
{
	if (dec == 0) return 0;
	if (dec == 1) return 1;
	return (dec % 2) + 10 * dectobin(dec / 2);
}

char *eatnonascii(char *food, size_t len)
{
	if (len <= 0) return food;
	while (food[len-1] < 32 || food[len-1] > 126)
	{
		food[--len] = '\0';
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
		snprintf(store, size, "%05d:%05d%s ", dectobin(time[0]),
						    dectobin(time[1]),
						    meridiem);
	}
	/* military time is the default, so we need not do anything */
	else
	{
		snprintf(store, size, "%02d:%02d%s ", time[0], time[1],
						    meridiem);
	}
	return store;
}

int has_low_batt = 0;

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
	int percent = atoi(cap);

	if ( !has_low_batt && percent < LOWBATT)
	{
		run(LOWBATTERY);
		has_low_batt = 1;
	}
	else if (has_low_batt && percent > LOWBATT)
	{
		has_low_batt = 0;
	}

	snprintf(store, size, "%d%% ", percent);
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
	eatnonascii(cap, strlen(cap));
	/* my thinkpad says "Unknown" when charged ... */
	if (strcmp(cap, "Unknown") == 0) strcpy(cap, "F");
	if (strcmp(cap, "Charged") == 0) strcpy(cap, "F");
	if (strcmp(cap, "Charging") == 0) strcpy(cap, "C");
	if (strcmp(cap, "Discharging") == 0) strcpy(cap, "D");
	fclose(state);
	snprintf(store, size, "%s ", cap);
	return store;
}

char *volume(char *store, size_t size, int flag)
{
	// TODO
	return store;
}

char *cputemp(char *store, size_t size, int flag)
{
	FILE *cpufile;
	char temp[15];
	int t;
	cpufile = fopen(TEMPFILE, "r");
	if (!cpufile)
	{
		fprintf(stderr, "cannot read CPU temp\n");
		return "error ";
	}
	fgets(temp, 4, cpufile);
	eatnonascii(temp, strlen(temp));
	if (temp[2] == '0')
		temp[2] = '\0';
	fclose(cpufile);

	t = atoi(temp);

	if (flag & FARENHEIT)
	{
		t = (t*9)/5+32;	
		snprintf(store, size, "%d°F ", t);
	}
	else
		snprintf(store, size, "%d°C ", t);
	return store;
}
