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
 *  TpsSimulationPluginMolSimAPI.h
 *  tpsapi
 *
 *  Created by askeys on 7/1/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#ifndef TPSSIMULATIONPLUGINMOLSIMAPI_H
#define TPSSIMULATIONPLUGINMOLSIMAPI_H

#include "TpsSimulationPluginAtomic.h"
#include <string>
#include <map>
#include <molsimapi/molsimapi.h>

/**
\brief uses The mol_sim_api as the underlying simulation engine
\ingroup sim
\bug Currently only works for with constant energy trial moves, since 
	computeHamiltonian() has no access to temperature, and has limited access
	to information about the system energy
\todo Find some way to get thermodynamics information 

To run a transition path sampling simulation using the mol_sim_api, the user 
must instantiate a mol_sim_api simulation. 
*/
class TpsSimulationPluginAtomicMolSimAPI : public TpsSimulationPluginAtomic
{
	public:
		TpsSimulationPluginAtomicMolSimAPI(
			IAPISimulation*, IAPIIntegrator*, IAPIPotentialMaster*, int=3);
		virtual ~TpsSimulationPluginAtomicMolSimAPI();
		
		//re-implemented from TpsSimulationPlugin:
		virtual void run(int nsteps=1);
		virtual void writeRestart(const char* filename);
		virtual void readRestart(const char* filename);
		virtual void freeRestart(const char* filename);
		virtual void copyRestart(const char* src_file, const char* dest_file);
		virtual double computeHamiltonian();
		double getExpectedTemperature();
		
		//re-implemented from TpsSimulationPluginAtomic:
		int getNumberOfAtoms();
		int getDimensions();
		void setTimestep(double);
		double getTimestep();		

		virtual void invertVelocities();
		virtual void callbackOnInvertVelocities();
		
		void copyTypesTo(std::vector<int>&);
		void copyBoxTo(BoxInfo&);
		void setVelocities(std::vector< std::vector<double> >&);
		void setPositions(std::vector< std::vector<double> >&);
		void copyPositionsTo(std::vector< std::vector<double> >&);
		void copyVelocitiesTo(std::vector< std::vector<double> >&);
		double computeKinetic();
		
		void setRestartMode(int);
		IAPISimulation* getIAPISimulation();
		IAPIIntegrator* getIAPIIntegrator();
		IAPIPotentialMaster* getIAPIPotentialMaster();
		
		enum {DISK=1, RAM=2, BINARY=4, ASCII=8};  
		
	protected:
		void callbackCopyExtraRestartInfoTo(std::vector<double>& info);
		void callbackSetExtraRestartInfo(std::vector<double>& info);
		
		IAPISimulation* _sim;
		IAPIIntegrator* _integrator;
		IAPIPotentialMaster* _potential_master;
		int _box_id;
		int _dimensions;
		double _temperature;
		int _restart_mode;

		struct _MemoryMappedTimesliceAtomic 
		{
			std::vector< std::vector<double> > x;
			std::vector< std::vector<double> > v;
			std::vector< double > extra;
		};
		std::map<std::string, _MemoryMappedTimesliceAtomic> _mem_timeslice;
		
};

#endif
