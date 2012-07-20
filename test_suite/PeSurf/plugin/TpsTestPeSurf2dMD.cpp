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
 *  TpsTestPeSurf2dMD.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestPeSurf2dMD.h"
#include <iostream>
#include <cmath>

TpsTestPeSurf2dMD::TpsTestPeSurf2dMD()
:	_dt(0.025)
{	
}

void TpsTestPeSurf2dMD::run(int nsteps)
{
	for (int i=0; i<nsteps; i++) {
		verlet(_current_timeslice);
	}
	callbackOnRunCommand();
}

double TpsTestPeSurf2dMD::getExpectedTemperature()
{
	std::cerr << "Warning: this is an NVE MD simulation, you shouldn't";
	std::cerr << " be using trial moves that require calculation of ";
	std::cerr << "the temperature\n";
	return 0.0;
}

//------------------------
//everything defined below this point is not required...

void TpsTestPeSurf2dMD::setTimestep(double dt)
{
	_dt = dt;
}

void TpsTestPeSurf2dMD::verlet(TpsTestPeSurf2dTimeslice& m)
{
	double dt2 = 0.5 *_dt;

	m.v[0] +=  dt2 * m.f[0];
	m.v[1] +=  dt2 * m.f[1];     
	m.x[0]  +=  _dt * m.v[0];
	m.x[1]  +=  _dt * m.v[1];
	updateForce();
	m.v[0] +=  dt2* m.f[0];
	m.v[1] +=  dt2* m.f[1];

	return;
}
