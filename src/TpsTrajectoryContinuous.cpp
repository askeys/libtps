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
 *  TpsTrajectoryContinuous.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/21/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrajectoryContinuous.h"
#include "TpsRNG.h"
#include "TpsTimeslice.h"
#include "TpsSimulationPlugin.h"
#include <algorithm>
#include <iostream>
#include <cassert>

TpsTrajectoryContinuous::TpsTrajectoryContinuous(
	TpsSimulationPlugin& sim, TpsRNG& rng, 
	int path_length, int intervals)
:	TpsTrajectory(sim, path_length, 50),
	_rng(rng),
	_n_restart_time(intervals)
{
}

TpsTrajectoryContinuous::TpsTrajectoryContinuous(TpsTrajectoryContinuous& t)
:	TpsTrajectory(t), 
	_rng(t._rng),
	_restart_timeslice(t._restart_timeslice),
	_restart_time(t._restart_time),
	_n_restart_time(t._n_restart_time)
{
}

TpsTrajectoryContinuous* TpsTrajectoryContinuous::clone()
{
	return new TpsTrajectoryContinuous(*this);
}

TpsTrajectoryContinuous::~TpsTrajectoryContinuous()
{
}

void TpsTrajectoryContinuous::setNumberOfIntervals(int ntime)
{
	_n_restart_time = ntime;
}

void TpsTrajectoryContinuous::copy(TpsTrajectory& traj)
{
	try {
		TpsTrajectoryContinuous& trajc 
			= dynamic_cast<TpsTrajectoryContinuous&>(traj);
		_restart_time = trajc._restart_time;
		_restart_timeslice = trajc._restart_timeslice;
		TpsTrajectory::copy(traj);
	}
	catch (std::exception& e) {
		int cast_to_continuous_trajectory = 0;
		assert(cast_to_continuous_trajectory);
	}
}

TpsTimeslice& TpsTrajectoryContinuous::getRandomTimeslice(TpsRNG& rng)
{
	if (_restart_timeslice.size() == 0) {
		TpsTimeslice m = _timeslice[_timeslice.size()/2];
		for (unsigned int i=0; i<_timeslice.size(); i++) {
			if (i != _timeslice.size()/2) {
				_sim.freeRestart(_timeslice[i].getFilename().c_str());
			}
		}
		_timeslice.clear();
		_timeslice.push_back(m);
		extendBackward();
		extendForward();
	}

	assert(_restart_timeslice.size() > 0);
	TpsTimeslice* m = _restart_timeslice[0];
	_restart_timeslice.pop_front();
	return *m;
}


void TpsTrajectoryContinuous::shiftTimesteps(int timesteps)
{
	assert(_timeslice.size() > 0);
	
	_t_shift += timesteps;
	int t0 = _t_shift;
	int tf = _t_length + _t_shift;
	
	std::deque<int> new_t;
	for (int i=0; i<_n_restart_time; i++) {
		int t = _rng.randInt()%timesteps;
		if (timesteps > 0) {
			new_t.push_back(tf-t);
		}
		else {
			new_t.push_back(t0+t);			
		}
	}
	
	while (_timeslice[0].getTimesteps() < t0) {
		_sim.freeRestart(_timeslice[0].getFilename().c_str());
		int old_t = _timeslice.front().getTimesteps();
		for (unsigned int i=0; i<_restart_time.size(); i++) {
			if (_restart_time[i] == old_t) {
				_restart_time[i] = new_t[0];
				new_t.pop_front();
			}
		}
		_timeslice.pop_front();
	}

	while (_timeslice[_timeslice.size()-1].getTimesteps() > tf) {
		_sim.freeRestart(
			_timeslice[_timeslice.size()-1].getFilename().c_str());
		int old_t = _timeslice.back().getTimesteps();
		for (unsigned int i=0; i<_restart_time.size(); i++) {
			if (_restart_time[i] == old_t) {
				_restart_time[i] = new_t[0];
				new_t.pop_front();
			}
		}
		_timeslice.pop_back();
	}
}

void TpsTrajectoryContinuous::generateRestartPts()
{
	int ts = _t_shift;
	int interval = _t_length;
		
	if (_restart_timeslice.size() == 0) {
		_restart_time.clear();
		for (int i=0; i<_n_restart_time; i++) {
			int trand = ts + _rng.randInt() % interval;
			_restart_time.push_back(trand);
		}
	}
}

void TpsTrajectoryContinuous::extendForward(int nslices)
{
	generateRestartPts();
	std::vector<int> time = _restart_time;
	time.push_back(_t_shift);
	TpsTrajectory::extendForward(time);	
	_restart_timeslice.clear();
	for (unsigned int i=0; i<_restart_time.size(); i++) {	
		for (unsigned int j=0; j<_timeslice.size(); j++) {
			if (_restart_time[i] == _timeslice[j].getTimesteps()) {
				_restart_timeslice.push_back(&_timeslice[j]);
			}
		}
	}
}

void TpsTrajectoryContinuous::extendBackward(int nslices)
{
	generateRestartPts();
	std::vector<int> time = _restart_time;
	time.push_back(_t_length + _t_shift);
	TpsTrajectory::extendBackward(time);
	_restart_timeslice.clear();
	for (unsigned int i=0; i<_restart_time.size(); i++) {	
		for (unsigned int j=0; j<_timeslice.size(); j++) {
			if (_restart_time[i] == _timeslice[j].getTimesteps()) {
				_restart_timeslice.push_back(&_timeslice[j]);
			}
		}
	}
}
