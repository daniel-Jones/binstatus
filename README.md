By default this program will set your root windows name to the time in binary, however if you set the environment variable binstatus to 'dec' (export binstatus=dec) it will display the time as hh:mm.


compile:
````cc binstatus.c -lX11 -o binstatus````


run:
````./binstatus &````


You may prefer to put this in your .xinitrc file.
