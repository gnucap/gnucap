/*$Id: inverse.cc,v 24.5 2003/04/27 01:05:05 al Exp $ -*- C++ -*-
 * Copyright (C) 2002 Telford Tendys
 * Author: Telford Tendys <telford@triode.net.au>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * 
 * The purpose is to find the inverse of a function by numerical
 * methods. We are given:
 *      f(x) = y (where y is constant and known)
 *      f(x) may be evaluated as a black-box for any x
 *      f'(x) is also evaluated for any x
 *
 * We need to find x (starting from initial guess x0)
 * 
 */
#define NDEBUG

#include <math.h>
#include <assert.h>
#include "e_elemnt.h"
#include "u_opt.h"

#ifdef TEST_MODE
#include <stdio.h>
#endif // TEST_MODE

/*
 * FIXME: Some arbitrary epsilon constants have been used here
 */
static void newtons_method_of_guessing( ELEMENT *E, double delta_y )
{
#ifdef TEST_MODE
	printf( "Using Newton's Method\n" );
#endif // TEST_MODE
	if( fabs( E->_y0.f1 ) > 1e-50 )
	{
		E->_y0.x -= delta_y / E->_y0.f1;
	}
	else
	{
		if( E->_y0.f1 > 0 )
			E->_y0.x -= delta_y * 1e50; /* This is not good */
		else
			E->_y0.x += delta_y * 1e50; /* This is not good either */
	}
}

/*
 * Inputs:
 *    y is in E->_y0.f0
 *    x0 (initial guess) is in E->_y0.x
 *
 * E->tr_eval() will evaluate new values for E->_y0.f0 and E->_y0.f1
 *
 * Outputs:
 *    y goes back into E->_y0.f0 (actually our best result goes here)
 *    x goes into E->_y0.x
 *
 * Return values:
 *    0 indicates successful convergence
 *   -1 indicates ran out of iterations
 *   -2 indicates problem with E
 *
 * (probably should have some NaN checks that it can bomb out with)
 */
int ELEMENT::inverse()
{
	int iter, state = 0;
	double target_y, scale;
	double low_x = 0, low_y = 0;
	double high_x = 0, high_y = 0;
	register double delta_y; /* A workhorse variable */

	if( !has_tr_eval()) return( -2 ); /* No function available */
	target_y = _y0.f0;
	scale = fabs( target_y );
	if( scale < 1e-50 ) scale = 1e-50; /* FIXME: use options */
	for( iter = 200; iter--; )
 	{
#ifdef TEST_MODE
		printf( "Before eval: state=%d target_y=%g x=%g f0=%g f1=%g\n",
				state, target_y, _y0.x, _y0.f0, _y0.f1 );
#endif // TEST_MODE
		common()->tr_eval(this);
		delta_y = _y0.f0 - target_y;
#ifdef TEST_MODE
		printf( "After eval: delta_y=%g x=%g f0=%g f1=%g\n",
				delta_y, _y0.x, _y0.f0, _y0.f1 );
#endif // TEST_MODE
		if( fabs( delta_y ) / scale < 1e-10 ) return( 0 ); /* FOUND! */
		switch( state )
		{
/*
 * Nothing is valid, we have an initial guess only
 * Figure out what side of the fence it sits on and thus get
 * either a valid high or a valid low. From there, use a 
 * Newton's Method step to have a stab at a second guess.
 * Frequently this stab will actually be very good but we
 * don't actually trust it.
 */
			case 0:
				low_x = high_x = _y0.x;
				low_y = high_y = _y0.f0;
				if( delta_y > 0 )
				{
					state = 1;
				}
				else
				{
					state = 2;
				}
				newtons_method_of_guessing( this, delta_y );
				break;
/*
 * If possible, straddle a zero crossing and jump to binary search
 * but if we can't find the straddle then use Newton's Method as a fallback.
 */
			case 1:
				assert( high_y > target_y );
				assert( low_x == high_x );
				assert( low_y == high_y );
				if( delta_y < 0 )  /* Is he straddlin'? */
				{
					if( high_x > _y0.x )
					{
					    low_x = _y0.x;
						low_y = _y0.f0;
						state = 3;
					}
					else
					{
					    high_x = _y0.x;
						high_y = _y0.f0;
						state = 4;
					}
 midpoint_binary_search:
#ifdef TEST_MODE
					printf( "Using Binary Search\n" );
#endif // TEST_MODE
					_y0.x = ( low_x + high_x ) / 2.0;
					break;
				}
				if( high_x > _y0.x )
				{
					low_y = _y0.f0;
					low_x = _y0.x;
				}
				else
				{
					high_y = _y0.f0;
					high_x = _y0.x;
				}
				newtons_method_of_guessing( this, delta_y );
				state = 5;
				break;
/*
 * If possible, straddle a zero crossing and jump to binary search
 * but if we can't find the straddle then use Newton's Method as a fallback.
 */
			case 2:
				assert( low_y < target_y );
				assert( low_x == high_x );
				assert( low_y == high_y );
				if( delta_y > 0 )  /* Is he straddlin'? */
				{
					if( low_x < _y0.x )
					{
					    high_x = _y0.x;
						high_y = _y0.f0;
						state = 3;
					}
					else
					{
					    low_x = _y0.x;
						low_y = _y0.f0;
						state = 4;
					}
					goto midpoint_binary_search;
				}
				if( high_x > _y0.x )
				{
					low_y = _y0.f0;
					low_x = _y0.x;
				}
				else
				{
					high_y = _y0.f0;
					high_x = _y0.x;
				}
				newtons_method_of_guessing( this, delta_y );
				state = 6;
				break;
/*
 * We have a straddle, narrow the region to a smaller straddle
 * and attempt to apply Newton's Method...
 * if that fails use a binary search as a fallback
 */
			case 3:
				assert( high_y > target_y );
				assert( low_y < target_y );
				assert( high_x > low_x );
				assert( _y0.x >= low_x );
				assert( _y0.x <= high_x );

				if( delta_y > 0 )
				{
					high_x = _y0.x;
					high_y = _y0.f0;
				}
				else
				{
					low_x = _y0.x;
					low_y = _y0.f0;
				}
				newtons_method_of_guessing( this, delta_y );
				if( _y0.x < low_x ) goto midpoint_binary_search;
				if( _y0.x > high_x ) goto midpoint_binary_search;
				break;
/*
 * We have a straddle, narrow the region to a smaller straddle
 * and attempt to apply Newton's Method...
 * if that fails use a binary search as a fallback
 */
			case 4:
				assert( high_y < target_y );
				assert( low_y > target_y );
				assert( high_x > low_x );
				assert( _y0.x >= low_x );
				assert( _y0.x <= high_x );

				if( delta_y < 0 )
				{
					high_x = _y0.x;
					high_y = _y0.f0;
				}
				else
				{
					low_x = _y0.x;
					low_y = _y0.f0;
				}
				newtons_method_of_guessing( this, delta_y );
				if( _y0.x < low_x ) goto midpoint_binary_search;
				if( _y0.x > high_x ) goto midpoint_binary_search;
				break;
/*
 * We still haven't found a straddle and we have already had two tries
 * at Newton's Method. It we have a straddle by now then fine,
 * if not then start to expand our span.
 */
			case 5:
				assert( high_y > target_y );
				assert( low_y > target_y );
				assert( high_x > low_x );
				if( delta_y < 0 )  /* Is he straddlin'? */
				{
					if( high_x > _y0.x )
					{
					    low_x = _y0.x;
						low_y = _y0.f0;
						state = 3;
					}
					else
					{
					    low_x = high_x;
						low_y = high_y;
					    high_x = _y0.x;
						high_y = _y0.f0;
						state = 4;
					}
					goto midpoint_binary_search;
				}
#ifdef TEST_MODE
				printf( "Widening the span in search of straddle\n" );
#endif // TEST_MODE
				if( _y0.x < low_x )
				{
					low_y = _y0.f0;
					low_x = _y0.x;
					_y0.x = high_x + high_x - low_x;
				}
				else if( _y0.x > high_x )
				{
					high_y = _y0.f0;
					high_x = _y0.x;
					_y0.x = low_x + low_x - high_x;
				}
				else
				{
					_y0.x = low_x + low_x - high_x;
				}
				break;
/*
 * Still no straddle... same as case 5 except we are on the other side
 */
			case 6:
				assert( high_y < target_y );
				assert( low_y < target_y );
				assert( high_x > low_x );
				if( delta_y > 0 )  /* Is he straddlin'? */
				{
					if( low_x < _y0.x )
					{
					    high_x = _y0.x;
						high_y = _y0.f0;
						state = 3;
					}
					else
					{
					    low_x = _y0.x;
						low_y = _y0.f0;
						state = 4;
					}
					goto midpoint_binary_search;
				}
#ifdef TEST_MODE
				printf( "Widening the span in search of straddle\n" );
#endif // TEST_MODE
				if( _y0.x < low_x )
				{
					low_y = _y0.f0;
					low_x = _y0.x;
					_y0.x = high_x + high_x - low_x;
				}
				else if( _y0.x > high_x )
				{
					high_y = _y0.f0;
					high_x = _y0.x;
					_y0.x = low_x + low_x - high_x;
				}
				else
				{
					_y0.x = high_x + high_x - low_x;
				}
				break;
		}
	}
/*
 * We ran out of iterations, there isn't much we can do here
 * so just return an error. Possibly the error code should be
 * based on the state but at the moment the error code doesn't
 * give much info about how close we got
 */
	return( -1 );
}

/*
 * Telford's Knee-chord method of estimating a linear model for saturated inductors
 *
 * NB: All this is written from the point of view of an inductor (see d_coil.cc)
 * but the principle is not limited to any particular device.
 *
 * -- Find the point that is off the main curve which is the system operating point
 *    outside of this device (if we presume that we are the only non-linear device
 *    in the world then this point is the intersection of the load-line of everything
 *    except this device and the previous estimated linear model of this device).
 *
 *    The point in question (call it OP1) is:
 *          (current=_y0.x, flux={_y1.f0+_y1.f1*(y0.x-y1.x)})
 *
 *    This is obtained by taking the most recent current estimate and plugging it into
 *    the PREVIOUS linear estimate of the device behaviour.
 *
 * -- Draw a line horizontal from OP1 until it touches the non-linear curve
 *    for this device (this is standard inversion of a well-behaved function,
 *    we use a hybrid of Newton's Method and a binary search for this purpose).
 *    By horizontal, I mean keep constant flux and adjust the current.
 *
 *    This leads us to OP2 which is:
 *          (current=<result of function inversion>, flux=<same as OP1>)
 *
 * -- Draw a vertical line from OP1 to touch the non-linear curve
 *    (this is just a standard tr_eval() which has just been done so we merely
 *    save the _y0.f0 and _y0.x values and restore them after we have finished
 *    with function inversions). Call this point OP3 (this is the operating point
 *    that the standard Newton's Method uses as a working estimate of the system,
 *    the only difference between the Knee-chord method and the standard method is
 *    the value of _y0.f1)
 *
 * -- Find the slope of the interval connecting OP2 and OP3 and use this slope
 *    as the _y0.f1 value rather than the simple derivative of the curve.
 *
 *    Using this slope has a number of advantages over Newton's Method with regard
 *    to stability. If OP1 is close to the non-linear curve then we are close
 *    to convergence and we can say that the non-linear curve is close to linear
 *    in the local region. Thus OP2 and OP3 will be close together and the inverval
 *    between them will essentially be the same as the local function derivative
 *    (actually, once OP2 and OP3 are sufficiently close, it is better to switch
 *    back to Newton's Method since the derivative will be calculated more accurately).
 *    If OP2 and OP3 are far apart then the slope becomes a broad average that
 *    estimates the general behaviour of the non-linear curve in the region near
 *    where we are operating. This is important when we are far from converged
 *    so that stability is maintained.
 *
 * Probably this should return a success/failure code but I don't know what
 * to use the returned value for so don't bother right now
 */
void ELEMENT::knee_chord()
{
	double save_x, save_f0, save_f1;

	if( OPT::strategy != stKNEECHORD ) return; /* This strategy not in use */
	if( !has_tr_eval()) return; /* No function available */
/*
 * Save the standard values -- these are OP3
 */
	save_x = _y0.x;
	save_f0 = _y0.f0;
/*
 * Save this too, just in case we can't get the result we want
 * (fallback to standard algorithm)
 */
	save_f1 = _y0.f1;
/*
 * Project the old linear estimate of device behaviour onto the new coil current
 * in order to come up with a flux linkage value (in _y0.f0) gives us OP1
 */
	_y0.f0 = _y1.f0 + _y1.f1 * ( _y0.x - _y1.x );
/*
 * Invert the function so that we find a _y0.x value that gives _y0.f0
 * (thus we get OP2)
 */
	if( 0 > inverse())
	{
		untested();
		error( bPICKY, long_label() + ": function inversion failed\n" );
		_y0.f1 = save_f1; /* Fallback */
	}
	else
	{
/*
 * Find the all-important chord gradient
 */
		double numerator = _y0.f0 - save_f0;
		double denominator = _y0.x - save_x;

		if( fabs( denominator ) > 1e-50 )
		{
			_y0.f1 = numerator / denominator;
#ifdef TEST_MODE
			printf( "Chord gradient is %g\n", _y0.f1 );
#endif // TEST_MODE
		}
		else
		{
#ifdef TEST_MODE
			printf( "Fallback because OP2 and OP3 are too close\n" );
#endif // TEST_MODE
			_y0.f1 = save_f1; /* Fallback */
		}
	}
/*
 * Back to the original point again
 * (Come to think of it, leaving it at OP2 might also work)
 */
	_y0.x = save_x;
	_y0.f0 = save_f0;
}
