/*
 *  TpsTrialMoveShotBackward.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TpsTrialMoveShotBackward.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsTimeslice.h"
#include <iostream>

TpsTrialMoveShotBackward::TpsTrialMoveShotBackward(int max_steps)
    : _max_steps(max_steps)
{
	
}

TpsTrialMoveShotBackward::~TpsTrialMoveShotBackward()
{
}

bool TpsTrialMoveShotBackward::doMove(TpsAlgorithmTPS& controller)
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
        int first_time = old_trajectory.getTimeslice(0).getTimesteps();
        if (restart_time - first_time <= _max_steps) {
            break;
        }
    }
	new_trajectory.copy(old_trajectory, restart_time, INT_MAX);	
	new_trajectory.extendBackward();

	if(path_function.hA(new_trajectory) && path_function.hBorHB(new_trajectory)) {
		controller.getTrajectoryFactory().moveTrajectory(-1, num+1);
		controller.incrementPathNumber();
		return true;
	}
	else {
		old_trajectory.incrementWeight();	
	}
	return false;
}
