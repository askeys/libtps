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
 *  TpsTrialMoveShotAimless.h
 *  tpsapi
 *
 *  Created by askeys on 6/9/09.
 *  Copyright 2009 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRIALMOVEAIMLESSSHOT_H
#define TPSTRIALMOVEAIMLESSSHOT_H

#include "TpsTrialMoveShot.h"
#include <vector>
#include <string>

/**
\brief Implements the "Aimless shooting" move
\ingroup trialmove
\ingroup 

Aimless shooting uses a specific trial move that defines the algorithm.  
Therefore, it is not possible to add different trial moves as it is for
some of the other TPS algorithms.  The only adjustable parameter is "delta
timeslices" which defines the maximum shift of a new shooting point from
the old shooting point.

Reference: B Peters, BL Trout, J. Chem. Phys, 2006
*/
class TpsTrialMoveShotAimless : public TpsTrialMoveShot
{
	public:
		TpsTrialMoveShotAimless(int=1);
		TpsTrialMoveShotAimless(TpsTimesliceModifier&, int=1);
		void setDeltaTimeslices(int);
		std::vector<std::string>& getShootingPoints(); 
		void clearShootingPoints(TpsSimulationPlugin&);

	protected:
		virtual bool doMove(TpsAlgorithmTPS&);

		std::vector<std::string> _shooting_points;
		int _max_path_length;
		int _delta_timeslices;
		int _n_shooting_pts;
		int _old_shooting_pt;

};

#endif
