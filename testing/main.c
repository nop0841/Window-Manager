#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
	Display * display; 
	Window  window; 
	int screen; 

	// display is the thing that links me to the x server 
	display = XOpenDisplay(NULL);
	// This is the screen number basically 
	screen = DefaultScreen(display); 

	// Creating the winodw 

	window = XCreateSimpleWindow(
		display,
		RootWindow(display,screen),
		0,0, // location from 0,0 in dwm i think 
		400,300, // size of the winodw 
		1, // border size
		WhitePixel(display,screen),
		BlackPixel(display,screen) 
	); 
	
	XMapWindow(display,window); 	

	// keeping the program in a loop until idk like a close button 


	XEvent event; 
	
	
	XSelectInput(display,window, ExposureMask | KeyPressMask); 
	
	//loop
	while(1)
	{
		XNextEvent(display,&event); 
		
		if(event.type == KeyPress)
		{
			KeySym keysym;
			char buffer[2]; 
			int length = XLookupString(
				&event.xkey,
				buffer,
				sizeof(buffer),
				&keysym,
				NULL
			);	
		
			//printf("keysym : %s\n",XKeysymToString(keysym));
			if(length > 0)
			{
				buffer[length] = '\0';
				printf("pressed : %s\n",buffer);	
				if(strcmp(buffer,"C") == 0)
				{
					break;
				}

				if(strcmp(XKeysymToString(keysym),"T") == 0 
				&& (event.xkey.state & (Mod1Mask | ShiftMask)) 
				== (ShiftMask | Mod1Mask))
				{
					if(fork() == 0)
					{
						execlp("st","st",NULL);		
						exit(1);
					}
				}
			}
		}	
	}		
	XCloseDisplay(display);	
	return 0;   
}
