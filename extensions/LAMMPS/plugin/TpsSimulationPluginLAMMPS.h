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
 *  TpsSimulationPluginLAMMPS.h
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#ifndef TPSSIMULATIONPLUGINLAMMPS_H
#define TPSSIMULATIONPLUGINLAMMPS_H

#include <tps/TpsSimulationPluginAtomic.h>
#include <string>

#include <lammps/lammps.h>

/**
\brief uses LAMMPS as the underlying MD simulation engine
\ingroup extensions
\ingroup sim
\bug So far only works with fix_nve, fix_nvt, fix_langevin, and fix_npt
\bug Does not yet work correctly for parallel simulations.  It is not clear 
    whether this will ever be supported, since it greatly complicates the code
    without much apparent gain.  Perhaps for protein folding applications it
    could be useful...
\bug Input commands are never deleted from the list; this is not necessarily
    bad since command with the same name are overwritten by subsequent commands. 
    Thus, only the final command is actually applied.  However, this can
    be problematic in rare cases if the number of commands is large, and 
    initializing a command is slow.
\bug Turning off LAMMPS output makes it hard to tell when LAMMPS exits in error
    However, leaving output on results in huge amounts of information being 
    written to stdout.

\todo Support parallel simulations
\todo Support molecular atom style
\todo Add code to properly perturb molecular velocities

To run a transition path sampling simulation using LAMMPS, the user must 
define a standard LAMMPS script to initialize a simulation. There are
several things to take into consideration here.  First, the capabilities of the
LAMMPS plugin depends on your LAMMPS build.  LAMMPS must be built as a library
to be compatible.  Since this is difficult, we include a CMakeLists.txt in the
3rdparty folder.  We also include a script to patch the standard LAMMPS 
package to allow for time-reversible nvt and npt integrators.  With this 
being said, many of the features of LAMMPS have not yet been implemented, 
although they could be trivially at this point (see todo list). 
*/
class TpsSimulationPluginLAMMPS : public TpsSimulationPluginAtomic
{
	public:
		TpsSimulationPluginLAMMPS();
		TpsSimulationPluginLAMMPS(int, char**);
		virtual ~TpsSimulationPluginLAMMPS();
		
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
		void setVelocities(std::vector< std::vector<double> >&);
		void setPositions(std::vector< std::vector<double> >&);
		void copyPositionsTo(std::vector< std::vector<double> >&);
		void copyVelocitiesTo(std::vector< std::vector<double> >&);
		double computeKinetic();
		
		//LAMMPS plugin specific:
		void readInputScript(const char* filename);
		void readInputScript(std::istream&);
		void setVerbose(bool);
		void minimize(const char* = "0.0001 0.0001 100 1000");
		void executeCommand(const char*);		
		void useRAM(bool=true);
		void useDisk(bool=true);		
		LAMMPS_NS::LAMMPS* getLAMMPS();
		
	protected:
		void appendRestartScript(const char*);
		void init(int, char**);
		void updateLAMMPSInternals();
		void getExtraRestartInfo(std::vector<double>& info);
		void setExtraRestartInfo(std::vector<double>& info);
		void parseFix(std::istream & is);
		
		LAMMPS_NS::LAMMPS* _lmp;
		int _procid;
		int _nproc;
		double _temperature;
		double _pressure;
		bool _verbose;
		std::string _restart_script;
		bool _use_disk;
		std::string _thermostat_name;
		std::string _barostat_name;

		struct _MemoryMappedTimesliceAtomic 
		{
			std::vector< std::vector<double> > x;
			std::vector< std::vector<double> > v;
			std::vector< double > extra;
		};
		std::map<std::string, _MemoryMappedTimesliceAtomic> _mem_timeslice;
		
};

TpsSimulationPluginLAMMPS& safeDowncastToLAMMPSPlugin(TpsSimulationPlugin&);

#endif
