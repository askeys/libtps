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
 *  TpsTestRandomWalk.h
 *  tpsapi
 *
 *  Created by askeys on 6/11/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */


#ifndef RANDOMWALEXAMPLE_H
#define RANDOMWALEXAMPLE_H

class TpsRNG;

#include "TpsSimulationPluginAtomic.h"
#include "TpsRNG.h"

#include <string>
#include <cmath>
#include <map>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
struct TpsTestRandomWalkTimeslice
{
	double x[2];
	
	double getPositionX();
	double getPositionY();
};
#endif

/**
\brief A simple simulation of a single particle in a 2D potential field
\ingroup testsuite

The equations of motion are integrated using Langevin dynamics.

As with all of the PeSurf2D family of examples, the potential field can be 
modified in two ways:
	-# Overriding the forceFunction() and potentialFunction() methods
	-# Calling the setPotentialField(potential_function, force_function) method
*/
class TpsTestRandomWalk : public TpsSimulationPluginAtomic
{
	public:	

		TpsTestRandomWalk(TpsRNG&);
		void run(int);
		void writeRestart(const char*);
		void readRestart(const char*);
		void freeRestart(const char*);		
		void copyRestart(const char*, const char*);		
		double computeHamiltonian();
		void setParticlePosition(double, double);		
		void copyPositionsTo(std::vector< std::vector<double> >&);
		int getDimensions();
		void setStepSize(double);		
		TpsTestRandomWalkTimeslice& getCurrentTimeslice();

	protected:
		double _xstdev;
		TpsRNG& _rng;
		TpsTestRandomWalkTimeslice _current_timeslice;
		std::map<std::string, TpsTestRandomWalkTimeslice> _saved_timeslices;						
};

#endif
