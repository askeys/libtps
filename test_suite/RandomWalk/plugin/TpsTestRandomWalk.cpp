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
 *  TpsTestRandomWalk.cpp
 *  tpsapi
 *
 *  Created by askeys on 6/11/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestRandomWalk.h"
#include <iostream>

TpsTestRandomWalk::TpsTestRandomWalk(TpsRNG& rng)
: _xstdev(1.0), _rng(rng)
{	
	_current_timeslice.x[0] = 0.0;
	_current_timeslice.x[1] = 0.0;
}

void TpsTestRandomWalk::setStepSize(double xstdev)
{
	_xstdev = xstdev;
}

void TpsTestRandomWalk::run(int nsteps)
{
	for (int i=0; i<nsteps; i++) {
		double dx = _rng.randNormal(0.0, _xstdev);
		double dy = _rng.randNormal(0.0, _xstdev);

		_current_timeslice.x[0] += dx;
		_current_timeslice.x[1] += dy;
	}
	callbackOnRunCommand();
}

int TpsTestRandomWalk::getDimensions()
{
	return 2;
}

void TpsTestRandomWalk::copyPositionsTo(std::vector< std::vector<double> >&x)
{
	x.resize(1);
	std::vector<double> xi(2, 0.0);
	xi[0] = _current_timeslice.x[0];
	xi[1] = _current_timeslice.x[1];
	x[0] = xi;
}

void TpsTestRandomWalk::writeRestart(const char* filename)
{
	std::string timeslice_name = filename;
	_saved_timeslices[timeslice_name] = _current_timeslice;
}

void TpsTestRandomWalk::readRestart(const char* filename)
{			
	std::string timeslice_name = filename;
	_current_timeslice = _saved_timeslices[timeslice_name];
}

void TpsTestRandomWalk::freeRestart(const char* filename)
{
	std::string timeslice_name = filename;
	_saved_timeslices.erase(timeslice_name);
}

void TpsTestRandomWalk::copyRestart(const char* src_file, const char* dest_file)
{
	std::string name1 = src_file;
	std::string name2 = dest_file;				
	_saved_timeslices[name2] = _saved_timeslices[name1];
}

double TpsTestRandomWalk::computeHamiltonian()
{
	return 0.0;
}

//everything defined below this point is not required...

void TpsTestRandomWalk::setParticlePosition(double x, double y)
{
	_current_timeslice.x[0] = x;
	_current_timeslice.x[1] = y;
}

TpsTestRandomWalkTimeslice& TpsTestRandomWalk::getCurrentTimeslice()
{
	return _current_timeslice;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
double TpsTestRandomWalkTimeslice::getPositionX()
{
	return x[0];
}

double TpsTestRandomWalkTimeslice::getPositionY()
{
	return x[1];
}
#endif
