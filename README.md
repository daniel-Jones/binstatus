This program sets your X servers root window name as the current time in 12-hour binary. This allows dwm (and possibly other) window managers use it in their status bar. I use it with dwm as a clock to practice recognising binary numbers quickly.

argumewnts:

````-d: display time in decimal format (HH:MM)````


````-m: Use the 24-hour time format````


compile:
````cc binstatus.c -lX11 -o binstatus````


run:
````./binstatus &````


You may prefer to put this in your .xinitrc file and move the binary to somewhere like /usr/bin.
