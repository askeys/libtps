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
 *  TpsTrajectoryRandomStep.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/21/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrajectoryRandomStep.h"
#include "TpsTimeslice.h"
#include "TpsRNG.h"
#include <cassert>

TpsTrajectoryRandomStep::TpsTrajectoryRandomStep(
	TpsSimulationPlugin& sim, TpsRNG& rng, int path_length, int step_size)
:	TpsTrajectory(sim, path_length, step_size),
	_rng(rng)

{
}

TpsTrajectoryRandomStep::TpsTrajectoryRandomStep(TpsTrajectoryRandomStep& t)
:	TpsTrajectory(t),
	_rng(t._rng)
{
}

TpsTrajectoryRandomStep* TpsTrajectoryRandomStep::clone()
{
	return new TpsTrajectoryRandomStep(*this);
}

TpsTrajectoryRandomStep::~TpsTrajectoryRandomStep()
{
}

void TpsTrajectoryRandomStep::extendForward(int nslices)
{
	assert(_timeslice.size() > 0);
		
	int t0 = _timeslice.back().getTimesteps();
	int tf = INT_MAX;
	if (nslices == TO_ENDPOINT) {
		tf = _t_length + _t_shift;
	}
	
	int ns=0;
	std::vector<int> t_save;
	//t_save.push_back(t0);
	int t = t0;
	while(t<=tf) {
		t += 1 + _rng.randInt() % _t_step;
		t_save.push_back(t);
		if ((++ns) >= nslices) {
			break;
		}
	}
	if (nslices == TO_ENDPOINT) {
		t_save.push_back(tf);
	}
		
	TpsTrajectory::extendForward(t_save);
	if (nslices != TO_ENDPOINT) {
		updatePathLength();
	}
}

void TpsTrajectoryRandomStep::extendBackward(int nslices)
{
	assert(_timeslice.size() > 0);

	int t0 = _timeslice.front().getTimesteps();
	int tf = INT_MIN;
	if (nslices == TO_ENDPOINT) {
		tf = _t_shift;
	}
	
	int ns=0;
	std::vector<int> t_save;
	//t_save.push_back(t0);
	int t = t0;
	while (t>=tf) {
		t -= 1 + _rng.randInt() % _t_step;
		t_save.push_back(t);
		if ((++ns) >= nslices) {
			break;
		}
	}
	if (nslices == TO_ENDPOINT) {
		t_save.push_back(tf);
	}
	TpsTrajectory::extendBackward(t_save);
	if (nslices != TO_ENDPOINT) {
		updatePathLength();
	}
}
