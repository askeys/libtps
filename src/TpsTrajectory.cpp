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

#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include "TpsRNG.h"
#include "TpsSimulationPlugin.h"
#include "TpsSimulationPluginAtomic.h"
#include "TpsTimesliceModifier.h"
#include "TpsTrajectoryIterator.h"
#include "TpsTimesliceModifier.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>

bool TpsTrajectory::_created_tmp_directory = false;

/**
\param sim is the simulation engine used to create the trajectory
\param path_length is the maximum length of the path
\param step_size is the parameter that determines the length between saved 
    states along the trajectory.  For a uniform step trajectory, the step_size 
    is constant.  For a random step trajectory, step_size is the maximum random
    step, etc.
*/
TpsTrajectory::TpsTrajectory( 
	TpsSimulationPlugin& sim, int path_length, int step_size) :
	_weight(1.0),
	_t_length(path_length),
	_t_step(step_size),
	_t_shift(0),
	_trajectory_number(0), 
	_sim(sim),
	_factory_ID(-1)
{
	if (!_created_tmp_directory) {
		system("mkdir tmp");
		_created_tmp_directory = true;
	}
}

/**
\brief Copy constructor
\param traj is the trajectory to copy
\note the array of timeslices is note copied.  This is to avoid the large 
    overhead of copying and deleting this information if it is note needed.  The 
    copy function 
\note The ensemble weight of the new trajectory is not copied, it is set to 1
\note Since the trajectory is not created by a TpsTrajectoryFactory, the
    _factory_ID is set to -1 (indicates that the trajectory is not associated 
    with any factory).

Specifically copies _t_length, _t_step, _t_shift, and _sim.
Parameter _trajectory_number is set to -_trajectory_number.  
Parameter _factory_ID is set to -1.
*/
TpsTrajectory::TpsTrajectory(TpsTrajectory& traj) :
	_weight(1.0),
	_t_length(traj._t_length),
	_t_step(traj._t_step),
	_t_shift(traj._t_shift),
	_trajectory_number(-traj._trajectory_number), 
	_sim(traj._sim),
	_factory_ID(-1)
{
	if (!_created_tmp_directory) {
		system("mkdir tmp");
		_created_tmp_directory = true;
	}
}

/**
\brief Clone constructor
\note This function calls the copy constructor
\note The clone constructor is different from the copy constructor in that it 
creates an object of the same type as the cloned trajectory.
  
This function is typically used by TpsTrajectoryFactory to make many clones of 
the prototype trajectory that is passed to it.
*/
TpsTrajectory* TpsTrajectory::clone()
{
	int this_virtual_function_was_overridden = 0;
	assert(this_virtual_function_was_overridden);
	return new TpsTrajectory(*this);
}

/**
\brief Destructor
\note Does nothing
*/
TpsTrajectory::~TpsTrajectory()
{
	//clear();
}

/**
\brief Get the number of timeslices
\return the number of TpsTimeslices in the _timelice array
*/
int TpsTrajectory::getNumberOfTimeslices()
{
	return _timeslice.size();
}

/**
\brief Get a particular timeslice
\return the TpsTimesclice at position "i" in the _timelice array
*/
TpsTimeslice& TpsTrajectory::getTimeslice(unsigned int i)
{
	assert(i<_timeslice.size());
	return _timeslice[i];
}

/**
\brief Get the last timeslice
\return the TpsTimeslice at the end of the _timelice array
*/
TpsTimeslice& TpsTrajectory::getLastTimeslice()
{
	assert(_timeslice.size() > 0);
	return _timeslice[_timeslice.size()-1];
}


/**
\brief Get a random timeslice
\param rng is the random number generator to use to randomly pick the timesclice
\return the TpsTimesclice at a random position in the _timelice array
*/
TpsTimeslice& TpsTrajectory::getRandomTimeslice(TpsRNG& rng)
{
	assert(_timeslice.size() > 0);
	int index = rng.randInt() % _timeslice.size();
	return _timeslice[index];
}

/**
\brief Append a timeslice to the front of the trajectory
\param m is the timesclice to append to the trajectory
*/		
void TpsTrajectory::pushFront(TpsTimeslice& m)
{
	_timeslice.push_front(m);
}

/**
\brief Append a timeslice to the back of the trajectory
\param m is the timesclice to append to the trajectory
*/		
void TpsTrajectory::pushBack(TpsTimeslice& m)
{
	_timeslice.push_back(m);
}

/**
\brief Set the endpoints of the trajectory
\param t_start is the low endpoint
\param t_end is the high endpoint
*/		
void TpsTrajectory::setTimeWindow(int t_start, int t_end)
{
	_t_shift = t_start;
	_t_length = t_end - _t_shift;
}

/**
\brief set the desired length of the trajectory in timesteps
\param length is the is the desired number of timesteps that will be spanned by 
    the trajectory after the trajectory has been run
*/
void TpsTrajectory::setTargetLength(int length)
{
	_t_length = length;
}

/**
\brief get the desired length of the trajectory in timesteps
\return the is the desired number of timesteps that will be spanned by 
    the trajectory after the trajectory has been run
*/
int TpsTrajectory::getTargetLength()
{
	return _t_length;
}

/**
\brief Get the actual length of the trajectory
\return the time window of the trajectory
\note this is different from getTargetLength() in that it returns the actual
    length of the trajectory, not the desired length.  Therefore, a trajectory 
    that has not yet been extended will have a length of zero.
*/
int TpsTrajectory::getLength()
{
	return (_timeslice.back().getTimesteps() -  
		_timeslice.front().getTimesteps());
}

/**
\param step is the step size of the trajectory _t_step
\note the step size has a different meaning for the different trajectory types.
    For a uniformily spaced trajectory, the step size is the time window between
    save points.  For a randomly spaced trajectory, the step size is the max 
    time window between save points.  For a trajectory with a user-defined step,
    the step size is meaningless.
*/
void TpsTrajectory::setStepSize(int step) 
{
	_t_step = step;
}

/**
\brief start the trajectory by saving an initial TpsTimeslice from the current
    state of the underlying simulation engine
*/
void TpsTrajectory::start()
{
	std::string filename = nameFile(0);
	TpsTimeslice m(filename.c_str(), 0);
	_sim.writeRestart(filename.c_str());
	_timeslice.push_back(m);
}

/**
\brief Extend the trajectory forward in time by a specified number of timeslices
\param nstep is the number of timeslices (default is TO_ENDPOINT, which extends 
    to trajectory min endpoint)
\note This is an interface function that must be overridden by all derived 
    trajectories
*/
void TpsTrajectory::extendForward(int)
{
}

/**
\brief Extend the trajectory forward in time by a specified number of timeslices
\param nstep is the number of timeslices (default is TO_ENDPOINT, which extends 
    to trajectory max endpoint)
\note This is an interface function that must be overridden by all derived 
    trajectories
*/
void TpsTrajectory::extendBackward(int)
{
}

/**
\brief Update the target path length based on the current path length 
*/
void TpsTrajectory::updatePathLength()
{
	assert(_timeslice.size() > 0);
	_t_shift = _timeslice[0].getTimesteps();
	_t_length = _timeslice.back().getTimesteps() - _t_shift; 
}

/**
\brief Extend the trajectory forward, saving timeslices at the specified points
\param t_save is a vector of times for which to save restart files
*/
void TpsTrajectory::extendForward(std::vector<int>& t_save)
{
	assert(_timeslice.size() > 0);
	if (t_save.size() == 0) {
		return;
	}

	int t = _timeslice.back().getTimesteps();
	_sim.readRestart(_timeslice.back().getFilename().c_str());
	
	std::sort(t_save.begin(), t_save.end());

	bool any = false;
	unsigned int i;
	for (i=0; i<t_save.size(); i++) {
		if (t_save[i] > t) {
			any = true;
			break;
		}
	}
	if (!any) {
		return;
	}
	
	for (i=i; i<t_save.size(); i++) {
		int t_step = t_save[i] - t;
		TpsTimeslice m;
		_sim.run(t_step);
		t += t_step;
		m.setTimesteps(t);
		std::string filename = nameFile(t);
		m.setFilename(filename.c_str());	
		_timeslice.push_back(m);
		_sim.writeRestart(filename.c_str());
	}
}

/**
\brief Extend the trajectory backward, saving timeslices at the specified points
\param t_save is a vector of times for which to save restart files
*/
void TpsTrajectory::extendBackward(std::vector<int>& t_save)
{
	assert(_timeslice.size() > 0);
	if (t_save.size() == 0) {
		return;
	}
	
	int t = _timeslice.front().getTimesteps();
	_sim.readRestart(_timeslice.front().getFilename().c_str());
	
	std::sort(t_save.begin(), t_save.end());
	
	bool any = false;
	int i;
	for (i = t_save.size()-1; i>=0; i--) {
		if (t_save[i] < t) {
			any = true;
			break;
		}
	}
	
	if (!any) {
		return;
	}
	
	for (i=i; i>=0; i-=1) {
		int t_step = t - t_save[i];
		TpsTimeslice m;
		_sim.invertVelocities();
		_sim.run(t_step);
		_sim.invertVelocities();
		t = t_save[i];
		m.setTimesteps(t);
		std::string filename = nameFile(t);
		m.setFilename(filename.c_str());	
		_timeslice.push_front(m);
		_sim.writeRestart(filename.c_str());
	}
}

/**
\brief Extend the trajectory forward and backwards in time
*/
void TpsTrajectory::extend()
{
	assert(_t_step >= 1.0);
	
	extendBackward();
	extendForward();
}

void TpsTrajectory::clear()
{
	for (unsigned int i=0; i<_timeslice.size(); i++) {
		_sim.freeRestart(_timeslice[i].getFilename().c_str());
	}
	_timeslice.clear();
}

void TpsTrajectory::invert(bool invert_velocities)
{
	std::deque<TpsTimeslice> temp = _timeslice;
	int nmm1 = _timeslice.size()-1;
	int tmax = _timeslice[nmm1].getTimesteps();
	_t_shift = 0;
	_timeslice.clear();
	for (unsigned int i=0; i<temp.size(); i++) {
		if (invert_velocities) {
			_sim.readRestart(temp[nmm1-1].getFilename().c_str());
			_sim.invertVelocities();
			_sim.writeRestart(temp[nmm1-1].getFilename().c_str());
		}
		_timeslice.push_back(temp[nmm1-i]);
		_timeslice[i].setTimesteps(_timeslice[i].getTimesteps()-tmax);
	}
}

/**
\brief Insert a timeslice, maintains sequential time of trajectory
*/
void TpsTrajectory::insert(TpsTimeslice& m)
{
	//insert 0
	if (_timeslice.size() == 0) {
		_timeslice.push_back(m);
		return;
	}
	
	int t = m.getTimesteps();
	int t0 = _timeslice[0].getTimesteps();
	int tf = _timeslice[_timeslice.size()-1].getTimesteps();
	
	//insert front
	if (t <= t0) {
		_timeslice.push_front(m);
		return;
	}

	//insert back
	if (t >= tf) {
		_timeslice.push_back(m);
		return;
	}
	
	//insert middle
	unsigned int sm1 = _timeslice.size()-1;
	std::deque<TpsTimeslice>::iterator iter = _timeslice.begin();

	for (unsigned int i=0; i<sm1; i++) {
		int ti = _timeslice[i].getTimesteps();
		int tip1 = _timeslice[i+1].getTimesteps();
		++iter;
		if (t > ti && t < tip1) {
			_timeslice.insert(iter, m);
			return;
		}
	}
}

/**
\brief Return the index of a given timeslice (-1 if not found)
*/
int TpsTrajectory::find(TpsTimeslice& m)
{
	for (unsigned int i=0; i<_timeslice.size(); i++) {
		if (_timeslice[i].getTimesteps() == m.getTimesteps() &&
			_timeslice[i].getFilename() == m.getFilename() ) {
			return i;
		}
	}
	return -1;
}

/**
\bug Design flaw? Does not actually erase the timeslice from memory, just from
    the trajectory array
*/
void TpsTrajectory::erase(TpsTimeslice& m)
{
	if (_timeslice.size() == 0) {
		return;
	}

	int t = m.getTimesteps();
    std::string filename = m.getFilename();
    
    for (unsigned int i=0; i<_timeslice.size(); i++) {
        if (_timeslice[i].getTimesteps() == t &&
            _timeslice[i].getFilename() == filename) {
            _timeslice.erase(_timeslice.begin()+i);
            return;
        }
    }
    std::cerr << "TpsTrajectory: WARNING: attempt to erase ";
    std::cerr << "non-existant microstate " << m.getFilename() << ", ";
    std::cerr << "timestep" << t << "\n";
}

void TpsTrajectory::setSimulationPlugin(TpsSimulationPlugin& sim)
{
	_sim = sim;
}

TpsSimulationPlugin& TpsTrajectory::getSimulationPlugin()
{
	return _sim;
}

TpsTimeslice& TpsTrajectory::newTimeslice()
{
	TpsTimeslice m;
	_timeslice.push_back(m);
	return _timeslice[_timeslice.size()-1];
}

void TpsTrajectory::reduceSize(int size)
{
	int csize = _timeslice.size();
	if (csize <= size) {
		return;
	}
	
	int skip = csize / size;
	std::deque<TpsTimeslice> temp = _timeslice;
	_timeslice.clear();
	
	for (int i=0; i<csize; i++) {
		if (i%skip == 0) {
			_timeslice.push_back(temp[i]);
		}
		else {
			_sim.freeRestart(temp[i].getFilename().c_str());
		}

	}
}

double TpsTrajectory::getWeight()
{
	return _weight;
}

void TpsTrajectory::incrementWeight()
{
	_weight++;
}

void TpsTrajectory::setFactoryID(int factory_ID)
{
	_factory_ID = factory_ID;
}

std::string TpsTrajectory::nameFile(int timesteps)
{
	std::ostringstream filename;
	if (_factory_ID == -1) {
		filename << "tmp/" << _trajectory_number 
					<< "." << timesteps << ".restart";	
	}
	else {
		filename << "ensemble" << _factory_ID << "/trajectory" 
				<< _trajectory_number << "/" << timesteps << ".restart";
	}

	return filename.str();
}

void do_tiny_steps(TpsSimulationPlugin& sim, int nstep, double stepsize)
{
	try {
		TpsSimulationPluginAtomic& sim_atomic 
			= dynamic_cast<TpsSimulationPluginAtomic&>(sim);
		double timestep = sim_atomic.getTimestep();
		if (stepsize <= 0) {
			stepsize = timestep / 10.0;
		}
		sim_atomic.setTimestep(stepsize);
		sim_atomic.run(nstep);
		sim_atomic.invertVelocities();
		sim_atomic.run(nstep);
		sim_atomic.invertVelocities();
		sim_atomic.setTimestep(timestep);
	}
	catch (std::exception& e) {
		int cast_to_atomic_simulation = 0;
		assert(cast_to_atomic_simulation);
	}
}

void TpsTrajectory::shootFrom(
	TpsTrajectory& trj, TpsRNG& rng, TpsTimesliceModifier& perturbation, 
	int ntinystep, double stepsize)
{
	clear();
	_t_length = trj._t_length;
	_t_step = trj._t_step;
	_t_shift = trj._t_shift;

	TpsTimeslice m = trj.getRandomTimeslice(rng);
	_sim.readRestart(m.getFilename().c_str());
	
	while(!(perturbation.modify(_sim))) {
		trj.incrementWeight();
	}

	if (ntinystep > 0) {
		do_tiny_steps(_sim, ntinystep, stepsize);
	}
		
	std::string filename = nameFile(m.getTimesteps());
	m.setFilename(filename.c_str());
	_sim.writeRestart(filename.c_str());
	_timeslice.push_back(m);	
}

void TpsTrajectory::shootFrom(
	TpsTrajectory& trj, TpsRNG& rng)
{
	clear();
	_t_length = trj._t_length;
	_t_step = trj._t_step;
	_t_shift = trj._t_shift;
	
	TpsTimeslice m = trj.getRandomTimeslice(rng);
	_sim.readRestart(m.getFilename().c_str());	
	std::string filename = nameFile(m.getTimesteps());
	m.setFilename(filename.c_str());
	_sim.writeRestart(filename.c_str());
	_timeslice.push_back(m);	
}

void TpsTrajectory::shootFrom(
	TpsTrajectory& trj, int index, TpsTimesliceModifier& vdist, 
	int ntinystep, double stepsize)
{
	clear();
	_t_length = trj._t_length;
	_t_step = trj._t_step;
	_t_shift = trj._t_shift;
	
	TpsTimeslice m = trj.getTimeslice(index);
	
	_sim.readRestart(m.getFilename().c_str());	
	vdist.modify(_sim);
	if (ntinystep > 0) {
		do_tiny_steps(_sim, ntinystep, stepsize);
	}
	std::string filename = nameFile(m.getTimesteps());
	m.setFilename(filename.c_str());
	_sim.writeRestart(filename.c_str());
	_timeslice.push_back(m);		
}

std::deque<TpsTimeslice>& TpsTrajectory::getTimeslices()
{
	return _timeslice;
}

/**
\brief Copy a trajectory
\param tmin is the minimimum timeslice of the copy
\param tmax is the maximimum timeslice of the copy
\note This function makes a copy of all timesclices as well as copying the 
    trajectory parameters

Specifically, this function copies the _timeslice array as well as _t_length,
_t_step, and _t_shift.  The timeslices are renamed according to the name of the
new trajectory.
*/
void TpsTrajectory::copy(TpsTrajectory& traj, int tmin, int tmax)
{
	_timeslice.clear();
	std::deque<TpsTimeslice>& timeslice = traj.getTimeslices();
	for (unsigned int i=0; i<timeslice.size(); i++) {
		TpsTimeslice m = timeslice[i];
		int t = m.getTimesteps();
		if (t < tmin || t > tmax) {
			continue;
		}
		std::string new_filename = nameFile(m.getTimesteps());
		_sim.copyRestart(
			timeslice[i].getFilename().c_str(), new_filename.c_str());
		m.setFilename(new_filename.c_str());
		_timeslice.push_back(m);
	}
	_t_length = traj._t_length;
	_t_step = traj._t_step;
	_t_shift = traj._t_shift;
}

int TpsTrajectory::getStepSize() const
{
	return _t_step;
}

void TpsTrajectory::shiftTimesteps(int timesteps)
{	
	_t_shift += timesteps;
	int t0 = _t_shift;
	int tf = _t_length + _t_shift;
	
	if (_timeslice.size() == 0) {
		return;
	}
	
	while (_timeslice[0].getTimesteps() < t0) {
		_sim.freeRestart(_timeslice[0].getFilename().c_str());
		_timeslice.pop_front();
		if (_timeslice.size() == 0) {
			return;
		}
	}

	if (_timeslice.size() == 0) {
		return;
	}

	while (_timeslice[_timeslice.size()-1].getTimesteps() > tf) {
		_sim.freeRestart(
			_timeslice[_timeslice.size()-1].getFilename().c_str());
		_timeslice.pop_back();
		if (_timeslice.size() == 0) {
			return;
		}
	}
}

int TpsTrajectory::getID()
{
	return _trajectory_number;
}

void TpsTrajectory::setID(int number)
{
	_trajectory_number = number;
}

void TpsTrajectory::popBack()
{
	assert(_timeslice.size() > 0);
	_sim.freeRestart(
		_timeslice[_timeslice.size()-1].getFilename().c_str());
	_timeslice.pop_back();
}

void TpsTrajectory::popFront()
{
	assert(_timeslice.size() > 0);
	_sim.freeRestart(_timeslice[0].getFilename().c_str());
	_timeslice.pop_front();
}

TpsTrajectoryIterator& TpsTrajectory::begin()
{	
	assert(_timeslice.size() > 0);
	_begin._traj_ptr = this;
	_begin._t = _timeslice[0].getTimesteps();
	_begin._restart_timeslice = 0;
	return _begin;
}

TpsTrajectoryIterator& TpsTrajectory::end()
{
	assert(_timeslice.size() > 0);
	_end._traj_ptr = this;
	_end._t = _timeslice[_timeslice.size()-1].getTimesteps();
	_end._restart_timeslice = getNumberOfTimeslices()-1;
	return _end;
}

TpsTrajectoryReverseIterator& TpsTrajectory::rbegin()
{	
	assert(_timeslice.size() > 0);
	_rbegin._traj_ptr = this;
	_rbegin._t = _timeslice[_timeslice.size()-1].getTimesteps();
	_rbegin._restart_timeslice = getNumberOfTimeslices()-1;
	return _rbegin;
}

TpsTrajectoryReverseIterator& TpsTrajectory::rend()
{
	assert(_timeslice.size() > 0);
	_rend._traj_ptr = this;
	_rend._t = _timeslice[0].getTimesteps();
	_rend._restart_timeslice = 0;
	return _rend;
}

void TpsTrajectory::print(std::ostream& os)
{
	os << _trajectory_number << "\t";
	os << _weight << "\t";
	os << _t_step << "\t";
	os << _t_length << "\t";
	os << _t_shift << "\t";
	os << _timeslice.size() << "\t";
	for (unsigned int i=0; i<_timeslice.size(); i++) {
		_timeslice[i].print(os);
	}
	os << "\n";
}

void TpsTrajectory::read(std::istream& is)
{
	//is >> _trajectory_number;
	is >> _weight;
	is >> _t_step;
	is >> _t_length;
	is >> _t_shift;
	int nmicro;
	is >> nmicro;
	
	for (int i=0; i<nmicro; i++) {
		TpsTimeslice m;
		m.read(is);
		_timeslice.push_back(m);
	}
}
