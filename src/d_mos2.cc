/* $Id: d_mos2.model,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
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
 * mos model equations: spice level 2 equivalent
 */
/* This file is automatically generated. DO NOT EDIT */

#include "l_compar.h"
#include "l_denoise.h"
#include "ap.h"
#include "d_mos2.h"
/*--------------------------------------------------------------------------*/
const double NA(NOT_INPUT);
const double INF(BIGBIG);
/*--------------------------------------------------------------------------*/
int MODEL_MOS2::_count = 0;
/*--------------------------------------------------------------------------*/
SDP_MOS2::SDP_MOS2(const COMMON_COMPONENT* cc)
  :SDP_MOS123(cc)
{
  assert(cc);
  const COMMON_MOS* c = prechecked_cast<const COMMON_MOS*>(cc);
  assert(c);
  const MODEL_MOS2* m = prechecked_cast<const MODEL_MOS2*>(c->model());
  assert(m);
  relxj = ((m->xj != NA && m->xj > 0)
	       ? .5 * m->xj / l_eff
	       : NA);
  eta_1 = ((cgate != 0)
	       ? (kPI/4.) * E_SI * m->delta / cgate * l_eff
	       : 0.);
  eta = eta_1 + 1.;
  eta_2 = eta / 2.;
}
/*--------------------------------------------------------------------------*/
TDP_MOS2::TDP_MOS2(const DEV_MOS* d)
  :TDP_MOS123(d)
{
  assert(d);
  const COMMON_MOS* c = prechecked_cast<const COMMON_MOS*>(d->common());
  assert(c);
  const SDP_MOS2* s = prechecked_cast<const SDP_MOS2*>(c->sdp());
  assert(s);
  const MODEL_MOS2* m = prechecked_cast<const MODEL_MOS2*>(c->model());
  assert(m);

      double temp = SIM::temp;
      double tempratio  = temp / m->_tnom; // ratio
      double tempratio4 = tempratio * sqrt(tempratio);
      double kt = temp * K;
      double egap = 1.16 - (7.02e-4*temp*temp) / (temp+1108.);
      double arg = (m->egap*tempratio - egap) / (2*kt);
  vt = kt / Q;
  phi = m->phi*tempratio + (-2*vt*(1.5*log(tempratio)+Q*(arg)));
  sqrt_phi = sqrt(phi);
  phi_sqrt_phi = phi * sqrt_phi;
  beta = m->kp * tempratio4 * s->w_eff / s->l_eff;
  uo = m->uo * tempratio4;
  vbi = (fixzero(
	(m->vto - m->polarity * m->gamma * sqrt(m->phi)
	 +.5*(m->egap-egap) + m->polarity* .5 * (phi-m->phi)), m->phi));
}
/*--------------------------------------------------------------------------*/
MODEL_MOS2::MODEL_MOS2()
  :MODEL_MOS123(),
   kp(NA),
   nfs(0.0),
   vmax(NA),
   neff(1.0),
   ucrit(1e4*ICM2M),
   uexp(NA),
   utra(NA),
   delta(0.0),
   calc_kp(false),
   alpha(NA),
   xd(NA),
   xwb(NA),
   vbp(NA),
   cfsox(NA)
{
  ++_count;
  mjsw = .33;
  tox = 1e-7;
  cox = NA;
  vto = NA;
  gamma = NA;
  phi = NA;
  mos_level = LEVEL;
}
/*--------------------------------------------------------------------------*/
bool MODEL_MOS2::parse_front(CS& cmd)
{
  return MODEL_MOS123::parse_front(cmd);
}
/*--------------------------------------------------------------------------*/
bool MODEL_MOS2::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "DIODElevel", &mos_level)
    || get(cmd, "KP", &kp)
    || get(cmd, "NFS", &nfs, mSCALE, ICM2M2)
    || get(cmd, "VMAx", &vmax)
    || get(cmd, "NEFf", &neff, mPOSITIVE)
    || get(cmd, "UCRit", &ucrit, mSCALE, ICM2M)
    || get(cmd, "UEXp", &uexp)
    || get(cmd, "UTRa", &utra)
    || get(cmd, "DELta", &delta)
    || MODEL_MOS123::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
void MODEL_MOS2::parse_finish()
{
  MODEL_MOS123::parse_finish();
      
      cox = E_OX / tox;
      if (kp == NA) {
	kp = uo * cox;
	calc_kp = true;
      }
      if (nsub != NA) {
	if (phi == NA) {
	  phi = (2.*KoQ) * _tnom * log(nsub/NI);
	  if (phi < .1) {
	    untested();
	    error(bWARNING,
		  long_label() + ": calculated phi too small, using .1\n");
	    phi = .1;
	  }
	  calc_phi = true;
	}
	if (gamma == NA) {
	  gamma = sqrt(2. * E_SI * Q * nsub) / cox;
	  calc_gamma = true;
	}
	if (vto == NA) {
	  double phi_ms = (tpg == gtMETAL)
	    ? -.05 - (egap + polarity * phi) / 2.
	    : -polarity * (tpg * egap + phi) / 2.;
	  double vfb = phi_ms - Q * nss / cox;
	  vto = vfb + polarity * (phi + gamma * sqrt(phi));
	  calc_vto = true;
	}
      }
  if (cox == NA) {
    cox = E_OX/tox;
  }
  if (vto == NA) {
    vto = 0.0;
  }
  if (gamma == NA) {
    gamma = 0.0;
  }
  if (phi == NA) {
    phi = 0.6;
  }
  if (kp == NA) {
    kp = 2e-5;
  }
  alpha = ((nsub != NA)
	? (2. * E_SI) / (Q * nsub)
	: 0.);
  xd = sqrt(alpha);
  xwb = ((nsub != NA)
	? xd * sqrt(pb)
	: .25e-6);
  vbp = ucrit * E_SI / cox;
  cfsox = Q * nfs / cox;
}
/*--------------------------------------------------------------------------*/
SDP_CARD* MODEL_MOS2::new_sdp(const COMMON_COMPONENT* c)const
{
  assert(c);
  {if (dynamic_cast<const COMMON_MOS*>(c)) {
    return new SDP_MOS2(c);
  }else{
    return MODEL_MOS123::new_sdp(c);
  }}
}
/*--------------------------------------------------------------------------*/
void MODEL_MOS2::print_front(OMSTREAM& o)const
{
  MODEL_MOS123::print_front(o);
}
/*--------------------------------------------------------------------------*/
void MODEL_MOS2::print_params(OMSTREAM& o)const
{
  o << "level=2";
  MODEL_MOS123::print_params(o);
  if (mos_level != LEVEL)
    o << "  diodelevel=" << mos_level;
  if (!calc_kp)
    o << "  kp=" << kp;
  o << "  nfs=" << nfs/(ICM2M2);
  if (vmax != NA)
    o << "  vmax=" << vmax;
  if (neff != 1.0 || lambda == NA)
    o << "  neff=" << neff;
  if (ucrit != 1e4*ICM2M || uexp != NA)
    o << "  ucrit=" << ucrit/(ICM2M);
  if (uexp != NA)
    o << "  uexp=" << uexp;
  if (false)
    o << "  utra=" << utra;
  o << "  delta=" << delta;
}
/*--------------------------------------------------------------------------*/
void MODEL_MOS2::print_calculated(OMSTREAM& o)const
{
  MODEL_MOS123::print_calculated(o);
  if (calc_kp)
    o << "* kp=" << kp;
}
/*--------------------------------------------------------------------------*/
bool MODEL_MOS2::is_valid(const COMMON_COMPONENT* cc)const
{
  return MODEL_MOS123::is_valid(cc);
}
/*--------------------------------------------------------------------------*/
void MODEL_MOS2::tr_eval(COMPONENT* brh)const
{
  DEV_MOS* d = prechecked_cast<DEV_MOS*>(brh);
  assert(d);
  const COMMON_MOS* c = prechecked_cast<const COMMON_MOS*>(d->common());
  assert(c);
  const SDP_MOS2* s = prechecked_cast<const SDP_MOS2*>(c->sdp());
  assert(s);
  const MODEL_MOS2* m = this;
  const TDP_MOS2 T(d);
  const TDP_MOS2* t = &T;

    #define short_channel	(m->xj != NOT_INPUT  &&  m->xj > 0.)
    #define do_subthreshold	(m->nfs != 0.)
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    trace1(d->long_label().c_str(), d->evaliter());
    trace3("", d->vds, d->vgs, d->vbs);
    assert(m->_tnom > 0);
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    d->reverse_if_needed();
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double v_phi_s = t->phi - d->vbs;
    double sarg, dsarg_dvbs, d2sdb2, sarg3;
    {
      {if (d->vbs <= 0.) {
	sarg = sqrt(v_phi_s);
	dsarg_dvbs = -.5 / sarg;
	d2sdb2 = .5 * dsarg_dvbs / v_phi_s;
	d->sbfwd = false;
	trace3("sb-ok", sarg, v_phi_s, dsarg_dvbs);
      }else{
	{if (OPT::mosflags & 01000) {
	  sarg = t->sqrt_phi / (1. + .5 * d->vbs / t->phi);
	  dsarg_dvbs = -.5 * sarg * sarg / t->phi_sqrt_phi;
	  d2sdb2 = -dsarg_dvbs * sarg / t->phi_sqrt_phi;
	  untested();
	  trace3("***sb-reversed(01000)***", sarg, v_phi_s, dsarg_dvbs);
	}else{
	  sarg = t->sqrt_phi 
	    / (1. + .5 * d->vbs / t->phi 
	       + .375 * d->vbs * d->vbs / (t->phi * t->phi));
	  dsarg_dvbs = (-.5 * sarg * sarg / t->phi_sqrt_phi) 
	    * (1. + 1.5 * d->vbs / t->phi);
	  d2sdb2 = (-dsarg_dvbs * sarg / t->phi_sqrt_phi)
	    - (.75 * sarg / (t->phi_sqrt_phi * t->phi)) 
	    * (2. * d->vbs * dsarg_dvbs + sarg);
	  untested();
	  trace3("***sb-reversed(00000)***", sarg, v_phi_s, dsarg_dvbs);
	}}
	d->sbfwd = true;
      }}
      sarg3 = sarg*sarg*sarg;
      assert(sarg > 0.);
      assert(dsarg_dvbs < 0.);
      assert(up_order(-1/t->phi, d2sdb2, 1/t->phi));
      trace2("", d2sdb2, sarg3);
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double barg, dbarg_dvbs, d2bdb2;
    {
      double vbd = d->vbs - d->vds;
      double v_phi_d = t->phi - vbd;
      {if (vbd <= 0.) {
	barg = sqrt(v_phi_d);
	dbarg_dvbs = -.5 / barg;
	d2bdb2 = .5 * dbarg_dvbs / v_phi_d;
	d->dbfwd = false;
	trace4("db-ok", barg, v_phi_d, dbarg_dvbs, d2bdb2);
      }else{
	{if (OPT::mosflags & 01000) {
	  barg = t->sqrt_phi / (1. + .5 * vbd / t->phi);
	  dbarg_dvbs = -.5 * barg * barg / t->phi_sqrt_phi;
	  d2bdb2 = -dbarg_dvbs * barg / t->phi_sqrt_phi;
	  untested();
	  trace4("***db-reversed(00000)***",barg, v_phi_d, dbarg_dvbs, d2bdb2);
	}else{
	  barg = t->sqrt_phi 
	    / (1. + .5 * vbd / t->phi 
	       + .375 * vbd * vbd / (t->phi * t->phi));
	  dbarg_dvbs = (-.5 * barg * barg / t->phi_sqrt_phi) 
	    * (1. + 1.5 * vbd / t->phi);
	  d2bdb2 = (-dbarg_dvbs * barg / t->phi_sqrt_phi)
	    - (.75 * barg / (t->phi_sqrt_phi * t->phi)) 
	    * (2. * vbd * dbarg_dvbs + barg);
	  trace4("***db-reversed(00000)***",barg, v_phi_d, dbarg_dvbs, d2bdb2);
	}}
	d->dbfwd = true;
      }}
      assert(barg > 0.);
      assert(dbarg_dvbs < 0.);
      assert(up_order(-1/t->phi, d2bdb2, 1/t->phi));
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double gamma_s, dgamma_s_dvds, dgamma_s_dvbs, dgddb2;
    {
      {if (short_channel) {
	double argxd = 1. + 2. * barg * m->xd / m->xj;
	assert(argxd > 0);
	double argd = sqrt(argxd);
	trace2("", argxd, argd);
	
	double alpha_d = s->relxj * (argd - 1.);
	double dalpha_d_dvds = m->xd / (4. * s->l_eff * argd * barg);
	double dalpha_d_dvbs = -dalpha_d_dvds;
	trace3("", alpha_d, dalpha_d_dvds, dalpha_d_dvbs);
	
	double argxs = 1. + 2. * sarg * m->xd / m->xj;
	assert(argxs > 0);
	double args = sqrt(argxs);
	trace2("", argxs, args);
	
	double alpha_s = s->relxj * (args - 1.);
	double dalpha_s_dvbs = -m->xd / (4. * s->l_eff * args * sarg);
	trace2("", alpha_s, dalpha_s_dvbs);
	
	gamma_s = m->gamma * (1. - alpha_s - alpha_d);
	dgamma_s_dvds = -m->gamma *  dalpha_d_dvds;
	dgamma_s_dvbs = -m->gamma * (dalpha_d_dvbs + dalpha_s_dvbs);
	
	double dasdb2=-m->xd*(d2sdb2+dsarg_dvbs*dsarg_dvbs*m->xd/(m->xj*argxs))
	  / (s->l_eff*args);
	double daddb2=-m->xd*(d2bdb2+dbarg_dvbs*dbarg_dvbs*m->xd/(m->xj*argxd))
	  / (s->l_eff*argd);
	dgddb2 = -.5 * m->gamma * (dasdb2 + daddb2);
	
	if (gamma_s <= 0. && m->gamma > 0. /* && !IO::suppresserrors */) {
	  untested();
	  error(bTRACE, d->long_label() + ": gamma is negative\n");
	  error(bTRACE, "+   gamma_s=%g, alpha_s=%g, alpha_d=%g\n",
		gamma_s,    alpha_s,    alpha_d);
	}
	trace4("no short chan", gamma_s, dgamma_s_dvds, dgamma_s_dvds, dgddb2);
      }else{
	gamma_s = m->gamma;
	dgamma_s_dvds = dgamma_s_dvbs = 0.;
	dgddb2 = 0.;
	trace4("short channel", gamma_s, dgamma_s_dvds, dgamma_s_dvds, dgddb2);
      }}
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    /* von, subthreshold, cutoff, vgst */
    double vc, vc_eta, dvon_dvbs;
    double xn, vtxn, dxn_dvbs;	/* subthreshold only */
    {
      double vbin = t->vbi + s->eta_1 * v_phi_s;
      d->von = vbin + gamma_s * sarg;
      dvon_dvbs = -s->eta_1 + dgamma_s_dvbs * sarg + gamma_s * dsarg_dvbs;
      trace3("guess", vbin, d->von, dvon_dvbs);
      
      {if (do_subthreshold) {
	double cdonco = -(gamma_s*dsarg_dvbs + dgamma_s_dvbs*sarg) + s->eta_1;
	xn = 1. + m->cfsox + cdonco;
	vtxn = t->vt * xn;
	dxn_dvbs = 2. * dgamma_s_dvbs * dsarg_dvbs
	  + gamma_s * d2sdb2 + dgddb2 * sarg;
	trace3("do_sub", xn, vtxn, dxn_dvbs);
	
	d->von += vtxn;
	dvon_dvbs += t->vt * dxn_dvbs;
	d->vgst = d->vgs - d->von;
	trace3("", d->von, dvon_dvbs, d->vgst);
	
	d->subthreshold = (d->vgs < d->von);
	d->cutoff = false;
      }else{
	xn = vtxn = dxn_dvbs = 0.;
	d->vgst = d->vgs - d->von;
	trace3("no_sub", xn, vtxn, dxn_dvbs);
	trace3("", d->von, dvon_dvbs, d->vgst);
	
	d->subthreshold = false;
	d->cutoff = (d->vgs < d->von);
	if (d->cutoff) {
	  trace0("***** cut off *****");
	  d->ids = 0.;
	  d->gmf = d->gmr = 0.;
	  d->gds = 0.;
	  d->gmbf = d->gmbr = 0.;
	  return;
	}
      }}
      double vgsx = (d->subthreshold) ? d->von : d->vgs;
      vc = vgsx - vbin;
      vc_eta = vc / s->eta;
      trace3("", vgsx, vc, vc_eta);
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double ufact, duf_dvgs, duf_dvds, duf_dvbs, ueff;
    {
      {if (m->uexp != NOT_INPUT  &&  d->vgst > m->vbp) {
	ufact = pow(m->vbp/d->vgst, m->uexp);
	duf_dvgs = -ufact * m->uexp / d->vgst;
	duf_dvds = 0.;	/* wrong, but as per spice2 */
	duf_dvbs = dvon_dvbs * ufact * m->uexp / d->vgst;
	trace4("calc ufact", ufact, duf_dvgs, duf_dvds, duf_dvbs);
      }else{
	ufact = 1.;
	duf_dvgs = duf_dvds = duf_dvbs = 0.;
	trace4("def ufact", ufact, duf_dvgs, duf_dvds, duf_dvbs);
      }}
      ueff = t->uo * ufact;	/* ???? */
      trace2("", ufact, ueff);
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    /* vdsat  according to Baum's Theory of scattering velocity saturation  */
    int use_vmax = m->vmax != NOT_INPUT;
    {if (use_vmax) {
      double gammad = gamma_s / s->eta;
      double v1 = vc_eta + v_phi_s;
      double v2 = v_phi_s;
      double xv = m->vmax * s->l_eff / ueff;
      double a1 = gammad * (4./3.);
      double b1 = -2. * (v1+xv);
      double c1 = -2. * gammad * xv;			/* end of scope */
      double d1 = 2.*v1*(v2+xv) - v2*v2 - (4./3.)*gammad*sarg3;
      double a = -b1;					/* xv, v1, v2, sarg3 */
      double b = a1 * c1 - 4. * d1;
      double c = -d1 * (a1*a1 - 4.*b1) - c1*c1;
      double r = -a*a / 3. + b;
      double r3 = r*r*r;				/* r */
      double s = 2. * a*a*a / 27. - a*b / 3. + c;	/* b, c */
      double s2 = s*s;
      double p = s2 / 4. + r3 / 27.;			/* r */
      double y3;
      {if (p < 0.) {					/* p */
	double ro = pow((-r3 / 27), (1./6.));		/* s2, r3 */
	double fi = atan(-2. * sqrt(-p) / s);
	y3 = 2. * ro * cos(fi/3.) - a / 3.;
      }else{
	double p2 = sqrt(p);
	double p3 = pow((fabs(-s/2.+p2)), (1./3.));
	double p4 = pow((fabs(-s/2.-p2)), (1./3.));	/* s */
	y3 = p3 + p4 - a / 3.;				/* a */
	untested();
      }}
      
      double x4[8];
      int iknt = 0;
      {if (a1*a1 / 4. - b1 + y3  < 0.  &&  y3*y3 / 4. - d1  < 0.) {
	untested();
	error(bWARNING,
	      "%s: internal error: a3,b4, a1=%g, b1=%g, y3=%g, d1=%g\n",
	      d->long_label().c_str(),    a1,    b1,    y3,    d1);
      }else{
	double a3 = sqrt(a1*a1 / 4. - b1 + y3);
	double b3 = sqrt(y3*y3 / 4. - d1);
	for (int i = 0;   i < 4;   i++) {
	  static const double sig1[4] = {1., -1., 1., -1.};
	  static const double sig2[4] = {1., 1., -1., -1.};
	  double a4 = a1 / 2. + sig1[i] * a3;
	  double b4 = y3 / 2. + sig2[i] * b3;		/* y3 */
	  double delta4 = a4*a4 / 4. - b4;
	  if (delta4 >= 0.) {
	    double sd4 = sqrt(delta4);
	    x4[iknt++] = - a4 / 2. + sd4;
	    x4[iknt++] = - a4 / 2. - sd4;		/* i */
	  }
	}
      }}
      
      double xvalid = 0.;
      int root_count = 0;
      for (int j = 0;   j < iknt;   j++) {			/* iknt */
	if (x4[j] > 0.) {
	  double poly4 = x4[j]*x4[j]*x4[j]*x4[j]/* ~= 0, used as check	*/
	    + a1 * x4[j]*x4[j]*x4[j]		/* roundoff error not	*/
	    + b1 * x4[j]*x4[j]			/* propagated, so ok	*/
	    + c1 * x4[j]
	    + d1;				/* a1, b1, c1, d1 */
	  if (fabs(poly4) <= 1e-6) {
	    root_count++;
	    if (root_count <= 1) {		/* xvalid = min(x4[j]) */
	      xvalid=x4[j];
	    }
	    {if (x4[j] <= xvalid) {
	      xvalid=x4[j];				/* x4[], j */
	    }else{
	      untested();
	    }}
	  }
	}
      }
      {if (root_count <= 0) {				/* root_count */
	if (OPT::picky <= bTRACE || !IO::suppresserrors) {
	  untested();
	  error(bWARNING, d->long_label() + ": Baum's theory rejected\n");
	}
	use_vmax = false;
	d->vdsat = 0.;
	trace1("use_vmax rejected", d->vdsat);
      }else{
	d->vdsat = xvalid*xvalid - v_phi_s;
	trace1("use_vmax", d->vdsat);
      }}
    }else{
      d->vdsat = 0.;
      trace1("!use_vmax", d->vdsat);
    }}
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    /* vdsat     according to Grove-Frohman equation  */
    double dvdsat_dvgs = NOT_VALID;
    double dvdsat_dvbs = NOT_VALID;
    {if (!use_vmax) {
      {if (gamma_s > 0.) {
	double argv = vc_eta + v_phi_s;
	{if (argv > 0.) {
	  double gammad = gamma_s / s->eta;
	  double gammd2 = gammad * gammad;
	  double arg1 = sqrt(1. + 4. * argv / gammd2);
	  d->vdsat = vc_eta  +  gammd2 * (1.-arg1) / 2.;
	  dvdsat_dvgs = (1. - 1./arg1) / s->eta;
	  dvdsat_dvbs = (gammad * (1.-arg1) + 2.*argv / (gammad*arg1))
	    / s->eta * dgamma_s_dvbs
	    + 1./arg1 + s->eta_1 * dvdsat_dvgs;
	  trace3("!use_vmax,gamma>0,argv>0",d->vdsat,dvdsat_dvgs,dvdsat_dvbs);
	}else{
	  d->vdsat = 0.;
	  dvdsat_dvgs = dvdsat_dvbs = 0.;
	  untested();
	  if (!IO::suppresserrors) {
	    untested();
	    error(bWARNING, d->long_label() + ": argv is negative\n");
	  }
	  untested();
	  trace2("argv<0", argv, vc);
	  trace3("!use_vmax,gamma>0,argv<=0",d->vdsat,dvdsat_dvgs,dvdsat_dvbs);
	}}
      }else{
	d->vdsat = vc_eta;
	dvdsat_dvgs = 1.;
	dvdsat_dvbs = 0.;
	trace3("!use_vmax, gamma<=0", d->vdsat, dvdsat_dvgs, dvdsat_dvbs);
      }}
    }else{
      /* dvdsat_dvgs, dvdsat_dvbs   deferred */
      trace3("use_vmax", d->vdsat, dvdsat_dvgs, dvdsat_dvbs);
    }}
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    if (d->vdsat < 0.) {
      error(bWARNING, "%s: calculated vdsat (%g) < 0.  using vdsat = 0.\n",
	    d->long_label().c_str(), d->vdsat);
      d->vdsat = 0.;
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double bsarg, dbsarg_dvbs;
    {
      double vbdsat = d->vbs - d->vdsat;
      {if (vbdsat <= 0.) {
	double v_phi_ds = t->phi - vbdsat;
	bsarg = sqrt(v_phi_ds);
	dbsarg_dvbs = -.5 / bsarg;
	trace3("vbdsat <= 0", vbdsat, bsarg, dbsarg_dvbs);
      }else{
	bsarg = t->sqrt_phi / (1. + .5 * vbdsat / t->phi);
	dbsarg_dvbs = -.5 * bsarg * bsarg / t->phi_sqrt_phi;
	untested();
	trace3("vbdsat > 0", vbdsat, bsarg, dbsarg_dvbs);
      }}
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    /* local   dvdsat_dvgs, dvdsat_dvbs   maybe */
    {
      {if (use_vmax) {
	double bodys = bsarg*bsarg*bsarg - sarg3;
	double gdbdvs =
	  2. * gamma_s * (bsarg*bsarg*dbsarg_dvbs - sarg*sarg*dsarg_dvbs);
	double argv = vc_eta - d->vdsat;
	double vqchan = argv - gamma_s * bsarg;
	double dqdsat = -1. + gamma_s * dbsarg_dvbs;
	double vl = m->vmax * s->l_eff;
	double dfunds = vl * dqdsat - ueff * vqchan;
	double dfundg = (vl - ueff * d->vdsat) / s->eta;
	double dfundb = -vl * (1. + dqdsat - s->eta_1 / s->eta)
	  + ueff * (gdbdvs - dgamma_s_dvbs * bodys / 1.5) / s->eta;
	dvdsat_dvgs = -dfundg / dfunds;
	dvdsat_dvbs = -dfundb / dfunds;
	trace2("use_vmax", dvdsat_dvgs, dvdsat_dvbs);
      }else{
	/* dvdsat_dvgs, dvdsat_dvbs   already set */
	trace2("!use_vmax", dvdsat_dvgs, dvdsat_dvbs);
      }}
      assert(dvdsat_dvgs != NOT_VALID);
      assert(dvdsat_dvbs != NOT_VALID);
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    double  dl_dvgs, dl_dvds, dl_dvbs, clfact;
    {
      {if (d->vds > 0.) {
	{if (m->lambda == NOT_INPUT) {
	  double dldsat;
	  {if (use_vmax) {
	    double xdv = m->xd / sqrt(m->neff);
	    double xlv = m->vmax * xdv / (2. * ueff);
	    double argv = d->vds - d->vdsat;
	    if (argv < 0.) {
	      argv = 0.;
	    }
	    double xls = sqrt(xlv*xlv + argv);
	    double dl = (xls-xlv) * xdv;
	    /* lambda = dl / (s->l_eff * d->vds); */
	    clfact = (1. - dl / s->l_eff);
	    dldsat = xdv / (2. * xls * s->l_eff);
	  }else{
	    double argv = (d->vds - d->vdsat) / 4.;
	    double sargv = sqrt(1. + argv*argv);
	    {if (argv + sargv >= 0.) {
	      double dl = m->xd * sqrt(argv + sargv);
	      /* lambda = dl / (s->l_eff * d->vds); */
	      clfact = (1. - dl / s->l_eff);
	      /* dldsat = lambda * d->vds / (8. * sargv); */
	      dldsat = dl / (s->l_eff * 8. * sargv);
	    }else{
	      /* lambda = 0.; */
	      clfact = 1.;
	      dldsat = 0.;
	      untested();
	      error(bWARNING, "%s: internal error: vds(%g) < vdsat(%g)\n",
		    d->long_label().c_str(), d->vds,   d->vdsat);
	    }}
	  }}
	  dl_dvgs =  dvdsat_dvgs * dldsat;
	  dl_dvds =              - dldsat;
	  dl_dvbs =  dvdsat_dvbs * dldsat;
	}else{
	  /* lambda = m->lambda; */
	  clfact = (1. - m->lambda * d->vds);
	  dl_dvgs = dl_dvbs = 0.;
	  dl_dvds = -m->lambda;
	}}
	
	/* clfact = (1. - lambda * d->vds); */
	if (clfact < m->xwb/s->l_eff) {
	  double leff = m->xwb / (2. - (clfact * s->l_eff / m->xwb));
	  double dfact = (leff * leff) / (m->xwb * m->xwb);
	  clfact = leff / s->l_eff;
	  dl_dvgs *= dfact;
	  dl_dvds *= dfact;
	  dl_dvbs *= dfact;
	}
      }else{  /* vds <= 0. */
	/* lambda = 0.; */
	clfact = 1.;
	dl_dvgs = dl_dvds = dl_dvbs = 0.;
	trace1("*** vds < 0 ***", d->vds);
      }}
      trace4("", dl_dvgs, dl_dvds, dl_dvbs, clfact);
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
    /* ids, gmf, gds, gmbf */
    {
      d->saturated = (d->vds > d->vdsat);
      double vdsx =  (d->saturated) ? d->vdsat : d->vds;
      double bargx = (d->saturated) ? bsarg : barg;
      double body = bargx*bargx*bargx - sarg3;
      double expg = (d->subthreshold) ? exp(d->vgst / vtxn) : 1.;
      trace4("", vdsx, bargx, body, expg);
      
      double beta = t->beta * ufact / clfact;
      double ids_on = 
	beta * ((vc - s->eta_2 * vdsx) * vdsx  - (2./3.) * gamma_s * body);
      double didvds = beta * (vc  -  s->eta * vdsx  -  gamma_s * bargx);
      fixzero(&didvds, ids_on);
      trace4("", beta, ids_on, didvds, d->saturated);
      
      d->ids = ids_on * expg;
      
      d->gmf = beta * vdsx;
      d->gmf += ids_on * (duf_dvgs/ufact - dl_dvgs/clfact);
      if (d->saturated) {
	d->gmf += didvds * dvdsat_dvgs;
      }
      if (d->subthreshold) {
	d->gmf = ids_on / vtxn;
	if (d->saturated) {
	  d->gmf += didvds * dvdsat_dvgs;
	}
	d->gmf *= expg;
      }
      
      d->gds = (d->saturated) ? 0.: didvds;
      d->gds += ids_on * (duf_dvds/ufact - dl_dvds/clfact);
      if (short_channel) {
	d->gds -= beta * (2./3.) * body * dgamma_s_dvds;
      }
      if (d->subthreshold) {
	double dxndvd = dgamma_s_dvds * dsarg_dvbs;
	double dodvds = dgamma_s_dvds * sarg + t->vt * dxndvd;
	double gmw = d->ids * d->vgst / (vtxn * xn);
	d->gds *= expg;
	d->gds -= d->gmf * dodvds + gmw * dxndvd;
      }
      
      d->gmbf = beta * (s->eta_1 * vdsx - gamma_s * (sarg - bargx));
      d->gmbf += ids_on * (duf_dvbs/ufact - dl_dvbs/clfact);
      if (short_channel) {
	d->gmbf -= beta * (2./3.) * body * dgamma_s_dvbs;
      }
      if (d->saturated) {
	d->gmbf += didvds * dvdsat_dvbs;
      }
      if (d->subthreshold) {
	double gmw = d->ids * d->vgst / (vtxn * xn);
	d->gmbf += beta * dvon_dvbs * vdsx;
	d->gmbf *= expg;
	d->gmbf -= d->gmf * dvon_dvbs + gmw * dxn_dvbs;
      }
      trace4("", d->ids, d->gmf, d->gds, d->gmbf);
    }
    {if (d->reversed){
      d->ids *= -1;
      d->gmr = d->gmf;
      d->gmbr = d->gmbf;
      d->gmf = d->gmbf = 0;
    }else{
      d->gmr = d->gmbr = 0.;
    }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
