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
 *  TpsAlgorithm.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#ifndef TPSALGORITHM
#define TPSALGORITHM

#include <vector>

class TpsTrajectoryEnsemble;
class TpsOrderParameter;
class TpsRNG;

/**
\brief The main class that puts the pieces together into a working algorithm
\ingroup core

Each controller implements a different transition path-sampling scheme.  
Depending on the nature of the problem and the desired information, some schemes 
may be more efficient than others, or give more pertinent results.  The 
algorithm classes themselves often contain very little code; they simply 
string together the API classes listed below into a short path sampling 
algorithm, comparable in length to writing the steps down on paper.  
The ability to construct custom algorithms using the TPS API classes 
is the main goal of this library.  
*/
class TpsAlgorithm
{
	public: 
		TpsAlgorithm(TpsTrajectoryEnsemble&, TpsRNG&, TpsOrderParameter&);
		virtual ~TpsAlgorithm();
		
		virtual void doStep();
				
		TpsTrajectoryEnsemble& getTrajectoryFactory();
		TpsRNG& getRNG();
		TpsOrderParameter& getPathFunction();
		
		virtual void callbackOnTrialMove();
		virtual void callbackOnTrialMoveAccepted();
		virtual void callbackOnTrialMoveRejected();
		virtual void callbackOnStepCompleted();
		
	protected:
		TpsRNG& _rng;
		TpsOrderParameter& _path_function;		
		TpsTrajectoryEnsemble& _trajectory_factory;
};

#endif
