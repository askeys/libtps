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
 *  TpsTrialMoveShot.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRIALMOVESHOT_H
#define TPSTRIALMOVESHOT_H

#include "TpsTimesliceModifier.h"
#include "TpsTrialMove.h"

class TpsAlgorithm;

/**
\brief Performs a TPS shooting move
\ingroup trial_move
\image html shot.jpg

A TPS shooting move is a type of Monte-Carlo move that walks the system through
trajectory space.  The move is performed by choosing a random point on an 
existing trajectory and adding a small perturbation.  The new trajectory is 
created by integrating the equations of motion backwards and forwards in time
from the perturbed timeslice.  The result is that the new trajectory differs 
slightly from the old one.  If the new trajectory is reactive, it is accepted,
otherwise it is rejected.
*/
class TpsTrialMoveShot : public TpsTrialMove
{
	public:
		TpsTrialMoveShot();
		TpsTrialMoveShot(TpsTimesliceModifier&);
		virtual ~TpsTrialMoveShot();
		void setTinyTimesteps(int=50, double=-1);
		
		double getStepSize();
		void setStepSize(double);

	protected:
		virtual bool doMove(TpsAlgorithmTPS&);
		static TpsTimesliceModifier _default_perturbation;
		TpsTimesliceModifier& _perturbation;
		int _n_tiny_timestep;
		double _size_tiny_timestep;

};

#endif
