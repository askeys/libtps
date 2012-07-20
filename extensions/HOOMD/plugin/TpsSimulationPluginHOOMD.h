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
 *  TpsSimulationPluginHOOMD.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#ifndef TPSSIMULATIONPLUGINHOOMD_H
#define TPSSIMULATIONPLUGINHOOMD_H

#include <tps/TpsSimulationPluginAtomic.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include <hoomd/hoomd_config.h>
#include <hoomd/System.h>

/**
\brief uses HOOMD as the underlying MD simulation engine
\ingroup extensions
\ingroup sim

To run a transition path sampling simulation using HOOMD, the user must 
define the function  initialize a simulation.
*/
class TpsSimulationPluginHOOMD : public TpsSimulationPluginAtomic
{
	public:
		TpsSimulationPluginHOOMD(boost::shared_ptr<System>);
        virtual ~TpsSimulationPluginHOOMD();
		
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
		void copyTypesTo(std::vector<int>&);
		void copyBoxTo(BoxInfo&);
		void setPositions(std::vector< std::vector<double> >&);
		void setVelocities(std::vector< std::vector<double> >&);
		void copyPositionsTo(std::vector< std::vector<double> >&);
		void copyVelocitiesTo(std::vector< std::vector<double> >&);
		double computeKinetic();
		
		//HOOMD plugin specific:
		void setExpectedTemperature(double);
        void useRAM(bool=true);
		void useDisk(bool=true);
        boost::shared_ptr<System> getHOOMD();
        
	protected:
        
		void setAccelerations(std::vector< std::vector<double> >&);
		void copyAccelerationsTo(std::vector< std::vector<double> >&);
        void setBox(BoxInfo&);

        boost::shared_ptr<System> _hoomd;        
		void getExtraRestartInfo(std::vector<double>& info);
		void setExtraRestartInfo(std::vector<double>& info);

		double _temperature;
		double _pressure;
		bool _use_disk;
		int _timesteps;

		struct _MemoryMappedTimesliceAtomic 
		{
			std::vector< std::vector<double> > x;
			std::vector< std::vector<double> > v;
			std::vector< std::vector<double> > a;
            BoxInfo box;
			std::vector< double > extra;
		};
		std::map<std::string, _MemoryMappedTimesliceAtomic> _mem_timeslice;
		
};

TpsSimulationPluginHOOMD& safeDowncastToHOOMDPlugin(TpsSimulationPlugin&);

#endif
