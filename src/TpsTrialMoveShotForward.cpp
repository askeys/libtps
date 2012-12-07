/*
 *  TpsTrialMoveShotForward.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TpsTrialMoveShotForward.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsTimeslice.h"
#include <iostream>

TpsTrialMoveShotForward::TpsTrialMoveShotForward(int max_steps) 
    : _max_steps(max_steps)
{
}

TpsTrialMoveShotForward::~TpsTrialMoveShotForward()
{
}

bool TpsTrialMoveShotForward::doMove(TpsAlgorithmTPS& controller)
{	
	int num = controller.getPathNumber();
	TpsTrajectory& old_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(num);
	TpsTrajectory& new_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(-1);
	TpsRNG& rng = controller.getRNG();
	TpsOrderParameter& path_function = controller.getPathFunction();
	
    int restart_time = 0;
    while (1) {
        new_trajectory.clear();
        new_trajectory.shootFrom(old_trajectory, rng);
        restart_time = new_trajectory.getTimeslice(0).getTimesteps();
        int nslice = old_trajectory.getNumberOfTimeslices();
        int last_time = old_trajectory.getTimeslice(nslice-1).getTimesteps();
        if (last_time - restart_time <= _max_steps) {
            break;
        }
    }
    new_trajectory.copy(old_trajectory, INT_MIN, restart_time);
	new_trajectory.extendForward();
	
	//depending on what we are doing, we may want to count
	//paths that end in B (hb) or paths that visit B (HB)
	if(path_function.hBorHB(new_trajectory)) {
		controller.getTrajectoryFactory().moveTrajectory(-1, num+1);
		controller.incrementPathNumber();
		return true;
	}
	old_trajectory.incrementWeight();	
	return false;
}
