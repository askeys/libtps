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

#ifndef PESURF2DBDH
#define PESURF2DBDH

class TpsRNG;

#include "TpsTestPeSurf2d.h"
#include "TpsRNG.h"

/**
\brief A simple simulation of a single particle in a 2D potential field. The 
	equations of motion are integrated using Langevin dynamics.
\ingroup testsuite

*/
class TpsTestPeSurf2dBD : public TpsTestPeSurf2d
{
	public:	

		TpsTestPeSurf2dBD(TpsRNG&);

		void run(int nsteps);
		void initConstants();				
		void setTimestep(double);
		void setDragCoefficient(double);		
		void setTemperature(double);
		
	private:
				
		void bivariateRand(double &r, double& v);		
		//system variables
		double _dt;
		double _sigma;
		
		//langevin thermostat/integrator variables
		double _gamma;
		double _norm;
		double _c0, _c1, _c2, _c1min2, _c_rv;
		
		//bivariate rng variables
		TpsRNG& _rng;
		double _sig_r, _sig_v;
		double _s12os11, _sqrts11, _sqrtSos11;		
};

#endif
