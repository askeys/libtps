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
 *  TpsTrialMove.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRIALMOVE_H
#define TPSTRIALMOVE_H

class TpsAlgorithmTPS;
class TpsTrajectory;
class TpsRand;

/**
\brief A trial walk in trajectory space
\ingroup core
\ingroup trial_move

The main idea here is that we can build up various path sampling schemes
by adding trial moves to our algorithm class.  At the present, some 
algorithms cannot be correctly built up in this way, such as the FFS family of 
algorithms.

Example:
\code
TpsAlgorithmTPS tps(tpe, rng, order_param, initializer);
TpsTrialMoveShot shot;
TpsTrialMoveShift shift(min, max);
tps.addTrialMove(shot);
tps.addTrialMove(shift);
\endcode
*/
class TpsTrialMove
{
	public:
		TpsTrialMove();
		virtual ~TpsTrialMove();
		
		void attempt(TpsAlgorithmTPS&);
		void setAttemptsPerCycle(double n);
		double getAttemptsPerCycle();
		int getID();
		double getAcceptanceProbability();
        void resetAcceptanceProbability();
		virtual double getStepSize();
		virtual void setStepSize(double);
		virtual void callbackOnAccepted();
		virtual void callbackOnRejected();	
		virtual void callbackOnAttempt();
		void optimizeStepSize(bool, double=0.5, int=10);

	protected:
		
		virtual void updateStepSize();
		virtual bool doMove(TpsAlgorithmTPS&);
				
		int _n_attemps;
		int _n_accepts;
		
		static int _global_id;
		int _id;
		double _n_attempts_per_cycle;
		
		bool _optimize;
		double _target_acceptance_probability;
		int _optimize_every;
};

#endif
