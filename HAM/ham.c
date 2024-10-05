/**
 * Single Window Application
 */

#include "exec/types.h"
#include "exec/io.h"
#include "exec/memory.h"

#include "graphics/gfx.h"
#include "hardware/dmabits.h"
#include "hardware/custom.h"
#include "hardware/blit.h"
#include "graphics/gfxmacros.h"
#include "graphics/copper.h"
#include "graphics/view.h"
#include "graphics/gels.h"
#include "graphics/regions.h"
#include "graphics/clip.h"
#include "exec/exec.h"
#include "graphics/text.h"
#include "graphics/gfxbase.h"

#include "libraries/dos.h"
#include "graphics/text.h"
#include "intuition/intuition.h"

/* The size of each chip */
#define XSIZE 4
#define YSIZE 8

/* Add this to set the top most HAM bits */
#define HOLDRG 0x10
#define HOLDGB 0x20
#define HOLDRB 0x30

#define WAIT_TIME 100

extern struct Window *OpenWindow();
extern struct Screen *OpenScreen();

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct NewScreen nsScreen = {
    0, 0,                       /* start position         */
    320, 200, 6,                /* width, height, depth   */
    0, 1,                       /* detail pen, block pen  */
    HAM,                      /* viewing mode           */
    CUSTOMSCREEN,               /* screen type            */
    NULL,                       /* use default font       */
    "How HAM Works",                  /* screen title           */
    NULL                        /* ptr to addit'l gadgets */
};

struct NewWindow nwWindow = {
      0, 12,                     /* start position         */
      320, 188,                 /* width, height          */
      0, 1,                     /* detail pen, block pen  */
      CLOSEWINDOW,             /* IDCMP flags            */
      WINDOWCLOSE|GIMMEZEROZERO|ACTIVATE,            
                                /* window flags           */
      NULL,                     /* ptr to first user gadget */
      NULL,                     /* ptr to user checkmark  */ 
      "Nice Delicious HAM",                /* window title           */ 
      NULL,                     /* pointer to screen      */
      NULL,                     /* pointer to superbitmap */
      0,0,320,188,              /* sizing limits min/max  */
      CUSTOMSCREEN              /* type of screen         */ 
      };

struct Window *wWindow;

struct Screen *sScreen;

ham()
{
	int i=0, j=0;

	SetAPen(wWindow->RPort,15);

	Move(wWindow->RPort,200,7);
	Text(wWindow->RPort,"00",2);

	Move(wWindow->RPort,200,15);
	Text(wWindow->RPort,"01",2);
	
	Move(wWindow->RPort,200,23);
	Text(wWindow->RPort,"02",2);

	Move(wWindow->RPort,200,31);
	Text(wWindow->RPort,"03",2);

	Move(wWindow->RPort,200,39);
	Text(wWindow->RPort,"04",2);

	Move(wWindow->RPort,200,47);
	Text(wWindow->RPort,"05",2);

	Move(wWindow->RPort,200,55);
	Text(wWindow->RPort,"06",2);

	Move(wWindow->RPort,200,63);
	Text(wWindow->RPort,"07",2);

	Move(wWindow->RPort,200,71);
	Text(wWindow->RPort,"08",2);

	Move(wWindow->RPort,200,79);
	Text(wWindow->RPort,"09",2);

	Move(wWindow->RPort,200,87);
	Text(wWindow->RPort,"10",2);

	Move(wWindow->RPort,200,95);
	Text(wWindow->RPort,"11",2);

	Move(wWindow->RPort,200,103);
	Text(wWindow->RPort,"12",2);

	Move(wWindow->RPort,200,111);
	Text(wWindow->RPort,"13",2);

	Move(wWindow->RPort,200,119);
	Text(wWindow->RPort,"14",2);

	Move(wWindow->RPort,200,127);
	Text(wWindow->RPort,"15",2);

	Move(wWindow->RPort,0,136);
	Text(wWindow->RPort,"B",1);

	Move(wWindow->RPort,24,136);
	Text(wWindow->RPort,"BLU",3);

	Move(wWindow->RPort,88,136);
	Text(wWindow->RPort,"RED",3);

	Move(wWindow->RPort,156,136);
	Text(wWindow->RPort,"GRN",3);

	SetWindowTitles(wWindow,"HAM Base Colors 0-16",-1);

	/* First column of base colors */
        for (i=0;i<16;i++)
	{
		SetAPen(wWindow->RPort,i);
		RectFill(wWindow->RPort,
				0,i*YSIZE,
				XSIZE,i*YSIZE+YSIZE);
	}

	Delay(WAIT_TIME);

	/* Hold and modify colors */

        for (i=0;i<16;i++)
	{
		int xo = XSIZE;

		SetWindowTitles(wWindow,"HAM hold RG; change B",-1);

		for (j=0;j<16;j++)
		{
			SetAPen(wWindow->RPort,j+HOLDRG);
			RectFill(wWindow->RPort,
					xo,i*YSIZE,
					xo+XSIZE,i*YSIZE+YSIZE);
			xo += XSIZE;
		}

		Delay(WAIT_TIME);

		SetWindowTitles(wWindow,"HAM hold GB; change R",-1);

		for (j=0;j<16;j++)
		{
			SetAPen(wWindow->RPort,j+HOLDGB);
			RectFill(wWindow->RPort,
					xo,i*YSIZE,
					xo+XSIZE,i*YSIZE+YSIZE);
			xo += XSIZE;
		}

		Delay(WAIT_TIME);

		SetWindowTitles(wWindow,"HAM hold RB, change G",-1);

		for (j=0;j<16;j++)
		{
			SetAPen(wWindow->RPort,j+HOLDRB);
			RectFill(wWindow->RPort,
					xo,i*YSIZE,
					xo+XSIZE,i*YSIZE+YSIZE);
			xo += XSIZE;
		}

		Delay(WAIT_TIME);
	}

	SetWindowTitles(wWindow,"Re-Add Base Colors",-1);

	/* Second column of base colors */
        for (i=0;i<16;i++)
	{
		SetAPen(wWindow->RPort,i);
		RectFill(wWindow->RPort,
				64,i*YSIZE,
				64+XSIZE,i*YSIZE+YSIZE);
	}

	Delay(WAIT_TIME);

	/* Third column of base colors */
        for (i=0;i<16;i++)
	{
		SetAPen(wWindow->RPort,i);
		RectFill(wWindow->RPort,
				128,i*YSIZE,
				128+XSIZE,i*YSIZE+YSIZE);
	}

	Delay(WAIT_TIME);

	/* Finally, apply custom color palette */

	SetWindowTitles(wWindow, "Custom Color Palette",-1);

	SetRGB4(&sScreen->ViewPort, 0, 00, 00, 00);
	Delay(10);
	SetRGB4(&sScreen->ViewPort, 1, 15, 00, 00);
	Delay(10);
	SetRGB4(&sScreen->ViewPort, 2, 00, 15, 00);
	Delay(10);
	SetRGB4(&sScreen->ViewPort, 3, 00, 00, 15);
	Delay(10);
	SetRGB4(&sScreen->ViewPort, 4, 15, 15, 15);

	SetWindowTitles(wWindow, "<-- Press to close.",-1);
}

init()
{
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);

    if (!GfxBase)
    {
    	printf("Could not open graphics library.\n");
        return 0;
    }

    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

    if (!IntuitionBase)
    {
    	printf("Could not open intuition library.\n");
        return 0;
    }

    sScreen = (struct Screen *)OpenScreen(&nsScreen);

    if (!sScreen)
    {
	printf("Could not open our screen.\n");
        return 0;
    }

    nwWindow.Screen = sScreen;

    wWindow = (struct Window *)OpenWindow(&nwWindow);

    if (!wWindow)
    {
    	printf("Could not open our window.\n");
        return 0;
    }

    return 1;
}

done()
{
     if (wWindow)
         CloseWindow(wWindow);

     if (sScreen)
         CloseScreen(sScreen);

     if (IntuitionBase)
         CloseLibrary(IntuitionBase);
     
     if (GfxBase)
         CloseLibrary(GfxBase);
}

main()
{
    if (!init())
    {
        done();
        return 1;
    }

    ham();

    while (Wait(1 << wWindow->UserPort->mp_SigBit))
    {
        struct IntuiMessage *message;
        
        message = GetMsg(wWindow->UserPort);

        ReplyMsg(message);

        if (message->Class == CLOSEWINDOW)
            break;    
    }

    done();
}
