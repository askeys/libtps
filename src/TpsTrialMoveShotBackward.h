/*
 *  TpsTrialMoveShotBackward.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TPSTRIALMOVEBACKWARDSHOT
#define TPSTRIALMOVEBACKWARDSHOT

#include "TpsTrialMoveShot.h"
#include <limits.h>

/**
\brief Performs a backwards shooting move
\ingroup trial_move

The backward shooting move is carried out by choosing a random timeslice
and then integrating the system backward.  The new trajectory is accepted if it 
is reactive.  Note that stochastic systems do not require a momentum (etc.) 
perturbation to create divergent trajectories.  Therefore, shooting in either 
direction still results in a continuous trajectory, in contrast to systems
with deterministic dynamics.
*/
class TpsTrialMoveShotBackward : public TpsTrialMoveShot
{
	public:
		TpsTrialMoveShotBackward(int=INT_MAX);
		virtual ~TpsTrialMoveShotBackward();
		
	protected:
		virtual bool doMove(TpsAlgorithmTPS&);
        int _max_steps;
};

#endif
