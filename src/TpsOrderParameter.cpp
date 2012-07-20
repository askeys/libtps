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
 *  TpsOrderParameter.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/21/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#include "TpsOrderParameter.h"
#include "TpsTrajectory.h"
#include "TpsSimulationPlugin.h"
#include "TpsTimeslice.h"
#include <cassert>

int TpsOrderParameter::_global_id = 0;

TpsOrderParameter::TpsOrderParameter()
:	_use_HB(false),
	_unique_id(_global_id++)
{
}

TpsOrderParameter::~TpsOrderParameter()
{
}

bool TpsOrderParameter::isTransitionPath(TpsTrajectory& trajectory)
{
	if(hA(trajectory) && hB(trajectory)) {
		return true;
	}
	else {
		return false;
	}
}

bool TpsOrderParameter::hBorHB(TpsTrajectory& traj)
{
	if(_use_HB) {
		return HB(traj);
	}
	else {
		return hB(traj);
	}

}

void TpsOrderParameter::useHB(bool b)
{
	_use_HB = b;
}

bool TpsOrderParameter::hA(TpsTrajectory&)
{
	//this should be modified...
	return true;
}

bool TpsOrderParameter::hB(TpsTrajectory&)
{
	int this_virtual_function_was_overridden = 0;
	assert(this_virtual_function_was_overridden);
	return true;
}

bool TpsOrderParameter::HB(TpsTrajectory& traj)
{
	int n = traj.getNumberOfTimeslices();
	assert(n > 0);	
	
	TpsTrajectory* traj_clone = traj.clone();
	traj_clone->setID(-10001);
	traj_clone->copy(traj);
	
	for (int i=0; i<n; i++) {
		bool good = hB(*traj_clone);
		if (good) {
			delete traj_clone;		
			return true;
		}
		traj_clone->popBack();
	}
	delete traj_clone;		
	return false;
}

int TpsOrderParameter::getUniqueID()
{
	return _unique_id;
}

double TpsOrderParameter::evaluate(TpsTrajectory&, int)
{
	bool this_virtual_function_was_overridden = false;
	assert(this_virtual_function_was_overridden);
	return 0.0;
}
