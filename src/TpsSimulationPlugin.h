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


#ifndef TPSSIMULATIONPLUGIN_H
#define TPSSIMULATIONPLUGIN_H

#include <vector>
class TpsRNG;
class TpsSimulationPluginAtomic;
class TpsSimulationPluginLAMMPS;

/**
\brief An interface for hooking an arbitrary simulation code into the API
\ingroup core
\ingroup sim

The various TPS schemes require a simulation code to create trajectories.  
At a minimum, the simulation must  be capable of restarting at a particular 
timeslice along a trajectory and performing timesteps (MD) or trial moves (MC).  
Some schemes require time-reversibility.  The interface is sufficiently 
flexible that any molecular simulation code can be easily plugged in.  We will
eventually implement every standard molecular simulation package using the 
Etomica API.

This interface defines several required methods, which must be implemented
in order for the plugin to function correctly:
	- run
	- writeRestart
	- readRestart
	- freeRestart
	- copyRestart
	- computeHamiltonian
	- getExpectedTemperature
	- invertVelocities (only required for deterministic dynamics)
	
Additionally, there are several callback methods that can be defined to 
dynamically customize the plugin functionality.  The idea of a callbacks is 
that a class will call these methods each time a specific member function is 
called.  For example, we can define a method to execute every time the "run" 
function is called, by implementing callbackOnRunFunction().  Say we wanted to 
print the simulation energy to the screen after each simulation run.  We 
could do something like the following:

\code
void MySimulationPlugin::callbackOnRunCommand()
{
	energy = myEnergyFunction();
	cout << energy << endl;
}
\endcode

Probably the most useful aspect of the callbacks is to visualize the simulation
or reaction coordinates real-time as the TpsTrialMove is being executed.  Note
that since trajectories are typically created in a way that is non-contiguous, 
the resulting visualization will be non-contiguous as well.  If a contiguous
trajectory is desirable, it is typically better to implement the visualization
as a TpsAlgorithm callback.
*/
class TpsSimulationPlugin
{
	public:

		TpsSimulationPlugin();
		virtual ~TpsSimulationPlugin();

//Required:

		virtual void run(int nsteps=1);
		virtual void writeRestart(const char* filename);
		virtual void readRestart(const char* filename);
		virtual void freeRestart(const char* filename);
		virtual void copyRestart(const char* src_file, const char* dest_file);
		virtual double computeHamiltonian();
		virtual double getExpectedTemperature();
		
//Required for deterministic dynamics:

		virtual void invertVelocities();

//Optional:

		virtual void callbackOnRunCommand();									///<Optional function that is called every time the "run()" method is called
		virtual void callbackOnReadRestartCommand();							///<Optional function that is called every time the "readRestart()" method is called
		virtual void callbackOnWriteRestartCommand();							///<Optional function that is called every time the "writeRestart()" method is called

		virtual void writeData(const char* filename, int mode=0);				///<A function that one can override to define how a simulation writes output data 

//not re-implemented:		
		TpsSimulationPluginAtomic& safeDowncastToAtomicPlugin();
		//TpsSimulationPluginLattice& safeDowncastLattice();
};

#endif
