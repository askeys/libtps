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


#ifndef TPSANALYSISWRITETRAJECTORY
#define TPSANALYSISWRITETRAJECTORY

#include "TpsAnalysis.h"
#include <string>
#include <vector>

/**
\brief Writes a trajectory using the "writeData" method from TpsSimulationPlugin
\ingroup analysis

Often we want write our trajectories to a file for post processing.  The 
quickest, most flexible way to do this is to implement a custom trajectory 
writer using TpsAnalysisWriteTrajectory.  This class calls the "writeData" 
method from TpsSimulationPlugin along the trajectory.  Therefore, implementing
a custom writer in theis way requires two steps:

-# Implement the "writeData" method
-# Call TpsAnalysisWriteTrajectory

\example example_trajectory_writer.cpp
*/

class TpsAnalysisWriteTrajectory : public TpsAnalysis
{
	public:
		TpsAnalysisWriteTrajectory(const char* ="trajectory.xyz", int=50);
			
		virtual ~TpsAnalysisWriteTrajectory();

		virtual void analyze(TpsTrajectory&);
		
	protected:
		std::string _filename;
		int _step_size;	
		int _trajectory_wildcard_index;
		int _timestep_wildcard_index;
};

#endif
