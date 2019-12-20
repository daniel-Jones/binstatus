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

#ifndef CONFIG_H
#define CONFIG_H

#define SLEEPTIME 5 // number of seconds to sleep before each update
#define TEMPFILE "/sys/class/thermal/thermal_zone0/temp"
#define MAXLENGTH 256

enum flag
{
	NONE		= 0,
	NORMALTIME 	= 1<<1,
	BINARYTIME 	= 1<<2,
	MILITARYTIME	= 1<<3,
	SHOWMERIDIEM	= 1<<4,
	FARENHEIT	= 1<<5,
};

struct component
{
	char *(*function)();
	int flag;
};

/*
 * add or remove components as desired
 * components are shown in the order of this array
 */
static const struct component components[] ={
/*	function	flag	*/
	{currenttime,	NORMALTIME|SHOWMERIDIEM},
	{battery,	NONE},
	{charging,	NONE},
	{cputemp,	NONE},
};

#endif
