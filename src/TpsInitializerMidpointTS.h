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
 *  TpsInitializerMidpointTS.h
 *  tpsapi
 *
 *  Created by askeys on 2/24/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#ifndef TPSINITIALIZERMIDPOINTTS
#define TPSINITIALIZERMIDPOINTTS

#include "TpsInitializer.h"
class TpsAnalysisCommitter;
class TpsOrderParameter;

/**
\brief Creates a reactive trajectory with a transition state at the midpoint
\ingroup init

This method requires another initializer to create an initial reactive 
trajectory. After the trajectory has been created, this initializer uses
committer analysis (TpsAnalysisCommitter) to ensure that the new trajectory is 
centered near a transition state.  This is mostly useful for the Aimless 
Shooting method, which can take a very long time to "warm up" if not initialized 
near a transition state.
*/
class TpsInitializerMidpointTS : public TpsInitializer
{
	public:
		TpsInitializerMidpointTS(
			TpsOrderParameter&, TpsAnalysisCommitter&, TpsInitializer&, 
			int=20, double=3.0, int=100);
				
		virtual ~TpsInitializerMidpointTS();
		
		void initializeTrajectory(TpsTrajectory&, TpsOrderParameter&);
		
	protected:
		TpsOrderParameter& _order_param;
		TpsAnalysisCommitter& _committer; 
		TpsInitializer& _base_initializer;
		int _n_committer_trajectories;											///<number of trajectories to attempt per committer computation
		double _tolerance;														///<maximum acceptable distance from Pb=0.5
		int _max_tries;															///<maximum number of attempts before giving up
};

#endif
