/*
 *  TpsTrialMoveShotForward.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TPSTRIALMOVEFORWARDSHOT
#define TPSTRIALMOVEFORWARDSHOT

#include "TpsTrialMoveShot.h"
#include <limits.h>

/**
\brief Performs a forward shooting move
\ingroup trial_move
\warning This move only works for stochastic trajeocties (e.g., BD simulations)

The forward shooting move is carried out by choosing a random timeslice
and then integrating the system forward.  The new trajectory is accepted if it 
is reactive.  Note that stochastic systems do not require a momentum (etc.) 
perturbation to create divergent trajectories.  Therefore, shooting in either 
direction still results in a continuous trajectory, in contrast to systems
with deterministic dynamics.
*/
class TpsTrialMoveShotForward : public TpsTrialMoveShot
{
	public:
		TpsTrialMoveShotForward(int=INT_MAX);
		virtual ~TpsTrialMoveShotForward();
		
	protected:
		virtual bool doMove(TpsAlgorithmTPS&);
        int _max_steps;
};

#endif
