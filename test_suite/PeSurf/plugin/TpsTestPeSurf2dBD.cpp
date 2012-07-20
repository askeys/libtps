/*
LibTPS -- A Transition Path Sampling Library
(LibTPS) Open Source Software License Copyright 2012 The Regents of the 
University of Michigan All rights reserved.

LibTPS may contain modifications ("Contributions") provided, and to which
copyright is held, by various Contributors who have granted The Regents of the
University of Michigan the right to modify and/or distribute such Contributions.

You may redistribute, use, and create derivate works of LibTPS, in source
and binary forms, provided you abide by the following conditions:

* Redistributions of source code must retain the above copyright notice, this
list of conditions, and the following disclaimer both in the code and
prominently in any materials provided with the distribution.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions, and the following disclaimer in the documentation and/or
other materials provided with the distribution.

* Apart from the above required attributions, neither the name of the copyright
holder nor the names of LibTPS's contributors may be used to endorse or
promote products derived from this software without specific prior written
permission.

Disclaimer

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND/OR ANY
WARRANTIES THAT THIS SOFTWARE IS FREE OF INFRINGEMENT ARE DISCLAIMED.

IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 *  TpsTestPeSurf2dBD.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestPeSurf2dBD.h"
#include <iostream>

TpsTestPeSurf2dBD::TpsTestPeSurf2dBD(TpsRNG& rng)
 : _rng(rng)
{
	//these member variables are specific to the BD class
	_beta = 1.0;
	_dt = 0.25;
	_gamma = 2.5;
	_current_timeslice.x[0] = 2;
	_current_timeslice.x[1] = 2;

	initConstants();
}

void TpsTestPeSurf2dBD::run(int nsteps)
{
	for (int i=0; i<nsteps; i++) {

		double &x = _current_timeslice.x[0];
		double &y = _current_timeslice.x[1];
		double &vx = _current_timeslice.v[0];
		double &vy = _current_timeslice.v[1];
		double &fx = _current_timeslice.f[0];
		double &fy = _current_timeslice.f[1];
		
		double dx = 0.0, dy = 0.0, dvx = 0.0, dvy = 0.0;

		if (_gamma > 0) {
			bivariateRand(dx, dvx);
			bivariateRand(dx, dvy);
		}
		
		//velocity verlet initial step
		x  += _dt*(_c1*vx + _c2*_dt*fx) +dx;
		y  += _dt*(_c1*vy + _c2*_dt*fy) +dy; 
		vx = _c0 * vx + _c1min2 *_dt * fx;
		vy = _c0 * vy + _c1min2 *_dt * fy;

		//force calculation
		updateForce();
		
		
		//velocity verlet final step
		vx += _c2 * _dt * fx +dvx;
		vy += _c2 * _dt * fy +dvy;

	}
	callbackOnRunCommand();
}
		
void TpsTestPeSurf2dBD::setTemperature(double temp)
{
	_beta = 1.0 / temp;
	initConstants();
}

//everything defined below this point is not required...

void TpsTestPeSurf2dBD::initConstants()
{
	double dt = _dt;
	double fdt = _dt * _gamma;
	double ex  = exp (-fdt);
	double temp = 1./_beta;

	_sig_r = dt * dt * temp * (2. - (3. - 4.*ex + ex*ex)/fdt ) / fdt;
	_sig_v = temp * (1.0 - ex * ex);

	_sig_r = sqrt(_sig_r);
	_sig_v = sqrt(_sig_v);

	_c_rv  = dt * temp*(1.0 - ex) * (1.0 - ex) / fdt / _sig_r / _sig_v;
	_sigma = 0.5 / (1.0 - _c_rv * _c_rv);
	_norm  = 1.0 / sqrt (2.0 * M_PI * _sig_r * _sig_v *
				sqrt (1.0 - _c_rv * _c_rv));

	double s11 = _sig_r * _sig_r;
	double s22 = _sig_v * _sig_v;
	double s12 = _c_rv * _sig_r * _sig_v;
	double S   = s11 * s22 - s12 * s12;
	_s12os11 = s12 / s11; 
	_sqrts11   = sqrt(s11);
	_sqrtSos11 = sqrt(S/s11);

	_c0 =ex;
	_c1 =(1-_c0)/fdt;
	_c2 =(1-_c1)/fdt;
	if (_gamma==0) {
		_c0 = 1;
		_c1 = 1;
		_c2 = 0.5;
	}
	_c1min2 = _c1-_c2;		
}

void TpsTestPeSurf2dBD::setTimestep(double dt)
{
	_dt = dt;
	initConstants();
}

void TpsTestPeSurf2dBD::setDragCoefficient(double gamma)
{
	_gamma = gamma;
	initConstants();
}

void TpsTestPeSurf2dBD::bivariateRand(double &r, double& v)
{
	double fac,rsq,v1,v2;

	do {
		v1 =2.*_rng.randDouble() -1.;
		v2 =2.*_rng.randDouble() -1.;
		rsq = v1*v1 + v2*v2;
	} while ((rsq >= 1)|| (rsq ==0));
	fac = sqrt(-2.*log(rsq)/rsq);

	r = v1*fac * _sqrts11;
	v = v2*fac * _sqrtSos11 + r*_s12os11;
}
