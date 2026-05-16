#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


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

	/*
		
	XGrabPointer(
		X, 
		root, 
		False, 
		ButtonMotionMask, 
		GrabModeAsync,GrabModeAsync,
		None, 
		None, 
		CurrentTime
	);
	
	*/ 
	
	XGrabButton(
		X, 
		Button1, 
		Mod1Mask, 
		root, 
		False, 
		ButtonReleaseMask | ButtonPressMask, 
		GrabModeAsync,GrabModeAsync,
		None, None
	);

	XGrabButton(
		X, 
		Button3, 
		Mod1Mask, 
		root, 
		False, 
		ButtonReleaseMask | ButtonPressMask, 
		GrabModeAsync,GrabModeAsync,
		None, None
	);

}	


int main() 
{
	
	Display * X; 
	Window root;
	int screen; 
	
	//Dummys for the XQueryPointer Function. 
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
		ButtonMotionMask |
		ButtonReleaseMask | 
		ButtonPressMask
	); 	

	// buffer for moving windows
	//-------------------------------------------
	bool state; 
	int buffer_x;
	int buffer_y;
	
	state = false;  

	//-------------------------------------------
	// Event Loop
	
	XEvent event;

	while(1)
	{
		XNextEvent(X,&event);


		// Get the window ID where the mouse is on 
		XQueryPointer(
			X,
			root,
			&root_return,&child_return, // window ID that we want
			&root_x_return,&root_y_return, 
			&win_x_return,&win_y_return, 
			&mask_return
		);
		
		// information about the window that is showing 
		
		int window_x , window_y;
		unsigned int width_return, height_return, border_width_return, depth_return; 

		if(child_return != None)
		{
			XGetGeometry(
				X, 
				child_return, 
				&root_return, 
				&window_x, &window_y, 
				&width_return, &height_return, 
				&border_width_return, &depth_return
			);
		} 
	
		
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
			
			// Dick menu
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
					printf("Close window\n");
					XKillClient(X,child_return);
				}
                		XQueryPointer(
                	        	X,
                       	 		root,
                        		&root_return,&child_return, // window ID that we want
                        		&root_x_return,&root_y_return,
                        		&win_x_return,&win_y_return,
                        		&mask_return
                		);	
			}
			
			if( (event.xkey.state & (ShiftMask | Mod1Mask)) 
			== (ShiftMask | Mod1Mask))
			{
				if(child_return != root)
				{
					printf("Close window\n");
					XKillClient(X,child_return);
				}	
			}
		}

		if(event.type == MapRequest) 
		{
			XMapWindow(X,event.xmaprequest.window); 
		}

		// move window
		if(event.type == MotionNotify && event.xmotion.state == (Mod1Mask | Button1Mask)
		&& child_return != None)
		{
			
			printf("move\n"); 
			if(state == false)
			{
				buffer_x = root_x_return - window_x;
				buffer_y = root_y_return - window_y;
				state = true; 
			}
			XMoveWindow(
				X, 
				child_return,
				root_x_return - (buffer_x), 
				root_y_return - (buffer_y)
			);
			
		}	
	
			
		if(event.type == ButtonPress && 
		(event.xbutton.button == Button1 ||  event.xbutton.button == Button3) &&
		(event.xbutton.state & Mod1Mask) == Mod1Mask &&  
		child_return != None)
		{
			printf("Grab\n");
			XGrabPointer(
				X, 
				root, 
				False, 
				ButtonMotionMask | ButtonReleaseMask ,
				GrabModeAsync,GrabModeAsync,
				None, 
				None, 
				CurrentTime
			);

		}
		
		if(event.type == ButtonRelease && 
		(event.xbutton.button == Button1 ||  event.xbutton.button == Button3) &&
		((event.xbutton.state & Mod1Mask) == Mod1Mask) && 
		child_return != None)
		{
			printf("Ungrab\n");
			XUngrabPointer(X, CurrentTime);
			state = false; 
		}
		
		// Resize
		if(event.type == MotionNotify && 
		(event.xmotion.state  == (Mod1Mask | Button3Mask)) && 
		child_return != None)
		{
			printf("resize\n");
			XResizeWindow(X, child_return, root_x_return, root_y_return);
		}
		
	}
	XCloseDisplay(X); 
	return 0; 	
		
}


/// STATUS ------------------------------------------------------------------------- 
/* 
	Phase 1 
	
	* Find a way to close windows (Completed)(Not perfected)

	* Move windows and resize them with the pointer(Completed)(Not perfected)
 
	* Make a repo for this project on GitHub (Completed) 
	
	Phase 2 

	* Code a status bar just for the time and cpu usage. 

	* Fix the firefox issue 

	Phase 3
	
	* Post about it on reddit or stack over to find contributers (unlikely....)

	* Transfer this project on to the club (if there's even people....)
	  and make this project a team one. 

	
	Final stage i think.... (IF super lucky and shit)
	
	* The whole club helps me with this project. Wait i mean our project.
	* GitHub fully working. 
	* Outside people giving feedback and if luck is on our side then help 		
          from outside also.  

---------------------------------------------------------------------------------------*/
