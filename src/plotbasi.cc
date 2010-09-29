/* plotbasi.c  93.12.22
 * Copyright 1983-1992   Albert Davis
 * vector graphics, for pen plotters, etc.
 * indirect through basic draw strings.
 */
#include "io_.h"
#include "constant.h"
#include "pixelh.h"
/*--------------------------------------------------------------------------*/
	struct graph	*initbasic(struct graph *);
static	void		text(int,int,const char*,int);
static	void		drawline(int,int,int,int,int);
static	void		finishp(void);
static	void		cancel(void);
/*--------------------------------------------------------------------------*/
static FILE *device;
static struct graph *d;
/*--------------------------------------------------------------------------*/
struct graph *initbasic(struct graph *g)
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
 fprintf(device, "bm%d,%dc%d\"%s\"\n", x, y, color, string);
}
/*--------------------------------------------------------------------------*/
static void drawline(int x1, int y1, int x2, int y2, int color)
{
 fprintf(device, "BM%d,%dC%dM%d,%d\n", x1, y1, color, x2, y2);
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
