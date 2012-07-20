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


#include "TpsAnalysisTransitionTimes.h"
#include "TpsTimeslice.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

TpsAnalysisTransitionTimes::TpsAnalysisTransitionTimes(
	TpsOrderParameter& order_param, const char* filename, int write_time)
:	_order_parameter(order_param),
	_filename(filename), 
	_times(1),
	_write_time(write_time)
{
}

TpsAnalysisTransitionTimes::~TpsAnalysisTransitionTimes()
{
}

int TpsAnalysisTransitionTimes::compute(TpsTrajectory& traj)
{	
	int nm = traj.getNumberOfTimeslices();
	if (nm == 0) {
		return -1;
	}
	
	int tmin = traj.getTimeslice(0).getTimesteps();
	for (int i=0; i<nm; i++) {
		int tmax = traj.getTimeslice(i).getTimesteps();
		TpsTrajectory subtraj(traj);
		subtraj.setID(-11);
		subtraj.copy(traj, tmin, tmax);
		
		if (_order_parameter.isTransitionPath(subtraj)) {
			return tmax;
		}
	}
	return -1;
}

void TpsAnalysisTransitionTimes::analyze(TpsTrajectory& traj)
{		
	_transition_time[traj.getID()] = compute(traj);
	
	if (_times++ % _write_time == 0 && _filename != "none") {	
		std::ostream *os = NULL;
		std::ofstream file;
		
		if (_filename == "stdout") {
			os = &std::cout;
		}
		else if (_filename == "stderr") {
			os = &std::cerr;
		}
		else {
			file.open(_filename.c_str());
			os = &file;
		}
		
		using std::map;
		map<int, int>::iterator iter = _transition_time.begin();
		
		for (iter = iter; iter != _transition_time.end(); ++iter) {
			*os << iter->first <<"\t"<< iter->second <<"\n";
		}
	}
}
