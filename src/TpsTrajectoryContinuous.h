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
 *  TpsTrajectoryContinuousContinuous.h
 *  tpsapi
 *
 *  Created by askeys on 10/21/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRAJECTORYCONTINUOUS
#define TPSTRAJECTORYCONTINUOUS

#include "TpsTrajectory.h"
#include <deque>

/**
\brief A trajectory that simulates saving timeslices at every point
\ingroup traj
\bug Not sure if this trajectory still works (AK)

Of course, timeslices are not actually saved at every point.  The algorithm
goes as follows: 
	-# Choose n restart points
	-# Generate the trajectory saving the n timeslices
	-# Try restarting from the points one at a time
	-# If all the points fail, choose n more restart points
	-# Regenerate the trajectory, go to step 3
	
This simulates saving the trajectory at every point and choosing a point at
random.  The difference is, we choose our restart points first, and only
save n corresponding timeslices at a given time.
*/
class TpsTrajectoryContinuous : public TpsTrajectory
{
	public:
		TpsTrajectoryContinuous(TpsSimulationPlugin&, TpsRNG&, 
			int=-1, int=10);
		TpsTrajectoryContinuous(TpsTrajectoryContinuous&);
		virtual TpsTrajectoryContinuous* clone();
		virtual ~TpsTrajectoryContinuous();
				
		virtual TpsTimeslice& getRandomTimeslice(TpsRNG&);
		virtual void extendForward(int nslices=TO_ENDPOINT);
		virtual void extendBackward(int nslices=TO_ENDPOINT);
		void copy(TpsTrajectory&);
		void setNumberOfIntervals(int);
		void shiftTimesteps(int);
	protected:
		void generateRestartPts();
		TpsRNG& _rng;
		std::deque<TpsTimeslice*> _restart_timeslice;
		std::vector<int> _restart_time;
		int _n_restart_time;
		int _restart_index;
		int _t_shift_old;
};

#endif
