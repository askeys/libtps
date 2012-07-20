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

#include "TpsAnalysisTransitionStates.h"
#include "TpsTimeslice.h"
#include "TpsTrajectory.h"
#include "TpsAnalysisCommitter.h"
#include "TpsSimulationPlugin.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>

TpsAnalysisTransitionStates::TpsAnalysisTransitionStates(
	TpsAnalysisCommitter& committer, int method, const char* filename)
:	_committer(committer),
	_filename(filename),
	_nmin(10),
	_nmax(100), 
	_alpha(2), 
	_method(method),
	_skip(1)
{
}

TpsAnalysisTransitionStates::~TpsAnalysisTransitionStates()
{
}

void TpsAnalysisTransitionStates::setSkipTimeslices(int skip)
{
	_skip = skip;
}

void TpsAnalysisTransitionStates::setNmin(int nmin)
{
	_nmin = nmin;
}

void TpsAnalysisTransitionStates::setNmax(int nmax)
{
	_nmax = nmax;
}

void TpsAnalysisTransitionStates::setAlpha(double alpha)
{
	_alpha = alpha;
}

void TpsAnalysisTransitionStates::writeTS(const char* tse_member)
{
	std::ostream *os = NULL;
	std::ofstream file;
	
	if (_filename == "stdout") {
		os = &std::cout;
	}
	else if (_filename == "stderr") {
		os = &std::cerr;
	}
	else {
		file.open(_filename.c_str(), std::ios::app);
		os = &file;
	}
	
	*os << tse_member << "\n";
}

std::vector<TpsTimeslice>& TpsAnalysisTransitionStates::getTransitionStates()
{
	return _transition_state;
}

void TpsAnalysisTransitionStates::analyzeLinear(TpsTrajectory& traj)
{		
	int n = traj.getNumberOfTimeslices();
	assert(n > 0);	
	
	for (int i=0; i<n; i+=_skip) {
		saddlePoint(traj, i);
	}
}

std::string TpsAnalysisTransitionStates::nameFile(int t)
{
	std::ostringstream os;
	os << "computetse."<< t << ".tmp.restart";
	return os.str();
}

int TpsAnalysisTransitionStates::saddlePoint(TpsTrajectory& traj, int i)
{
	double pb = _committer.compute(traj, i, _nmin);
	double nb = pb*_nmin;
	double sigma = sqrt(pb*(1.0-pb)/_nmin);
	int ntotal = _nmin;
	while (pb - _alpha*sigma <= 0.5 && pb + _alpha*sigma >= 0.5) {
		nb += _committer.compute(traj, i, 1);
		if (ntotal++ > _nmax) {
			TpsSimulationPlugin& sim = traj.getSimulationPlugin();
			std::string fname = traj.getTimeslice(i).getFilename();			
			sim.readRestart(fname.c_str());
			
			sim.writeRestart(fname.c_str());
			writeTS(traj.getTimeslice(i).getFilename().c_str());
			_transition_state.push_back(traj.getTimeslice(i));
			return EQUALS;
		}
		pb = nb/(double)ntotal;	
		sigma = sqrt(pb*(1.0-pb)/ntotal);
	}
	
	if (pb - _alpha*sigma > 0.5) {
		return GT;
	}
	else if (pb + _alpha*sigma < 0.5) {
		return LT;
	}
	
	return EQUALS;
}

void TpsAnalysisTransitionStates::bisectInterval(
	TpsTrajectory& traj, int tstart, int tend, 
	double statestart, double stateend)
{		
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();
	std::string filename = nameFile(tstart);
	sim.readRestart(filename.c_str());
	
	int interval = tend - tstart;
	
	//std::cerr << tstart <<"\t" << tend <<"\n";
	
	if (interval <= 1) {
		return;
	}

	int tmid = tstart + interval / 2;	
	sim.run(interval/2);
		
	filename = nameFile(tmid);
	sim.writeRestart(filename.c_str());
	_temp_file.push_back(filename);
	TpsTimeslice m;
	m.setFilename(filename.c_str());
	m.setTimesteps(tmid);	

	traj.insert(m);
	int statemid = saddlePoint(traj, traj.find(m));
	traj.erase(m);
	
	if (statestart == LT && statemid == GT) {
		bisectInterval(traj, tstart, tmid, statestart, statemid);
	}
		
	if (statemid == LT && stateend == GT) {
		bisectInterval(traj, tmid, tend, statemid, stateend);
	}
}

void TpsAnalysisTransitionStates::analyzeBisection(TpsTrajectory& traj)
{		
	int n = traj.getNumberOfTimeslices();
	assert(n > 1);
	
	int nmm1 = n-1;

	int statestart = saddlePoint(traj, 0);
	
	for (int i=0; i<nmm1; i+=_skip) {
		int stateend = saddlePoint(traj, i+1);		
		//if neither endpoint is at the saddle point and the start and end are
		//in different states, inverval must contain a saddle point; so 
		//bisect the interval
		
		//std::cerr << i <<"\t" << statestart <<"\t" << stateend << "\n";
		if (!(statestart == EQUALS || stateend == EQUALS) &&
				stateend != statestart) {
			int tstart = traj.getTimeslice(i).getTimesteps();
			int tend = traj.getTimeslice(i+1).getTimesteps();
			TpsSimulationPlugin& sim = traj.getSimulationPlugin();
			std::string filename = nameFile(tstart);
			sim.writeRestart(filename.c_str());
			_temp_file.push_back(filename);
			bisectInterval(traj, tstart, tend, statestart, stateend);
			for (unsigned int k=0; k<_temp_file.size(); k++) {
				sim.freeRestart(_temp_file[k].c_str());
			}
			_temp_file.clear();
		}
		statestart = stateend;
	}	
}


void TpsAnalysisTransitionStates::analyze(TpsTrajectory& traj)
{
	if (_method == STEPWISE) {
		analyzeLinear(traj);
	}
	else if (_method == BISECTION) {
		analyzeBisection(traj);
	}
}
