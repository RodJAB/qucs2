/*
 * diode.cpp - diode class implementation
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: diode.cpp,v 1.30 2006/01/30 07:45:34 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "net.h"
#include "matrix.h"
#include "component_id.h"
#include "constants.h"
#include "device.h"
#include "diode.h"

#define NODE_C 0 /* cathode node */
#define NODE_A 1 /* anode node   */

diode::diode () : circuit (2) {
  rs = NULL;
  type = CIR_DIODE;
}

void diode::calcSP (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");
  complex y = 2 * z0 * rect (gd, Cd * 2.0 * M_PI * frequency);
  setS (NODE_C, NODE_C, 1.0 / (1.0 + y));
  setS (NODE_A, NODE_A, 1.0 / (1.0 + y));
  setS (NODE_C, NODE_A, y / (1.0 + y));
  setS (NODE_A, NODE_C, y / (1.0 + y));
}

void diode::calcNoiseSP (nr_double_t frequency) {
#if MICHAEL /* shot noise only */
  nr_double_t Id = getOperatingPoint ("Id");
  nr_double_t Is = getPropertyDouble ("Is") + getPropertyDouble ("Isr");

  // adjust shot noise current if necessary
  if (Id < -Is) Id = -Is;

  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");

  complex y = rect (gd, Cd * 2.0 * M_PI * frequency);
  complex f = 2 * z0 * (Id + 2 * Is) / norm (2 * z0 * y + 1) * QoverkB / T0;
  setN (NODE_C, NODE_C, +f); setN (NODE_A, NODE_A, +f);
  setN (NODE_C, NODE_A, -f); setN (NODE_A, NODE_C, -f);
#else
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
#endif
}

matrix diode::calcMatrixCy (nr_double_t frequency) {
  // fetch computed operating points
  nr_double_t Id = getOperatingPoint ("Id");
  nr_double_t Is = getPropertyDouble ("Is") + getPropertyDouble ("Isr");

  // adjust shot noise current if necessary
  if (Id < -Is) Id = -Is;

  nr_double_t Kf  = getPropertyDouble ("Kf");
  nr_double_t Af  = getPropertyDouble ("Af");
  nr_double_t Ffe = getPropertyDouble ("Ffe");

  // build noise current correlation matrix
  matrix cy = matrix (2);
  nr_double_t i = 2 * (Id + 2 * Is) * QoverkB / T0 +    // shot noise
    Kf * pow (fabs (Id), Af) / pow (frequency, Ffe) / kB / T0; // flicker noise
  cy.set (NODE_C, NODE_C, +i); cy.set (NODE_A, NODE_A, +i);
  cy.set (NODE_A, NODE_C, -i); cy.set (NODE_C, NODE_A, -i);
  return cy;
}

void diode::initModel (void) {
  // fetch necessary device properties
  nr_double_t T  = getPropertyDouble ("Temp");
  nr_double_t Tn = getPropertyDouble ("Tnom");
  nr_double_t A  = getPropertyDouble ("Area");

  // compute Is temperature and area dependency
  nr_double_t Is  = getPropertyDouble ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Xti = getPropertyDouble ("Xti");
  nr_double_t Eg  = getPropertyDouble ("Eg");
  nr_double_t T1, T2;
  T2 = kelvin (T);
  T1 = kelvin (Tn);
  Is = pnCurrent_T (T1, T2, Is, Eg, N, Xti);
  setScaledProperty ("Is", Is * A);

  // compute Isr temperature and area dependency
  nr_double_t Isr = getPropertyDouble ("Isr");
  nr_double_t Nr  = getPropertyDouble ("Nr");
  Isr = pnCurrent_T (T1, T2, Isr, Eg, Nr, Xti);
  setScaledProperty ("Isr", Isr * A);

  // compute Vj temperature dependency
  nr_double_t Vj = getPropertyDouble ("Vj");
  nr_double_t VjT;
  VjT = pnPotential_T (T1,T2, Vj);
  setScaledProperty ("Vj", VjT);

  // compute Cj0 temperature and area dependency
  nr_double_t Cj0 = getPropertyDouble ("Cj0");
  nr_double_t M   = getPropertyDouble ("M");
  Cj0 = pnCapacitance_T (T1, T2, M, VjT / Vj, Cj0);
  setScaledProperty ("Cj0", Cj0 * A);

  // compute Bv temperature dependency
  nr_double_t Bv  = getPropertyDouble ("Bv");
  nr_double_t Tbv = getPropertyDouble ("Tbv");
  nr_double_t DT  = T2 - T1;
  Bv = Bv - Tbv * DT;
  setScaledProperty ("Bv", Bv);

  // compute Tt temperature dependency
  nr_double_t Tt   = getPropertyDouble ("Tt");
  nr_double_t Ttt1 = getPropertyDouble ("Ttt1");
  nr_double_t Ttt2 = getPropertyDouble ("Ttt2");
  Tt = Tt * (1 + Ttt1 * DT + Ttt2 * DT * DT);
  setScaledProperty ("Tt", Tt);

  // compute M temperature dependency
  nr_double_t Tm1 = getPropertyDouble ("Tm1");
  nr_double_t Tm2 = getPropertyDouble ("Tm2");
  M = M * (1 + Tm1 * DT + Tm2 * DT * DT);
  setScaledProperty ("M", M);

  // compute Rs temperature and area dependency
  nr_double_t Rs  = getPropertyDouble ("Rs");
  nr_double_t Trs = getPropertyDouble ("Trs");
  Rs = Rs * (1 + Trs * DT);
  setScaledProperty ("Rs", Rs / A);
}

void diode::initDC (void) {

  // allocate MNA matrices
  allocMatrixMNA ();

  // initialize scalability
  initModel ();

  // initialize starting values
  Uprev = real (getV (NODE_A) - getV (NODE_C));

  // get device temperature
  nr_double_t T = getPropertyDouble ("Temp");

  // possibly insert series resistance
  nr_double_t Rs = getScaledProperty ("Rs");
  if (Rs != 0.0) {
    // create additional circuit if necessary and reassign nodes
    rs = splitResistor (this, rs, "Rs", "anode", NODE_A);
    rs->setProperty ("Temp", T);
    rs->setProperty ("R", Rs);
    rs->setProperty ("Controlled", getName ());
    rs->initDC ();
  }
  // no series resistance
  else {
    disableResistor (this, rs, NODE_A);
  }

  // calculate actual breakdown voltage
  Bv = getScaledProperty ("Bv");
  if (Bv != 0) {
    nr_double_t Ibv, Is, tol, Ut, Xbv, Xibv;
    Ibv = getPropertyDouble ("Ibv");
    Is = getScaledProperty ("Is");
    Ut = kelvin (T) * kBoverQ;
    // adjust very small breakdown currents
    if (Ibv < Is * Bv / Ut) {
      Ibv = Is * Bv / Ut;
      Xbv = Bv;
      logprint (LOG_ERROR, "WARNING: Increased breakdown current to %g to "
		"match the saturation current %g\n", Ibv, Is);
    }
    // fit reverse and forward regions
    else {
      int good = 0;
      tol = 1e-3 * Ibv;
      Xbv = Bv - Ut * log (1 + Ibv / Is);
      for (int i = 0; i < 25 ; i++) {
	Xbv = Bv - Ut * log (Ibv / Is + 1 - Xbv / Ut);
	Xibv = Is * (exp ((Bv - Xbv) / Ut) - 1 + Xbv / Ut);
	if (fabs (Xibv - Ibv) < tol) {
	  Bv = Xbv;
	  good = 1;
	  break;
	}
      }
      if (!good) {
	logprint (LOG_ERROR, "WARNING: Unable to fit reverse and forward "
		  "diode regions using Bv=%g and Ibv=%g\n", Bv, Ibv);
      }
    }
  }
}

void diode::calcDC (void) {
  nr_double_t Is  = getScaledProperty ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Isr = getScaledProperty ("Isr");
  nr_double_t Nr  = getPropertyDouble ("Nr");
  nr_double_t T   = getPropertyDouble ("Temp");

  nr_double_t Ud, Ut, Ieq, Ucrit, gtiny;

  T = kelvin (T);
  Ut = T * kBoverQ;
  Ud = real (getV (NODE_A) - getV (NODE_C));

  // critical voltage necessary for bad start values
  Ucrit = pnCriticalVoltage (Is, N * Ut);
  if (Bv != 0 && Ud < MIN (0, -Bv + 10 * N * Ut)) {
    nr_double_t V = -(Ud + Bv);
    V = pnVoltage (V, -(Uprev + Bv), Ut * N, Ucrit);
    Ud = -(V + Bv);
  }
  else {
    Ud = pnVoltage (Ud, Uprev, Ut * N, Ucrit);
  }
  Uprev = Ud;

  // tiny derivative for little junction voltage
  gtiny = (Ud < - 10 * Ut * N && Bv != 0) ? (Is + Isr) : 0;

  if (Ud >= -3 * N * Ut) { 
    gd = pnConductance (Ud, Is, Ut * N) +
      pnConductance (Ud, Isr, Ut * Nr) + gtiny;
    Id = pnCurrent (Ud, Is, Ut * N) +
      pnCurrent (Ud, Isr, Ut * Nr) + gtiny * Ud;
  }
  else if (Bv == 0 || Ud >= -Bv) {
    nr_double_t a = 3 * N * Ut / (Ud * M_E);
    a = cubic (a);
    Id = -Is * (1 + a) + gtiny * Ud;
    gd = +Is * 3 * a / Ud + gtiny;
  }
  else {
    nr_double_t a = exp (-(Bv + Ud) / N / Ut);
    Id = -Is * a + gtiny * Ud;
    gd = +Is * a / Ut / N + gtiny;
  }

  Ieq = Id - Ud * gd;
  setI (NODE_C, +Ieq);
  setI (NODE_A, -Ieq);

  setY (NODE_C, NODE_C, +gd); setY (NODE_A, NODE_A, +gd);
  setY (NODE_C, NODE_A, -gd); setY (NODE_A, NODE_C, -gd);
}

void diode::saveOperatingPoints (void) {
  nr_double_t Ud = real (getV (NODE_A) - getV (NODE_C));
  setOperatingPoint ("Vd", Ud);
}

void diode::calcOperatingPoints (void) {
  nr_double_t M   = getScaledProperty ("M");
  nr_double_t Cj0 = getScaledProperty ("Cj0");
  nr_double_t Vj  = getScaledProperty ("Vj");
  nr_double_t Fc  = getPropertyDouble ("Fc");
  nr_double_t Cp  = getPropertyDouble ("Cp");
  nr_double_t Tt  = getScaledProperty ("Tt");
  
  nr_double_t Ud, Cd;

  Ud = getOperatingPoint ("Vd");
  Cd = pnCapacitance (Ud, Cj0, Vj, M, Fc) + Tt * gd + Cp;
  Qd = pnCharge (Ud, Cj0, Vj, M, Fc) + Tt * Id + Cp * Ud;

  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Cd", Cd);
}

void diode::initAC (void) {
  allocMatrixMNA ();
  clearI ();
}

void diode::calcAC (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");
  complex y = rect (gd, Cd * 2.0 * M_PI * frequency);
  setY (NODE_C, NODE_C, +y); setY (NODE_A, NODE_A, +y);
  setY (NODE_C, NODE_A, -y); setY (NODE_A, NODE_C, -y);
}

void diode::calcNoiseAC (nr_double_t frequency) {
  setMatrixN (calcMatrixCy (frequency));
}

#define qState 0 // charge state
#define cState 1 // current state

void diode::initTR (void) {
  setStates (2);
  initDC ();
}

void diode::calcTR (nr_double_t) {
  calcDC ();
  saveOperatingPoints ();
  calcOperatingPoints ();

  nr_double_t Ud = getOperatingPoint ("Vd");
  nr_double_t Cd = getOperatingPoint ("Cd");

  transientCapacitance (qState, NODE_A, NODE_C, Cd, Ud, Qd);
}
