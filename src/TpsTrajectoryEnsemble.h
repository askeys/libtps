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
 *  TpsTrajectoryEnsemble.h
 *  tpsapi
 *
 *  Created by askeys on 11/8/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef TPSTRAJECTORYFACTORY_H
#define TPSTRAJECTORYFACTORY_H

#include <limits.h>
#include <map>
#include <string>

class TpsTrajectory;
class TpsSimulationPlugin;

/**
\brief Creates/deletes and otherwise organizes trajectories
\ingroup core
\ingroup traj
\bug this class creates directories in a *nix specific way

The main point of the ensemble class is to help the TpsAlgorithms create 
new trajectories in memory.  At the beginning of the code, the ensemble class
is given a single prototype trajectory which it then copies every time a new
blank trajectory is requested.  This allows us to create trajectories with 
consistent length, intervals, etc. for each trial move. As an added bonus,
the ensemble class helps us organize the trajectories for output or analysis.
*/
class TpsTrajectoryEnsemble
{
	public:
		TpsTrajectoryEnsemble(TpsTrajectory&, int=DEFAULT, bool=true);
		virtual ~TpsTrajectoryEnsemble();
		virtual TpsTrajectory& getTrajectory(int);
		TpsTrajectory& getLastTrajectory();
		TpsTrajectory& getFirstTrajectory();
		std::map <int, TpsTrajectory*>& getTrajectoryList();
		void eraseTrajectories(int=INT_MIN, int=INT_MAX);
		void eraseTrajectory(int);
		unsigned int getNumberOfTrajectories();
		void moveTrajectory(int, int);
		void saveTrajectories(const char* = "saved_trajectories.txt");
		void loadTrajectories(const char* = "saved_trajectories.txt");

	protected:
		enum {DEFAULT = -63451};
		std::map <int, TpsTrajectory*> _trajectory;
		TpsTrajectory& _prototype_trajectory;
		int _uniqe_ID;
		static int _s_unique_ID;
        bool _make_directories;
};

#endif
