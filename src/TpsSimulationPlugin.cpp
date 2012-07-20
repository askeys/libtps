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


#include "TpsSimulationPlugin.h"
#include "TpsSimulationPluginAtomic.h"
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <stdio.h>

TpsSimulationPlugin::TpsSimulationPlugin()
{
}

TpsSimulationPlugin::~TpsSimulationPlugin()
{
}

void TpsSimulationPlugin::run(int nsteps)
{
}

void TpsSimulationPlugin::writeData(const char* filename, int mode)
{
	std::cerr << "Error: ";
	std::cerr << "TpsSimulationPlugin::writeData should be implemented\n";
	//exit(1);	
}

void TpsSimulationPlugin::writeRestart(const char* filename)
{
	std::cerr << "Error: ";
	std::cerr << "TpsSimulationPlugin::writeRestart should be implemented\n";
	exit(1);
}

void TpsSimulationPlugin::readRestart(const char* filename)
{
	std::cerr << "Error: ";
	std::cerr << "TpsSimulationPlugin::readRestart should be implemented\n";
	exit(1);
}

void TpsSimulationPlugin::freeRestart(const char* filename)
{
	if (remove(filename) == -1) {
		std::cerr << "Error: could not delete file " << filename << "\n";
	}
}

void TpsSimulationPlugin::copyRestart(
	const char* src_file, const char* dest_file)
{
	std::ostringstream os;
	os << "cp " << src_file << " " << dest_file;
	system(os.str().c_str());
}

void TpsSimulationPlugin::invertVelocities()
{
}

double TpsSimulationPlugin::computeHamiltonian()
{
	return 1e21;
}

double TpsSimulationPlugin::getExpectedTemperature()
{
	return 1.0;
}

void TpsSimulationPlugin::callbackOnRunCommand()
{
}

void TpsSimulationPlugin::callbackOnReadRestartCommand()
{
}

void TpsSimulationPlugin::callbackOnWriteRestartCommand()
{
}

TpsSimulationPluginAtomic& TpsSimulationPlugin::safeDowncastToAtomicPlugin()
{
	try {
		TpsSimulationPluginAtomic& sim_atomic 
			= dynamic_cast<TpsSimulationPluginAtomic&>(*this);
			return sim_atomic;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		int cast_to_atomic_simulation = 0;
		assert(cast_to_atomic_simulation);
	}
}
