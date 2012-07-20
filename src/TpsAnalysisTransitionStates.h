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


#ifndef TPSANALYSISTRANSITIONSTATES
#define TPSANALYSISTRANSITIONSTATES

#include "TpsAnalysis.h"
#include "TpsTimeslice.h"
#include <string>
#include <map>
#include <vector>

class TpsAnalysisCommitter;


/**
\brief Identifies members of the Transition State Ensemble (TSE)
\ingroup analysis

This analysis computes the TSE using various methods.  The most straightforward
method is to step though each timeslice and test whether it commits to B
~50% of the time.  This calculation is expensive, so we always use the pruning
method described by Bolhuis, Delago, Geissler, pg. 89.  In order to further
decrease the computational cost, we have implemented an interval bisection
scheme that allows us to avoid calculating committers for many of the 
timeslices.  This method should be used with caution, however, since it may 
miss transition states if the path crosses the separatrix several times over 
the course of the trajectory.  
*/
class TpsAnalysisTransitionStates : public TpsAnalysis
{
	public:
		TpsAnalysisTransitionStates(
			TpsAnalysisCommitter&, int=STEPWISE, const char* ="TSE.txt");
		virtual ~TpsAnalysisTransitionStates();

		virtual void analyze(TpsTrajectory&);
		
		void setSkipTimeslices(int);
		void setNmin(int);
		void setNmax(int);
		void setAlpha(double);
		std::vector<TpsTimeslice>& getTransitionStates();
		enum {STEPWISE=1, BISECTION=2};
		
	protected:
		void analyzeBisection(TpsTrajectory&);
		int saddlePoint(TpsTrajectory&, int);
		void bisectInterval(TpsTrajectory&, int, int, double, double);
		std::string nameFile(int);
		void analyzeLinear(TpsTrajectory&);
		void writeTS(const char*);
		enum {LT, GT, EQUALS};
		TpsAnalysisCommitter& _committer;
		std::string _filename;
		std::vector<TpsTimeslice> _transition_state;
		std::vector<std::string> _temp_file;
		int _nmin, _nmax;
		double _alpha;
		int _method;
		int _skip;
};

#endif
