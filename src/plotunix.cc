/* plotunix.c  93.12.22
 * Copyright 1983-1992   Albert Davis
 * vector graphics, for pen plotters, etc.
 * indirect through unix plot filters
 * This is bogus.  It might work with minor changes.
 */
#include "io_.h"
#include "constant.h"
#include "pixelh.h"
/*--------------------------------------------------------------------------*/
	struct graph	*initunix(struct graph *);
static	void		text(int,int,const char*,int);
static	void		drawline(int,int,int,int,int);
static	void		finishp(void);
static	void		cancel(void);
/*--------------------------------------------------------------------------*/
static FILE *device;
static struct graph *d;
/*--------------------------------------------------------------------------*/
struct graph *initunix(struct graph *g)
{
 d = g;
 d->spx = NULL;
 d->lin = drawline;
 d->box = NULL;
 d->txt = text;
 d->can = cancel;
 d->fin = finishp;
 xclose( &device );
 device = fopen( PLOTFILE, "wt" ) ;
 return d;
}
/*--------------------------------------------------------------------------*/
static void text(int x, int y, const char *string, int color)
{
 int red, green, blue;
 red   = (color & 0x03) << 2;
 green = ((color>>2) & 0x03) << 2;
 blue  = ((color>>4) & 0x03) << 2;
 fprintf( device, "C%c%c%cm%c%c%c%c\"%s\"", red, green, blue,
	(x&0x7f), ((x>>8)&0x7f), (y&0x7f), ((y>>8)&0x7f), string );
}
/*--------------------------------------------------------------------------*/
static void drawline(int x1, int y1, int x2, int y2, int color)
{
 int red, green, blue;
 red   = (color & 0x03) << 2;
 green = ((color>>2) & 0x03) << 2;
 blue  = ((color>>4) & 0x03) << 2;
 fprintf( device, "C%c%c%cl%c%c%c%c%c%c%c%c", red, green, blue,
	(x1&0x7f), ((x1>>8)&0x7f), (y1&0x7f), ((y1>>8)&0x7f),
	(x2&0x7f), ((x2>>8)&0x7f), (y2&0x7f), ((y2>>8)&0x7f) );

}
/*--------------------------------------------------------------------------*/
static void finishp(void)
{
 xclose( &device );
 if ( access("/dev/ecaplot", 00) == FILE_BAD) /* no driver */	/* is exe   */
   if (    !findfile("ecaplot.com", OS::getenv("PATH"), X_OK).empty()
	|| !findfile("ecaplot.exe", OS::getenv("PATH"), X_OK).empty()
	|| !findfile("ecaplot.bat", OS::getenv("PATH"), X_OK).empty() )
	system( "ecaplot" );
}
/*--------------------------------------------------------------------------*/
static void cancel(void)
{;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
