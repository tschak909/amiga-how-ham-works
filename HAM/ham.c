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

#define WAIT_TIME 0

int selectedGadgetID = 0;
int colorIndex = 0;

char *colorLabels[16]   = {"0","1","2","3","4","5","6","7","8","9",
                           "A","B","C","D","E","F"};

int colorY[16] = {7,15,23,31,39,47,55,63,71,79,87,95,103,111,119,127};

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
      CLOSEWINDOW|MOUSEBUTTONS,             /* IDCMP flags            */
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

#define BUTTONLEFT 2
#define BUTTONTOP 2
#define BUTTONPEN 1

struct IntuiText PalRText = 
{
	BUTTONPEN, /* FrontPen */
	0, /* BackPen */
	JAM1, /* DrawMode */
	BUTTONLEFT-11, BUTTONTOP, /* LeftEdge, TopEdge */
	NULL, /* Font, Null for System font */
	"R",
	NULL /* Next Text item, none connected. */
};

struct IntuiText PalGText = 
{
	BUTTONPEN, /* FrontPen */
	0, /* BackPen */
	JAM1, /* DrawMode */
	BUTTONLEFT-11, BUTTONTOP, /* LeftEdge, TopEdge */
	NULL, /* Font, Null for System font */
	"G",
	NULL /* Next Text item, none connected. */
};

struct IntuiText PalBText = 
{
	BUTTONPEN, /* FrontPen */
	0, /* BackPen */
	JAM1, /* DrawMode */
	BUTTONLEFT-11, BUTTONTOP, /* LeftEdge, TopEdge */
	NULL, /* Font, Null for System font */
	"B",
	NULL /* Next Text item, none connected. */
};

#define SLIDERW 83
#define SLIDERH 10

SHORT sliderPoints[] = 
{
	0,0,
	SLIDERW, 0,
	SLIDERW, SLIDERH,
	0, SLIDERH,
	0,0
};

struct Border sliderBorder = 
{
	0, 0,
	1,
	0,
	JAM1,
	5,
	sliderPoints,
	NULL
};

struct PropInfo piPalSR = 
{
	AUTOKNOB|FREEHORIZ,
	0, 0,
	4096, MAXBODY,
	0, 0, 0, 0, 0, 0
};

struct PropInfo piPalSG = 
{
	AUTOKNOB|FREEHORIZ,
	0, 0,
	4096, MAXBODY,
	0, 0, 0, 0, 0, 0
};

struct PropInfo piPalSB = 
{
	AUTOKNOB|FREEHORIZ,
	0, 0,
	4096, MAXBODY,
	0, 0, 0, 0, 0, 0
};

struct Gadget gPalSB = 
{
	NULL,
	12, 40,
	SLIDERW, SLIDERH,
	NULL,
	GADGIMMEDIATE|RELVERIFY|FOLLOWMOUSE,
	PROPGADGET,
	(APTR)&sliderBorder,
	NULL,
	&PalBText,
	NULL,
	(APTR)&piPalSB,
	18,
	NULL
};

struct Gadget gPalSG = 
{
	&gPalSB,
	12, 26,
	SLIDERW, SLIDERH,
	NULL,
	GADGIMMEDIATE|RELVERIFY|FOLLOWMOUSE,
	PROPGADGET,
	(APTR)&sliderBorder,
	NULL,
	&PalGText,
	NULL,
	(APTR)&piPalSG,
	17,
	NULL
};

struct Gadget gPalSR = 
{
	&gPalSG,
	12, 12,
	SLIDERW, SLIDERH,
	NULL,
	GADGIMMEDIATE|RELVERIFY|FOLLOWMOUSE,
	PROPGADGET,
	(APTR)&sliderBorder,
	NULL,
	&PalRText,
	NULL,
	(APTR)&piPalSR,
	16,
	NULL
};

struct NewWindow nwPalette = {
      216, 145,                     /* start position         */
      100, 53,                 /* width, height          */
      0, 1,                     /* detail pen, block pen  */
      MOUSEBUTTONS|MOUSEMOVE|GADGETUP|GADGETDOWN|ACTIVEWINDOW,             
/* IDCMP flags            */
      WINDOWDRAG,            
                                /* window flags           */
      &gPalSR,                  /* ptr to first user gadget */
      NULL,                     /* ptr to user checkmark  */ 
      "Adjust",                /* window title           */ 
      NULL,                     /* pointer to screen      */
      NULL,                     /* pointer to superbitmap */
      10,10,100,100,              /* sizing limits min/max  */
      CUSTOMSCREEN              /* type of screen         */ 
      };

struct Window *wWindow;
struct Window *wPalette;

struct Screen *sScreen;

select_color(c)
int c;
{
	int i=0;

	colorIndex = c;

        /* Print the labels, with selected one getting color 1 */
	for (i=0;i<16;i++)
	{
		SetAPen(wWindow->RPort,(i == colorIndex ? 1 : 15));
		Move(wWindow->RPort,200,colorY[i]);
		Text(wWindow->RPort,colorLabels[i],1);
	}
}

ham()
{
	int i=0, j=0;

	SetAPen(wWindow->RPort,4);
	Move(wWindow->RPort,0,136);
	Text(wWindow->RPort,"B",1);

	SetAPen(wWindow->RPort,3);
	Move(wWindow->RPort,24,136);
	Text(wWindow->RPort,"BLU",3);

	SetAPen(wWindow->RPort,1);
	Move(wWindow->RPort,88,136);
	Text(wWindow->RPort,"RED",3);

	SetAPen(wWindow->RPort,2);
	Move(wWindow->RPort,156,136);
	Text(wWindow->RPort,"GRN",3);

	select_color(0);

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
        return 0;
    }

    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

    if (!IntuitionBase)
    {
        return 0;
    }

    sScreen = (struct Screen *)OpenScreen(&nsScreen);

    if (!sScreen)
    {
        return 0;
    }

    nwWindow.Screen = sScreen;
    nwPalette.Screen = sScreen;

    wWindow = (struct Window *)OpenWindow(&nwWindow);

    if (!wWindow)
    {
        return 0;
    }

    wPalette = (struct Window *)OpenWindow(&nwPalette);

    if (!wPalette)
    {
	return 0;
    }

    return 1;
}

done()
{
     if (wPalette)
         CloseWindow(wPalette);

     if (wWindow)
         CloseWindow(wWindow);

     if (sScreen)
         CloseScreen(sScreen);

     if (IntuitionBase)
         CloseLibrary(IntuitionBase); 
     
     if (GfxBase)
         CloseLibrary(GfxBase);
}

adjust_color(cc, nc)
int cc,nc;
{
	SHORT r,g,b,c;
	struct ViewPort *vp;
	struct ColorMap *cm;

	vp = &sScreen->ViewPort;
	cm = vp->ColorMap;
	c = GetRGB4(cm,colorIndex);
	b = c & 0x0F;
	g = (c >> 4) & 0x0F;
	r = (c >> 8) & 0x0F;

	switch (cc)
	{
		case 0:
			r = nc;
			break;
		case 1:
			g = nc;
			break;
		case 2:
			b = nc;
			break;
	}

	SetRGB4(vp, colorIndex, r, g, b);
}

handle_palette(class, code, gadget)
USHORT class, code;
struct Gadget *gadget;
{
    struct IntuiMessage *msg;

    while (msg = (struct IntuiMessage *)GetMsg((APTR)wPalette->UserPort))
    {
	int class, code;
        struct Gadget *gadget;

        class  = msg->Class;
        code   = msg->Code;
        gadget = (struct Gadget *)msg->IAddress;

	ReplyMsg(msg);

        if (class == GADGETDOWN)
        {
	    selectedGadgetID = gadget->GadgetID;
        }
	else if (class == GADGETUP)
	{
	    if (selectedGadgetID == 16)
		adjust_color(0,piPalSR.HorizPot >> 12);
	    else if (selectedGadgetID == 17)
		adjust_color(1,piPalSG.HorizPot >> 12);
	    else if (selectedGadgetID == 18)
		adjust_color(2,piPalSB.HorizPot >> 12);

	    selectedGadgetID = 0;
	}
        else if (class == MOUSEMOVE)
        {
	    if (selectedGadgetID == 16)
                adjust_color(0,piPalSR.HorizPot >> 12);
            else if (selectedGadgetID == 17)
                adjust_color(1,piPalSG.HorizPot >> 12);
            else if (selectedGadgetID == 18)
                adjust_color(2,piPalSB.HorizPot >> 12);
        }
    }
}

main()
{
    ULONG signals;

    if (!init())
    {
        done();
        return 1;
    }

    ham();

    FOREVER
    {
	signals = Wait((1 << wWindow->UserPort->mp_SigBit) |
                       (1 << wPalette->UserPort->mp_SigBit));

	if (signals & (1 << wWindow->UserPort->mp_SigBit))
	{
	    struct IntuiMessage *msg;
	    while (msg = (struct IntuiMessage *)
                             GetMsg((APTR)wWindow->UserPort))
            {
		int class,mx,my;
		class = msg->Class;
		mx = msg->MouseX;
		my = msg->MouseY;
		
		ReplyMsg(msg);

		if (class == MOUSEBUTTONS)
		{
		    if (my<8+12)
			select_color(0);
		    else if (my<16+12)
			select_color(1);
		    else if (my<24+12)
			select_color(2);
		    else if (my<32+12)
			select_color(3);
		    else if (my<40+12)
			select_color(4);
		    else if (my<48+12)
			select_color(5);
		    else if (my<56+12)
			select_color(6);
		    else if (my<64+12)
			select_color(7);
		    else if (my<72+12)
			select_color(8);
		    else if (my<80+12)
			select_color(9);
		    else if (my<88+12)
			select_color(10);
		    else if (my<96+12)
			select_color(11);
		    else if (my<104+12)
			select_color(12);
		    else if (my<112+12)
			select_color(13);
		    else if (my<120+12)
			select_color(14);
		    else if (my<128+12)
			select_color(15);
		}
		else if (class == CLOSEWINDOW)
			goto quit;
            }
	}

	if (signals & (1 << wPalette->UserPort->mp_SigBit))
	{
	    handle_palette();
	}

    }

quit:
    done();
}
