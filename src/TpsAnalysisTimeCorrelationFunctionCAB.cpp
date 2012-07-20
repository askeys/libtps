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

#include "TpsAnalysisTimeCorrelationFunctionCAB.h"
#include "TpsTimeslice.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>

TpsAnalysisTimeCorrelationFunctionCAB::TpsAnalysisTimeCorrelationFunctionCAB(
	TpsOrderParameter& order_param, const char* filename, 
	double bin_size, int write_time)
:	_order_parameter(order_param),
	_filename(filename), 
	_time_bin_size(bin_size),
	_write_time(write_time),
	_times(1)
{
}

TpsAnalysisTimeCorrelationFunctionCAB::~TpsAnalysisTimeCorrelationFunctionCAB()
{
}


void TpsAnalysisTimeCorrelationFunctionCAB::analyze(TpsTrajectory& traj)
{		
	int n = traj.getNumberOfTimeslices();
	assert(n > 0);	
	int t0 = traj.getTimeslice(0).getTimesteps();

	TpsTrajectory* traj_clone = traj.clone();
	traj_clone->setID(-10002);
	traj_clone->copy(traj);
	
	for (int i=n-1; i>0; i--) {
		int t = traj.getTimeslice(i).getTimesteps() - t0;
		t = (int)(rint(t/_time_bin_size) * _time_bin_size);
		
		if (_cab.find(t) == _cab.end()) {
			_cab[t] = 0.0;
			_count[t] = 0.0;
		}
		
		if (_order_parameter.hB(*traj_clone)) {
			_cab[t] += traj.getWeight();
		}
		_count[t] += traj.getWeight();
		traj_clone->popBack();
	}
	delete traj_clone;		

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
		map<int, double>::iterator iter = _cab.begin();
		
		for (iter = iter; iter != _cab.end(); ++iter) {
			*os << iter->first <<"\t"<< iter->second/_count[iter->first]<<"\n";
		}
	}
}
