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
 *  TpsTrajectoryEnsemble.cpp
 *  tpsapi
 *
 *  Created by askeys on 11/8/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrajectoryEnsemble.h"
#include "TpsTrajectory.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

int TpsTrajectoryEnsemble::_s_unique_ID = 0;

TpsTrajectoryEnsemble::TpsTrajectoryEnsemble(
	TpsTrajectory& traj, int unique_ID, bool make_dir)
: _prototype_trajectory(traj), _uniqe_ID(unique_ID), _make_directories(make_dir)
{
	if (_uniqe_ID == DEFAULT) {
		_uniqe_ID = _s_unique_ID++;
	}
	std::ostringstream os;
	os << "mkdir ensemble" << _uniqe_ID;
    if (_make_directories) {
        system(os.str().c_str());
    }
}

TpsTrajectoryEnsemble::~TpsTrajectoryEnsemble()
{
	std::map<int, TpsTrajectory*>::iterator i;
	for (i=_trajectory.begin(); i!=_trajectory.end(); ++i) {
		delete i->second;
	} 
}

TpsTrajectory& TpsTrajectoryEnsemble::getTrajectory(int number)
{
	TpsTrajectory* traj;
	if (_trajectory.find(number) == _trajectory.end()) {
		traj = _prototype_trajectory.clone();
		traj->setID(number);
		traj->setFactoryID(_uniqe_ID);
		_trajectory[number] = traj;
		std::ostringstream os;
		os << "ensemble" << _uniqe_ID << "/trajectory" << number;
		//std::cerr << os.str().c_str() << std::endl;
		if (_make_directories) {
            mkdir(os.str().c_str(), S_IRWXU);
		}
        //std::cerr << errno << std::endl;
		//system(os.str().c_str());
	}
	else {
		traj = _trajectory[number];
	}
	return *traj;
}

TpsTrajectory& TpsTrajectoryEnsemble::getFirstTrajectory()
{
	assert (_trajectory.size() > 0);
	return *(_trajectory.begin()->second);
}

TpsTrajectory& TpsTrajectoryEnsemble::getLastTrajectory()
{
	assert (_trajectory.size() > 0);
	return *(_trajectory.rbegin()->second);
}

void TpsTrajectoryEnsemble::eraseTrajectory(int trajectory_id)
{
    eraseTrajectories(trajectory_id, trajectory_id);
}

void TpsTrajectoryEnsemble::eraseTrajectories(int min, int max)
{
	std::map<int, TpsTrajectory*>::iterator i;
	std::vector<int> key;
	for (i=_trajectory.begin(); i!=_trajectory.end(); ++i) {
		if (i->first >= min && i->first <= max) {
			//std::cerr << i->first <<"\t" << min <<"\t" << max << std::endl;
			i->second->clear();
			delete i->second;
			key.push_back(i->first);
		}
	}
	for (unsigned int j=0; j<key.size(); j++) {
		_trajectory.erase(key[j]);

		std::ostringstream os;
		os << "ensemble" << _uniqe_ID << "/trajectory" << key[j];
        if (_make_directories) {
            rmdir(os.str().c_str());
        }
	}
}

std::map <int, TpsTrajectory*>& TpsTrajectoryEnsemble::getTrajectoryList()
{
	return _trajectory;
}

unsigned int TpsTrajectoryEnsemble::getNumberOfTrajectories()
{
	return _trajectory.size();
}

void TpsTrajectoryEnsemble::moveTrajectory(int old_key, int new_key)
{
	assert(_trajectory.find(old_key) != _trajectory.end());
	
	TpsTrajectory& old_traj = getTrajectory(old_key);
	TpsTrajectory& new_traj = getTrajectory(new_key);
	new_traj.copy(old_traj);
	
	_trajectory[old_key]->clear();
	delete _trajectory[old_key];
	_trajectory.erase(old_key);
}

void TpsTrajectoryEnsemble::saveTrajectories(const char* filename)
{
	std::ofstream os(filename);
	assert(os.good());
	std::map<int, TpsTrajectory*>::iterator i;

	for (i=_trajectory.begin(); i!=_trajectory.end(); ++i) {
		if (i->first >= 0) { 
			i->second->print(os);
		}
	}
	os.close();
}

void TpsTrajectoryEnsemble::loadTrajectories(const char* filename)
{
	std::ifstream is(filename);
	while (1) {
		int traj_num;
		is >> traj_num;
		if (is.fail()) {
			break;
		}
		TpsTrajectory& traj = getTrajectory(traj_num);
		traj.read(is);
	}
	is.close();
}
