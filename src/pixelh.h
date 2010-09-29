/*$Id: pixelh.h,v 13.15 1999/01/18 06:29:03 al Exp $
 * for defining the graphics device
 */
#ifndef PIXELH_H
#define PIXELH_H
/*--------------------------------------------------------------------------*/
#define RED   2
#define GREEN 4
#define YELLOW 5
#define WHITE 7
struct graph
	{			/* graphic device parameters */
	int	ppc ;		/* pixels per character */
	int	lpc ;		/* lines per character */
	int sw ;		/* screen width in pixels */
	int sh ;		/* screen height in pixels */
	int ww ;		/* window width in pixels */
	int wh ;		/* window height in pixels */
	int lm ;		/* location of left margin */
	int rm ;		/* right margin (from left) */
	int top;		/* top margin */
	int bm ;		/* bottom margin (from top) */
	void (*spx)(int,int,int);		/* setpixel function */
	void (*lin)(int,int,int,int,int);	/* line function */
	void (*box)(int,int,int,int,int);	/* box function */
	void (*txt)(int,int,const char*,int);	/* text function */
	void (*can)();				/* cancel graphics mode */
	void (*fin)();				/* finish the graph */
	int gmode;		/* graphics mode number */
	int tmode;		/* text mode number */
	int pri;		/* primary trace color */
	int sec;		/* secondary trace color */
	int grid;		/* grid color */
	int back;		/* background color */
	int palette;	/* color palette (ibm) */
	} ;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
