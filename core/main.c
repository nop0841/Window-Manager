#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//grab all the keys for the key bind 
void GrabKey(Display * X,Window root)
{
	//terminal 
	// alt + crlt + enter
	XGrabKey(
		X,
		XKeysymToKeycode(X,XK_Return),
		ShiftMask | Mod1Mask, 
		root,
		False,
		GrabModeAsync,GrabModeAsync
	);

	//close the WM 
	// q + alt + ctrl
	XGrabKey(
		X,
		XKeysymToKeycode(X,XK_q),
		ShiftMask | Mod1Mask, 
		root,
		False,
		GrabModeAsync,GrabModeAsync
	);

	//close window	
	XGrabKey(
		X,
		XKeysymToKeycode(X,XK_c),
		ShiftMask | Mod1Mask, 
		root,
		False,
		GrabModeAsync,GrabModeAsync
	);
	
	XGrabKey(
		X,
		XKeysymToKeycode(X,XK_p),
		Mod1Mask, 
		root,
		False,
		GrabModeAsync,GrabModeAsync
	);
	

	// Incase i need it later.....
	/*
	XGrabPointer(
		X,
		root,
		NULL,
	*/	

}


int main() 
{
	Display * X; 
	Window root;
	int screen; 
	
	
	//Variables dummys for the XQueryPointer Function. 
	//---------------------------------------------
	Window root_return;
	Window child_return;  
	int root_x_return; 
	int root_y_return; 
	int win_x_return;
	int win_y_return; 
	int mask_return; 
	//---------------------------------------------
	
	X = XOpenDisplay(NULL); 
	if(X == NULL)
	{
		printf("Failed to open the X server\n");
		return 0;
	}

	printf("Connect to the X server");
	screen = DefaultScreen(X);
	root = RootWindow(X,screen); 
	GrabKey(X,root);	

	XSelectInput(X,root,
		SubstructureNotifyMask | 
		SubstructureRedirectMask | 
		KeyPressMask  |
		PointerMotionMask
	); 	

	// Event Loop
	XEvent event;
	while(1)
	{
		XNextEvent(X,&event);

		XQueryPointer(
			X,
			root,
			&root_return,&child_return, // window ID that we want
			&root_x_return,&root_y_return, 
			&win_x_return,&win_y_return, 
			&mask_return
		);

		if(event.type == KeyPress)
		{
			
			KeySym keysym = XLookupKeysym(&event.xkey,0);
			
			if( (event.xkey.state & (Mod1Mask)) 
			== (Mod1Mask) && keysym == XK_Return)
			{
				//open st
				if(fork() == 0)
				{
					execlp("st","st",NULL);
					exit(0); 
				}
				
			}
	
			if( (event.xkey.state & (ShiftMask | Mod1Mask)) 
			== (ShiftMask | Mod1Mask) && keysym == XK_q)
			{
				break;
			}
			
			if( (event.xkey.state & (Mod1Mask)) 
			== (Mod1Mask) && keysym == XK_p)
			{
				if(fork() == 0)
				{
					execlp("dmenu_run","dmenu_run",NULL);
					exit(0); 
				}
			}
			
			//close window	
			if( (event.xkey.state & (ShiftMask | Mod1Mask)) 
			== (ShiftMask | Mod1Mask) && keysym == XK_c)
			{
				if(child_return != root)
				{
					XKillClient(X,child_return );
				}	
			}

			
			//dmenu 

		}

		if(event.type == MapRequest) 
		{
			XMapWindow(X,event.xmaprequest.window); 
		}
	
		// Mouse movements
		if(event.type == MotionNotify) 
		{
			XQueryPointer(
				X,
				root,
				&root_return,&child_return, // window ID that we want
				&root_x_return,&root_y_return, 
				&win_x_return,&win_y_return, 
				&mask_return
			);
		}
	}
	XCloseDisplay(X); 
	return 0; 	
		
}


/// STATUS ----------------------------------------------- 
/* 
	Phase 1 
	
	* Find a way to close windows (Completed)
	* Move windows and resize them with the pointer 
	* Make a repo for this project on GitHub 
	
	Phase 2 

	* Code a status bar just for the time and cpu usage. 
	* Fix the firefox issue 

	Phase 3 (Probabily at the time where the club exists)
	
	* Post about it on reddit or stack over to find contributers
	* Transfer this project on to the new team (The club 555) 
	  and make this project a team one. 

	Final stage i think.... (IF am like super lucky and shit)
	
	* The whole club helps me with this project. Wait i mean our project.
	* GitHub fully working. 
	* Outside people giving feedback and if luck is on our side then help 
          from outside also.  
*/
