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
 *  TpsTrajectoryIterator.h
 *  tpsapi
 *
 *  Created by askeys on 11/20/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRAJECTORYITERATOR
#define TPSTRAJECTORYITERATOR

class TpsTrajectory;
class TpsSimulationPlugin;

/**
\brief A convenience class to facilitate iteration along a trajectory
\ingroup traj

The easiest way to iterate along a trajectory is to load the earliest timeslice
and run the simulation for "n" size intervals.  However, for some simulation 
codes, trajectories may diverge after long times due to roundoff error on the 
order of the machine precision.  Consider, for example, what happens when we
save a timeslice at time "t" in an MD simulation 10 timesteps before the 
neighbor list is rebuilt.  When we reload the simulation at time "t" we will 
probably re-build the neighbor list at that step, since saving all of the 
neighbor list information would be expensive and redundant.  Now our neighbor 
list builds for the original simulation an our re-loaded simulation are 
offset by some time and, as a result, the order in which forces are summed in 
the simulation may be different.  This can lead to roundoff error which causes
a very slightly difference in the original and re-created trajeocties.  The 
point of the iterator, then, is to re-load the saved timeslice each time
it encounters one when stepping through time.  This way, the roundoff error 
never persists for long enough to cause a noticeable change in the trajectory,
provided timeslices are saved frequently enough.  (The required freqency 
depends on the size of the timesteps as well as the system, but the minimal 
frequency is typically on the order of 1,000-10,000 MD steps).
*/
class TpsTrajectoryIterator
{
	friend class TpsTrajectory;
	
	public:
		TpsTrajectoryIterator();
		TpsTrajectoryIterator(const TpsTrajectoryIterator&);
		virtual ~TpsTrajectoryIterator();
		TpsTrajectoryIterator& operator = (const TpsTrajectoryIterator&);
		bool operator != (TpsTrajectoryIterator&);
		void operator += (int);
		TpsSimulationPlugin& getSimulationPlugin();
	
	protected:
		int _t;
		int _restart_timeslice;
		TpsTrajectory* _traj_ptr;
		
};

#endif
