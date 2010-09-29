/* plotline.c  93.12.19
 * from C-Ware, and modified to be user installable?
 */
#include "pixelh.h"
#include "declare.h"	/* self */
/*LINTLIBRARY*/
/*--------------------------------------------------------------------------*/
	void initgraph(struct graph *);
	void stext(int,int,const char*,int);
	void setpixel(int,int,int);
	void box(int,int,int,int,int);
	void line(int,int,int,int,int);
static	void badpixel(int,int,int);
static	void badbox(int,int,int,int,int);
static	void badline(int,int,int,int,int);
/*--------------------------------------------------------------------------*/
#define sign(x)	    ((x < 0) ? 1 : 0)

static void (*d_setpix)(int,int,int);		/* setpixel function */
static void (*d_line)(int,int,int,int,int);	/* line function */
static void (*d_box)(int,int,int,int,int);	/* box function */
static void (*d_text)(int,int,const char*,int);	/* text function */
/*--------------------------------------------------------------------------*/
void initgraph(struct graph *d)
{
if (d->spx || d->lin)
    {
    if (d->spx)
	d_setpix = d->spx;
    else
	d_setpix = badpixel;

    if (d->lin)
	d_line = d->lin;
    else
	d_line = badline;

    if (d->box)
	d_box = d->box;
    else
	d_box = badbox;

    d_text = (d->txt) ;
    }
}
/*--------------------------------------------------------------------------*/
void stext(int x, int y, const char *str, int color)
{
(*d_text)(x,y,str,color);
}
/*--------------------------------------------------------------------------*/
void setpixel(int x, int y, int color)
{
(*d_setpix)(x,y,color);
}
/*--------------------------------------------------------------------------*/
void box(int x1, int y1, int x2, int y2, int color)
{
(*d_box)(x1,y1,x2,y2,color);
}
/*--------------------------------------------------------------------------*/
void line(int x1, int y1, int x2, int y2, int color)
{
(*d_line)(x1,y1,x2,y2,color);
}
/*--------------------------------------------------------------------------*/

static void badpixel(int x, int y, int color)
{
line(x,y,x,y,color);
}
/*--------------------------------------------------------------------------*/
static void badbox(int x1, int y1, int x2, int y2, int color)
{
line(x1,y1,x2,y1,color);
line(x2,y1,x2,y2,color);
line(x2,y2,x1,y2,color);
line(x1,y2,x1,y1,color);
}
/*--------------------------------------------------------------------------*/
static void badline(int x1, int y1, int x2, int y2, int color)
{
int x, y, dx, dy, adx, ady, d, d1, d2, step;

dx  = x2 - x1;
adx = abs(dx);
dy  = y2 - y1;
ady = abs(dy);

if (adx == 0)		    /* Vertical line */
    {
    y = (y1 < y2) ? y1 : y2;
    ady++;
    while (ady--)
	(*d_setpix)(x1, y++, color);
    return;
    }

if (ady == 0)		    /* Horizontal line */
    {
    x = (x1 < x2) ? x1 : x2;
    adx++;
    while (adx--)
	(*d_setpix)(x++, y1, color);
    return;
    }

if (adx < ady)
    {
    d  = (adx << 1) - ady;
    d1 = adx << 1;
    d2 = (adx - ady) << 1;
    (y1 < y2) ? (x = x1, y = y1) : (x = x2, y = y2);
    step = ((sign(dx) == sign(dy)) ? 1 : -1);
    (*d_setpix)(x, y, color);
    while (ady--)
	{
	y++;
	if (d < 0)
	    d += d1;
	else
	    {
	    x += step;
	    d += d2;
	    }
	(*d_setpix)(x, y, color);
	}
    }
else
    {
    d  = (ady << 1) - adx;
    d1 = ady << 1;
    d2 = (ady - adx) << 1;
    (x1 < x2) ? (x = x1, y = y1) : (x = x2, y = y2);
    step = ((sign(dx) == sign(dy)) ? 1 : -1);
    (*d_setpix)(x, y,color);
    while (adx--)
	{
	x++;
	if (d < 0)
	    d += d1;
	else
	    {
	    y += step;
	    d += d2;
	    }
	(*d_setpix)(x, y, color);
	}
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
