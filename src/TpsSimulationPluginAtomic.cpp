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

#include "TpsSimulationPluginAtomic.h"
#include <cassert>
#include <iostream>

void error_message(const char* function_name)
{
	std::cerr << "TpsSimulationPluginAtomic::" << function_name << " "; 
	std::cerr << "should have been re-implemented by a derived class\n.";
	std::cerr << "Use the debugger to trace back to the function call that";
	std::cerr << "caused this error.\n";
	int code_gets_to_this_point = 1;
	assert(!code_gets_to_this_point);
}

TpsSimulationPluginAtomic::TpsSimulationPluginAtomic()	
{
}

TpsSimulationPluginAtomic::~TpsSimulationPluginAtomic()
{
}

void TpsSimulationPluginAtomic::invertVelocities()
{
	std::vector< std::vector<double> > v;
	copyVelocitiesTo(v);
	
	//note: don't use _natom here; there may not be any velocities
	for(unsigned int i=0; i<v.size(); i++)	{
		v[i][0] = -1.0 * v[i][0];
		v[i][1] = -1.0 * v[i][1];
		v[i][2] = -1.0 * v[i][2];
	}

	setVelocities(v);
}

int TpsSimulationPluginAtomic::getNumberOfAtoms()
{
	error_message("getNumberOfAtoms");
	return 0;
}

void TpsSimulationPluginAtomic::copyBoxTo(BoxInfo&)
{
	error_message("copyBoxTo");
}

void TpsSimulationPluginAtomic::copyTypesTo(std::vector<int>&)
{
	error_message("copyTypesTo");
}

void TpsSimulationPluginAtomic::copyPositionsTo(
	std::vector< std::vector<double> >&)
{
	error_message("copyPositionsTo");
}

void TpsSimulationPluginAtomic::copyVelocitiesTo(
	std::vector< std::vector<double> >&)
{
	//error_message("copyVelocitiesTo");
}

void TpsSimulationPluginAtomic::setPositions(
	std::vector< std::vector<double> >&)
{
	error_message("setPositions");
}

void TpsSimulationPluginAtomic::setVelocities(
	std::vector< std::vector<double> >&)
{
	//error_message("setVelocities");
}

double TpsSimulationPluginAtomic::computeKinetic()
{
	error_message("computeKinetic");
	return 0.0;
}

void TpsSimulationPluginAtomic::setTimestep(double timestep)
{
	error_message("setTimestep");
}

double TpsSimulationPluginAtomic::getTimestep()
{
	error_message("getTimestep");
	return 0.0;
}

int TpsSimulationPluginAtomic::getDimensions()
{
	error_message("getDimensions");
	return 0;
}
