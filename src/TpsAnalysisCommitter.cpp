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

#include "TpsAnalysisCommitter.h"
#include "TpsSimulationPluginAtomic.h"
#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include "TpsOrderParameter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <limits>
#include <cassert>

TpsAnalysisCommitter::TpsAnalysisCommitter(TpsOrderParameter& op, 
	TpsTimesliceModifier& vel_dist, int len_traj, const char* filename, 
	int interval)
:	
	_op_aggregate(_default_op_aggregate),
	_velocity_distribution(vel_dist),
	_filename(filename),
	_times(1),
	_output_interval(interval),
	_bin_size(std::vector<double> (1, 0.1)),
	_length_fleeting_traj(len_traj),
	_n_fleeting_traj(10),
	_hist_min(std::vector<double>(1, 1e100)),
	_hist_max(std::vector<double>(1, -1e100)),
	_use_shortcut(false)
{
	_op_aggregate.addOrderParameter(op);
}

TpsAnalysisCommitter::TpsAnalysisCommitter(TpsOrderParameter& op, int len_traj,
	const char* filename, int interval)
:	
	_op_aggregate(_default_op_aggregate),
	_velocity_distribution(_default_velocity_distribution),
	_filename(filename),
	_times(1),
	_output_interval(interval),
	_bin_size(std::vector<double> (1, 0.1)),
	_length_fleeting_traj(len_traj),
	_n_fleeting_traj(10),
	_hist_min(std::vector<double>(1, 1e100)),
	_hist_max(std::vector<double>(1, -1e100)),
	_use_shortcut(false),
	_n_tiny_timestep(0),
	_size_tiny_timestep(-1.0)
{
	_op_aggregate.addOrderParameter(op);
}

TpsAnalysisCommitter::TpsAnalysisCommitter(TpsOrderParameterAggregate& op, 
	TpsTimesliceModifier& vel_dist, int len_traj, const char* filename, 
	int interval)
:	
	_op_aggregate(op),
	_velocity_distribution(vel_dist),
	_filename(filename),
	_times(1),
	_output_interval(interval),
	_length_fleeting_traj(len_traj),
	_n_fleeting_traj(10),
	_use_shortcut(false),
	_n_tiny_timestep(0),
	_size_tiny_timestep(-1.0)
{
	_bin_size = 
		std::vector<double> (_op_aggregate.getNumberOfOrderParameters(), 0.1);
	_hist_min = 
		std::vector<double>(_op_aggregate.getNumberOfOrderParameters(), 1e100);
	_hist_max = 
		std::vector<double>(_op_aggregate.getNumberOfOrderParameters(), -1e100);
}

TpsAnalysisCommitter::TpsAnalysisCommitter(TpsOrderParameterAggregate& op, 
	int len_traj, const char* filename, int interval)
:	
	_op_aggregate(op),
	_velocity_distribution(_default_velocity_distribution),
	_filename(filename),
	_times(1),
	_output_interval(interval),
	_length_fleeting_traj(len_traj),
	_n_fleeting_traj(10),
	_use_shortcut(false),
	_n_tiny_timestep(0),
	_size_tiny_timestep(-1.0)
{
	_bin_size = 
		std::vector<double> (_op_aggregate.getNumberOfOrderParameters(), 0.1);
	_hist_min = 
		std::vector<double>(_op_aggregate.getNumberOfOrderParameters(), 1e100);
	_hist_max = 
		std::vector<double>(_op_aggregate.getNumberOfOrderParameters(), -1e100);
}

TpsAnalysisCommitter::~TpsAnalysisCommitter()
{
	writeCommitter();
}

void TpsAnalysisCommitter::setFilename(const char* filename)
{
	_filename = filename;
}

void TpsAnalysisCommitter::setOutputInterval(int output_interval)
{
	_output_interval = output_interval;
}

void TpsAnalysisCommitter::setTinyTimesteps(int nstep, double stepsize)
{
	_n_tiny_timestep = nstep;
	_size_tiny_timestep = stepsize;
}

void TpsAnalysisCommitter::setBinSize(double bin_size)
{
	int nop = _op_aggregate.getNumberOfOrderParameters();
	_bin_size = std::vector<double> (nop, bin_size);
}

void TpsAnalysisCommitter::setUseShortcut(bool b)
{
	_use_shortcut = b;
}

void TpsAnalysisCommitter::setUseFullPath(bool b)
{
	_use_shortcut = !b;
}

void TpsAnalysisCommitter::setBinSize(std::vector<double>& bin_size)
{
	_bin_size = bin_size;
	unsigned int nop = _op_aggregate.getNumberOfOrderParameters();
	while (_bin_size.size() < nop) {
		_bin_size.push_back(_bin_size[bin_size.size()-1]);
	}
}

void TpsAnalysisCommitter::writeCommitter()
{
	std::ofstream os(_filename.c_str());

	using std::map;
	using std::vector;
	
	map<vector<double>, double>::iterator ii;
	for (ii=_hB.begin(); ii!=_hB.end(); ++ii) {
		std::vector<double> q_i = ii->first;
		for (unsigned int k=0; k<q_i.size(); k++) {
			os << q_i[k] <<"\t";
		}
		os << ii->second / _count[ii->first] <<"\n";
	}
	os.close();
}

void TpsAnalysisCommitter::setFleetingTrajectoryLength(int length)
{
	_length_fleeting_traj = length;
}

double TpsAnalysisCommitter::compute(TpsTrajectory& traj, int i, int ntraj)
{		
	if (_use_shortcut) {
		return computeShortcut(traj, i, ntraj);
	}
	else {
		return computeFullPath(traj, i, ntraj);
	}
}

void TpsAnalysisCommitter::analyze(TpsTrajectory& traj)
{		
	if (_use_shortcut) {
		analyzeShortcut(traj);
	}
	else {
		analyzeFullPath(traj);
	}
}

/**
\param traj is a trajectory
\param i is the index of the timeslice to compute the committer for
\param ntraj is the number of trajectories to run
\warning should only be used for certain order parameters (see explaination)

This function computes the standard committer by using the shortcut method.
We define the "standard committer" as the fraction of the time a fleeting
trajectory tends toward the reactant state.  In this scheme we save only two 
points along the test trajectory: the initial point (i.e., the timeslice "i" 
that we are computing the commiter for) and the endpoint, obtained by 
propogating the trajectory forward in time.  The order parameter therefore 
only has access to information at the trial point and the endpoint.  This is 
sufficient for some order parameters, but if the order parameter is a function 
of the full path, (e.g., a relative order parameter based on the difference 
between the start and end point) this method will give incorrect answers.
*/
double TpsAnalysisCommitter::computeShortcut(TpsTrajectory& traj, int i, int ntraj)
{
    if (ntraj <= 0) {
        ntraj = _n_fleeting_traj;
    }
	int n_micro = traj.getNumberOfTimeslices();
	assert(n_micro > 0);
	assert(i < n_micro);
	assert(i >= 0);
	
	if (_length_fleeting_traj <= 0) {
		_length_fleeting_traj = traj.getLength() / 2;
	}
	
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();

	TpsTrajectory temp_trajectory(sim);
	TpsTimeslice x0 = traj.getTimeslice(0);
	sim.readRestart(traj.getTimeslice(0).getFilename().c_str());
	sim.writeRestart("committer_begin.restart");
	x0.setFilename("committer_begin.restart");
	temp_trajectory.pushBack(x0);
	
	double hB = 0.0;
	
	for (int j=0; j<ntraj; j++) {
	
		sim.readRestart(traj.getTimeslice(i).getFilename().c_str());
		_velocity_distribution.modify(sim);

		if (_n_tiny_timestep > 0) {
			takeTinyTimesteps(sim);
		}

		sim.run(_length_fleeting_traj);
					
		TpsTimeslice xf;
		xf.setFilename("committer_end.restart");
		sim.writeRestart("committer_end.restart");
		temp_trajectory.pushBack(xf);
		
		if(_op_aggregate.hB(temp_trajectory)) {
			hB ++;
		}
		temp_trajectory.popBack();
	}		
	return hB / (double)ntraj;
}

/**
\param traj is a trajectory
\param i is the index of the timeslice to compute the committer for
\param ntraj is the number of trajectories to run
\warning should only be used for certain order parameters (see explaination)

This function computes the alternate committer using the "full path" method.
Here, we define the committer in the same way as B. Peters, et. al., by 
computing the fraction of the times a timeslice leads to a valid transition
path when velocities are drawn at random.  In this scheme, we save points along
a trajectory as specified by the trajectory.  Therefore, the order parameter
recieves a full path as an input.  This is the preferred method for order 
parameters that are a function of the full path, rather than just the endpoint.
*/
double TpsAnalysisCommitter::computeFullPath(TpsTrajectory& traj, int i, int ntraj)
{
    if (ntraj <= 0) {
        ntraj = _n_fleeting_traj;
    }
	int n_micro = traj.getNumberOfTimeslices();
	assert(n_micro > 0);
	assert(i >= 0);
	assert(i < n_micro);
	
	TpsTrajectory* temp_traj = traj.clone();
	temp_traj->setID(-10004);

	int t_shot = traj.getTimeslice(i).getTimesteps();
	int t_half_traj = temp_traj->getLength()/2;

	double hB = 0.0;
	
	for (int j=0; j<ntraj; j++) {
		temp_traj->clear();
		temp_traj->shootFrom(traj, i, _velocity_distribution, 
			_n_tiny_timestep, _size_tiny_timestep);

		temp_traj->setTimeWindow(t_shot-t_half_traj, t_shot+t_half_traj);
		temp_traj->extend();

		if (_op_aggregate.isTransitionPath(*temp_traj)) {
			hB++;
		}
		else {
			//try backward path
			temp_traj->invert();
			if (_op_aggregate.isTransitionPath(*temp_traj)) {
				hB++;
			}
		}
	}		
	
	delete temp_traj;
	return hB / (double)ntraj;
}

void TpsAnalysisCommitter::analyzeFullPath(TpsTrajectory& traj)
{
	int n_micro = traj.getNumberOfTimeslices();
	assert(n_micro > 0);
	if (_length_fleeting_traj <= 0) {
		_length_fleeting_traj = traj.getLength() / 2;
	}
	
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();

	TpsTrajectory* temp_trajectory = traj.clone();
	temp_trajectory->setID(-10004);
	int start_time = traj.getTimeslice(0).getTimesteps();
	int t0 = traj.getTimeslice(0).getTimesteps();
	
	for (int i=0; i<n_micro; i++) {
		for (int j=0; j<_n_fleeting_traj; j++) {
			int t = traj.getTimeslice(i).getTimesteps();
			temp_trajectory->copy(traj, start_time, t);
			int last = temp_trajectory->getNumberOfTimeslices() - 1;
			
			sim.readRestart(
				temp_trajectory->getTimeslice(last).getFilename().c_str());
			_velocity_distribution.modify(sim);
			
			if (_n_tiny_timestep > 0) {
				takeTinyTimesteps(sim);
			}

			sim.writeRestart(
				temp_trajectory->getTimeslice(last).getFilename().c_str());

			//leave this inside the loop in case pB depends on velocities:
			std::vector<double> q;
			std::vector<TpsOrderParameter*>& op = 
				_op_aggregate.getOrderParameterList();
			for (unsigned int k=0; k<op.size(); k++) {
				double q_k = op[k]->evaluate(*temp_trajectory, last);
				double q_hist = rint(q_k / _bin_size[k]) * _bin_size[k];
				_hist_max[k] = std::max(_hist_max[k], q_hist);
				_hist_min[k] = std::min(_hist_min[k], q_hist);
				q.push_back(q_hist);				
			}			
						
			if (_hB.find(q) == _hB.end()) {
				_hB[q] = 0.0;
				_count[q] = 0.0;
			} 
																																	
			temp_trajectory->setTimeWindow(t0, t + _length_fleeting_traj);
			temp_trajectory->extendForward();
			
			if(_op_aggregate.hB(*temp_trajectory)) {
				_hB[q] += traj.getWeight();
			}
			_count[q] += traj.getWeight();
			temp_trajectory->clear();
		}		
	}
	
	delete temp_trajectory;
	
	if (_output_interval > 0) {
		if (_times++ % _output_interval == 0) { 
			writeCommitter();
		}
	}
}

void TpsAnalysisCommitter::analyzeShortcut(TpsTrajectory& traj)
{
	int n_micro = traj.getNumberOfTimeslices();
	assert(n_micro > 0);
	if (_length_fleeting_traj <= 0) {
		_length_fleeting_traj = traj.getLength() / 2;
	}
	
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();

	TpsTrajectory temp_trajectory(sim);
	TpsTimeslice x0 = traj.getTimeslice(0);
	sim.readRestart(traj.getTimeslice(0).getFilename().c_str());
	sim.writeRestart("committer_begin.restart");
	x0.setFilename("committer_begin.restart");
	temp_trajectory.pushBack(x0);
	
	for (int i=0; i<n_micro; i++) {
		for (int j=0; j<_n_fleeting_traj; j++) {
		
			sim.readRestart(traj.getTimeslice(i).getFilename().c_str());
			_velocity_distribution.modify(sim);

			if (_n_tiny_timestep > 0) {
				takeTinyTimesteps(sim);
			}

			TpsTimeslice xm;
			xm.setFilename("committer_mid.restart");
			sim.writeRestart("committer_mid.restart");
			temp_trajectory.pushBack(xm);
			
			int last = temp_trajectory.getNumberOfTimeslices() - 1;
			std::vector<double> q;
			std::vector<TpsOrderParameter*>& op = 
				_op_aggregate.getOrderParameterList();
			for (unsigned int k=0; k<op.size(); k++) {
				double q_k = op[k]->evaluate(temp_trajectory, last);
				double q_hist = rint(q_k / _bin_size[k]) * _bin_size[k];
				_hist_max[k] = std::max(_hist_max[k], q_hist);
				_hist_min[k] = std::min(_hist_min[k], q_hist);
				q.push_back(q_hist);				
			}			
			temp_trajectory.popBack();
						
			if (_hB.find(q) == _hB.end()) {
				_hB[q] = 0.0;
				_count[q] = 0.0;
			} 
						
			sim.run(_length_fleeting_traj);
						
			TpsTimeslice xf;
			xf.setFilename("committer_end.restart");
			sim.writeRestart("committer_end.restart");
			temp_trajectory.pushBack(xf);
			
			if(_op_aggregate.hB(temp_trajectory)) {
				_hB[q] += traj.getWeight();
			}
			_count[q] += traj.getWeight();
			temp_trajectory.popBack();
		}		
	}
	
	if (_output_interval > 0) {
		if (_times++ % _output_interval == 0) { 
			writeCommitter();
		}
	}
}

void TpsAnalysisCommitter::takeTinyTimesteps(TpsSimulationPlugin& sim)
{
	try {
		TpsSimulationPluginAtomic& sim_atomic 
			= dynamic_cast<TpsSimulationPluginAtomic&>(sim);
		double timestep = sim_atomic.getTimestep();
		if (_size_tiny_timestep <= 0) {
			_size_tiny_timestep = timestep / 10.0;
		}
		sim_atomic.setTimestep(_size_tiny_timestep);
		sim_atomic.run(_n_tiny_timestep);
		sim_atomic.setTimestep(timestep);
	}
	catch (std::exception& e) {
		int cast_to_atomic_simulation = 0;
		assert(cast_to_atomic_simulation);
	}
}
