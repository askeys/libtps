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

#ifndef TPSANALYSISCOMMITTER
#define TPSANALYSISCOMMITTER

#include "TpsAnalysis.h"
#include "TpsTimesliceModifier.h"
#include "TpsOrderParameterAggregate.h"
#include <string>
#include <vector>
#include <map>

/**
\brief Computes the committer as a function of one or multiple order parameters
\ingroup analysis

This analysis module offers many different methods for computing the committer.
The committer is typically defined as the propensity for a trajectory to 
"commit" to the reactants state.   

In the simplest case, we might compute the committer as a function of a single 
reaction coordinate "Q."  In more complex cases, we might compute an 
N-dimensional committer surface as a function of several order parameters:
\f$<Q_1, Q_2, ... Q_N>\f$.  The user can either enter a single TpsOrderParameter 
as an input for a 1D committer or a TpsOrderParameterAggregate for an 
N-D committer.

For some applications, such as Aimless Shooting, we adapt a different definition
of the committer as the propensity for a timeslice to produce a member of the 
transition state ensemble if velocities are drawn at random.  The distinction
between these definitions is controlled here by a flag (see "computeFullPath").
*/
class TpsAnalysisCommitter : public TpsAnalysis
{
	public:
		TpsAnalysisCommitter(
			TpsOrderParameter&, TpsTimesliceModifier&, int = 0,
			const char* ="committer.txt", int=0);

		TpsAnalysisCommitter(TpsOrderParameter&, int = 0, 
			const char* ="committer.txt", int=0);

		TpsAnalysisCommitter(
			TpsOrderParameterAggregate&, TpsTimesliceModifier&, int = 0,
			const char* ="committer.txt", int=0);

		TpsAnalysisCommitter(TpsOrderParameterAggregate&, int = 0,
			const char* ="committer.txt", int=0);
		
		void setBinSize(double);
		void setBinSize(std::vector<double>&);
		void setFilename(const char*);
		void setOutputInterval(int);
		void setUseShortcut(bool=true);
		void setUseFullPath(bool=true);
		void setFleetingTrajectoryLength(int=-1);
		void setTinyTimesteps(int=50, double=-1);
		
		double compute(TpsTrajectory& traj, int i, int ntraj);

		virtual ~TpsAnalysisCommitter();

		virtual void analyze(TpsTrajectory&);
		
	protected:
		double computeShortcut(TpsTrajectory& traj, int i, int ntraj);
		double computeFullPath(TpsTrajectory& traj, int i, int ntraj);
		void takeTinyTimesteps(TpsSimulationPlugin&);
		void analyzeShortcut(TpsTrajectory&);
		void analyzeFullPath(TpsTrajectory&);
		void writeCommitter();
		TpsOrderParameterAggregate _default_op_aggregate;
		TpsOrderParameterAggregate& _op_aggregate;
		TpsTimesliceModifier& _velocity_distribution;
		TpsTimesliceModifier _default_velocity_distribution;
		std::string _filename;
		std::map< std::vector<double>, double> _hB;
		std::map< std::vector<double>, double> _count;
		int _times;
		int _output_interval;
		double _bin_size_d;
		std::vector<double> _bin_size;
		int _length_fleeting_traj;
		int _n_fleeting_traj;
		std::vector<double> _hist_min;
		std::vector<double> _hist_max;
		bool _use_shortcut;
		int _n_tiny_timestep;
		double _size_tiny_timestep;
};

#endif
