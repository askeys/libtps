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
 *  TpsTrajectoryIterator.cpp
 *  tpsapi
 *
 *  Created by askeys on 11/20/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include "TpsSimulationPlugin.h"
#include <iostream>

TpsTrajectoryIterator::TpsTrajectoryIterator()
:	_t(0),
	_restart_timeslice(0),
	_traj_ptr(0x0)
{
}

bool TpsTrajectoryIterator::operator != (TpsTrajectoryIterator& iter)
{
	if (_restart_timeslice == iter._restart_timeslice) {
		return false;
	}
	else {
		return true;
	}
}

TpsTrajectoryIterator::~TpsTrajectoryIterator()
{
}

TpsSimulationPlugin& TpsTrajectoryIterator::getSimulationPlugin()
{
	return _traj_ptr->getSimulationPlugin();
}

TpsTrajectoryIterator::TpsTrajectoryIterator(const TpsTrajectoryIterator& iter)
{
	(*this) = iter;
}

TpsTrajectoryIterator& TpsTrajectoryIterator::operator = (
	const TpsTrajectoryIterator& rhs)
{
	_t = rhs._t;
	_restart_timeslice = rhs._restart_timeslice;
	_traj_ptr = rhs._traj_ptr;	
	
	if (_traj_ptr->getNumberOfTimeslices() == 0) {
		std::cerr << "WARNING: Iterator pointing at empty trajectory\n";
		return *this;
	}
		
	_traj_ptr->getSimulationPlugin().readRestart(
			_traj_ptr->getTimeslice(_restart_timeslice).getFilename().c_str());
		
	return *this;
}

void TpsTrajectoryIterator::operator+=(int t_step)
{		
	//current timesteps
	int t_final = _t + t_step;
	
	//final save point
	int last_slice = _traj_ptr->getNumberOfTimeslices()-1;

	//the next save point
	int t_nextres = INT_MAX;
	if (_restart_timeslice < last_slice) {
		t_nextres = _traj_ptr->getTimeslice(_restart_timeslice+1).getTimesteps();
	}
	
	//if the new endpoint is less than the next save point, run to endpoint
	if (t_final < t_nextres) {
		_traj_ptr->getSimulationPlugin().run(t_step);
	}
	//otherwise, load the next save point and run from there
	else {		
		_restart_timeslice = -1;
		for (int i=0; i<=last_slice; i++) {
			if (_traj_ptr->getTimeslice(i).getTimesteps() == t_final) {
				_restart_timeslice = i;
				break;
			}
			else if (_traj_ptr->getTimeslice(i).getTimesteps() > t_final) {
				_restart_timeslice = i-1;
				break;
			}
		}
		
		int t_start = _t;

		//if we don't find an appropriate restart timeslice, 
		//set the restart_slice to "last" so the iterator == Trajectory::end()
		if (_restart_timeslice < 0) {
			_restart_timeslice = last_slice;
		}
		//otherwise, load the restart timeslice and set the startime to that 
		//timeslice
		else {
			TpsTimeslice m = _traj_ptr->getTimeslice(_restart_timeslice);
		    t_start = m.getTimesteps();
			_traj_ptr->getSimulationPlugin().readRestart(m.getFilename().c_str());
		}
		
		int step = (_t + t_step) - t_start;
		_traj_ptr->getSimulationPlugin().run(step);
	}	
	_t += t_step;
}
