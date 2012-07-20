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

#ifndef TPSSIMULATIONPLUGINATOMIC_H
#define TPSSIMULATIONPLUGINATOMIC_H

#include "TpsSimulationPlugin.h"

struct BoxInfo
{
	BoxInfo()
	: 	periodic(std::vector<bool>(3, true)),
		period(std::vector<double>(3, 0.0)),
		boxlo(std::vector<double>(3, 0.0)),
		boxhi(std::vector<double>(3, 0.0))
	{	
	}
	std::vector<bool> periodic;
	std::vector<double> period;
	std::vector<double> boxlo;
	std::vector<double> boxhi;
};

/**
\brief An interface to an atomistic simulation code
\ingroup sim

This interface defines methods common to an atomistic simulation codes:
	- positions
	- velocities
	- simulation box
	- boundary conditions
*/

class TpsSimulationPluginAtomic : public TpsSimulationPlugin
{

	public:
		TpsSimulationPluginAtomic();
		virtual ~TpsSimulationPluginAtomic();
		
		virtual int getNumberOfAtoms();
		virtual void copyTypesTo(std::vector<int>&);
		virtual void copyBoxTo(BoxInfo&);
		virtual void invertVelocities();

		virtual void setVelocities(std::vector< std::vector<double> >&);
		virtual void setPositions(std::vector< std::vector<double> >&);
		virtual void copyPositionsTo(std::vector< std::vector<double> >&);
		virtual void copyVelocitiesTo(std::vector< std::vector<double> >&);

		virtual double computeKinetic();
		virtual int getDimensions();
		virtual void setTimestep(double);
		virtual double getTimestep();		
};

#endif
