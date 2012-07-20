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

#ifndef TPSTRAJECTORY_H
#define TPSTRAJECTORY_H

#include "TpsTrajectoryIterator.h"
#include "TpsTrajectoryReverseIterator.h"

#include <vector>
#include <deque>
#include <string>
#include <limits.h>
#include <iostream>

class TpsTimeslice;
class TpsRNG;
class TpsSimulationPlugin;
class TpsTimesliceModifier;
class TpsTimesliceModifier;

/**
\brief Contains and manages sequence of TpsTimeslices, etc. (see below)
\ingroup core
\ingroup traj

The TpsTrajectory object has a variety of functions:

	-# Contains a list of TpsTimeslices (restart timeslices)
	-# Defines methods to perform shots, shifts, path inversions, etc. from
		other trajectories.  These methods are typically used by the 
		TpsTrialMove and TpsAnalysis classes to perform path sampling or 
		analyze the transition path ensemble.
	-# Understands how to extend itself forward and backward in time and
		when to save restart timeslices when doing so(see TpsTimeslice).
		
A trajectory may represent a whole transition path (e.g., in standard TPS) 
or a partial transition path (e.g., in TIS).  

Typically, a trajectory is given a starting timeslice (either from an 
existing trajectory or from scractch).  Given a starting point, the trajectory 
knows how to extend itself to the approriate size.

Subclasses of TpsTrajectory typically differ by the way that they save 
timeslices when extending forward or backward. For example, whereas
TpsTrajectoryUniformStep saves timeslices at linearly spaced intervals, 
TpsTrajectoryRandomStep saves timeslices at randomly-spaced intervals. 
To create a new TpsTrajectory object for saving timeslices at custom intervals, 
simply override the extend*() functions.  An even simpler way to do this is 
given by the TpsTrajectoryUserDefined class, which uses a list of timeslices 
for saving on construction.
*/
class TpsTrajectory
{
	public:
		TpsTrajectory(TpsSimulationPlugin&, int=-1, int=-1);
		TpsTrajectory(TpsTrajectory&);
		virtual ~TpsTrajectory();
		virtual TpsTrajectory* clone();
		
		virtual TpsTimeslice& getRandomTimeslice(TpsRNG&);
		TpsTimeslice& getTimeslice(unsigned int);
		int getNumberOfTimeslices();
		std::deque<TpsTimeslice>& getTimeslices();
		
		int find(TpsTimeslice&);												
		void insert(TpsTimeslice&);                                             
		void erase(TpsTimeslice&);												
		void popFront();
		void popBack();
		void pushFront(TpsTimeslice&);
		void pushBack(TpsTimeslice&);
		
		void start();
		virtual void extendForward(int nslice=TO_ENDPOINT);						
		virtual void extendBackward(int nslice=TO_ENDPOINT);					
		void extendForward(std::vector<int>&);									
		void extendBackward(std::vector<int>&);									
		virtual void extend();													
		void clear();

		void setSimulationPlugin(TpsSimulationPlugin&);
		TpsSimulationPlugin& getSimulationPlugin();
		TpsTimeslice& newTimeslice();

		void shootFrom(TpsTrajectory&, TpsRNG&, TpsTimesliceModifier&,
			int=0, double=1.0);
		void shootFrom(TpsTrajectory&, TpsRNG&);
		void shootFrom(TpsTrajectory&, int, TpsTimesliceModifier&, 
			int=0, double=1.0);
		
        TpsTimeslice& getLastTimeslice();
		void reduceSize(int);
		void incrementWeight();
		double getWeight();
		void setTargetLength(int);
		int getLength();
		int getTargetLength();
		void setTimeWindow(int, int);
		void setStepSize(int);
		int getStepSize() const;
		virtual void copy(TpsTrajectory&, int=INT_MIN, int=INT_MAX);
		virtual void shiftTimesteps(int);
		void setID(int);
		int getID();
		void print(std::ostream&);
		void read(std::istream&);
		void invert(bool=false);
		void setFactoryID(int);
		TpsTrajectoryIterator& begin();
		TpsTrajectoryIterator& end();
		TpsTrajectoryReverseIterator& rbegin();
		TpsTrajectoryReverseIterator& rend();
		
	protected:
		enum {TO_ENDPOINT=1000000};
		std::string nameFile(int);

		void updatePathLength();
		double _weight;
		int _t_length, _t_step, _t_shift;
		int _trajectory_number;
		std::deque<TpsTimeslice> _timeslice;
		TpsSimulationPlugin& _sim;
		TpsTrajectoryIterator _begin, _end;
		TpsTrajectoryReverseIterator _rbegin, _rend;
		int _factory_ID;
		static bool _created_tmp_directory;
};

#endif
