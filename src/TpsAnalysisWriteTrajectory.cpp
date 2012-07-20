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

#include "TpsAnalysisWriteTrajectory.h"
#include "TpsSimulationPlugin.h"
#include "TpsTrajectory.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

TpsAnalysisWriteTrajectory::TpsAnalysisWriteTrajectory(
	const char* filename, int step
)
:	_filename(filename), 
	_step_size(step),
	_trajectory_wildcard_index(-1),
	_timestep_wildcard_index(-1)
{
	for (unsigned int i=0; i<_filename.size(); i++) {
		if (_filename[i] == '%') {
			_trajectory_wildcard_index = i;
		}
		if (_filename[i] == '*') {
			_timestep_wildcard_index = i;
		}
	}
}

TpsAnalysisWriteTrajectory::~TpsAnalysisWriteTrajectory()
{
}

void TpsAnalysisWriteTrajectory::analyze(TpsTrajectory& traj)
{	
	std::string tmpfilename = _filename;
	
	if (_trajectory_wildcard_index >= 0) {
		std::ostringstream os;
		os << traj.getID();
		tmpfilename.replace(_trajectory_wildcard_index, 1, os.str());
	}
	
	int t=0;
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();
	sim.writeData(tmpfilename.c_str(), -1);

	for (TpsTrajectoryIterator i=traj.begin(); i!=traj.end(); i+=_step_size) {
		std::string filename = tmpfilename;

		if (_timestep_wildcard_index >=0) {
			std::ostringstream os;
			os << t;
			t+=_step_size;
			filename.replace(
				_timestep_wildcard_index, os.str().size(), os.str());
		}
		
		//TpsSimulationPlugin& sim = i.getSimulationPlugin();
		sim.writeData(filename.c_str(), 0);
	}
	
	sim.writeData(tmpfilename.c_str(), 1);
}
