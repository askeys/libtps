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
 *  TpsSimulationPluginHOOMD.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#include "TpsSimulationPluginHOOMD.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

#include <hoomd/SystemDefinition.h>
#include <hoomd/Integrator.h>
#include <hoomd/HOOMDBinaryDumpWriter.h>
#include <hoomd/HOOMDBinaryInitializer.h>

TpsSimulationPluginHOOMD::TpsSimulationPluginHOOMD(
    boost::shared_ptr<System> hoomd)
:	
	_hoomd(hoomd),
    _temperature(1.0),
	_use_disk(true),
	_timesteps(0)
{
    _hoomd->enableQuietRun(true);
}

TpsSimulationPluginHOOMD::~TpsSimulationPluginHOOMD()
{
}

#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGIN

void TpsSimulationPluginHOOMD::run(int nsteps)
{
    boost::python::object tmp;
    _hoomd->run(nsteps, 0, tmp);
	callbackOnRunCommand();
}

double TpsSimulationPluginHOOMD::computeHamiltonian()
{
    int t = _hoomd->getCurrentTimeStep();
	double ke = _hoomd->getIntegrator()->computeKineticEnergy(t);
	double pe = _hoomd->getIntegrator()->computePotentialEnergy(t);	
	return (ke + pe);
}

double TpsSimulationPluginHOOMD::getExpectedTemperature()
{
	return _temperature;
}

void TpsSimulationPluginHOOMD::freeRestart(const char* filename)
{
	if (_use_disk) {
		if (remove(filename) == -1) {
			std::cerr << "Error: could not delete file " << filename << "\n";
		}
	}
	else {
		if (_mem_timeslice.find(filename) != _mem_timeslice.end()) {
			_mem_timeslice.erase(filename);
		}
	}
}

void TpsSimulationPluginHOOMD::copyRestart(
	const char* src_file, const char* dest_file)
{
	if (_use_disk) {
		std::ostringstream os;
		os << "cp " << src_file << " " << dest_file;
		system(os.str().c_str());
	}
	else {
        //std::cerr << src_file << "\t" << dest_file << std::endl;
		_mem_timeslice[dest_file] = _mem_timeslice[src_file];
	}
}

void TpsSimulationPluginHOOMD::writeRestart(const char* filename)
{	
	if (_use_disk) {
		boost::shared_ptr<HOOMDBinaryDumpWriter> dump(
            new HOOMDBinaryDumpWriter(_hoomd->getSystemDefinition(), ""));
		dump->writeFile(filename, _timesteps);
	}
	else {
		_MemoryMappedTimesliceAtomic m;
		copyPositionsTo(m.x);
		copyVelocitiesTo(m.v);
        copyAccelerationsTo(m.a);
        copyBoxTo(m.box);
		getExtraRestartInfo(m.extra);
		_mem_timeslice[filename] = m;
	}
}

void TpsSimulationPluginHOOMD::readRestart(const char* filename)
{
	if (_use_disk) {
        HOOMDBinaryInitializer init(filename);

        const ParticleDataArrays& arrays = 
            _hoomd->getSystemDefinition()->getParticleData()->acquireReadWrite();
            init.initArrays(arrays);
        _hoomd->getSystemDefinition()->getParticleData()->release();

        _hoomd->getSystemDefinition()->getParticleData()->setBox(init.getBox());
        init.initIntegratorData(
            _hoomd->getSystemDefinition()->getIntegratorData());
 	}
	else {
		if (_mem_timeslice.find(filename) == _mem_timeslice.end()) {
			std::cerr << "TpsSimulationPluginHOOMD: ";
			std::cerr << "could not find memory mapped file " << filename;
			std::cerr << "\n";
			exit(1);
		}
		_MemoryMappedTimesliceAtomic& m = _mem_timeslice[filename];		
		setPositions(m.x);
		setVelocities(m.v);
		setAccelerations(m.a);
        setBox(m.box);
        setExtraRestartInfo(m.extra);
	}
}


#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGINATOMIC

void TpsSimulationPluginHOOMD::setPositions(
	std::vector< std::vector<double> >&x)
{
	const ParticleDataArrays& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadWrite();

	int natom = arrays.nparticles;
	for (int i=0; i<natom; i++) {
		arrays.x[arrays.rtag[i]] = x[i][0];
		arrays.y[arrays.rtag[i]] = x[i][1];
		arrays.z[arrays.rtag[i]] = x[i][2];
	}
    _hoomd->getSystemDefinition()->getParticleData()->notifyParticleSort();
	_hoomd->getSystemDefinition()->getParticleData()->release();
    
    /*
    double timestep = getTimestep();
    setTimestep(0.0);
    run(1);
    setTimestep(timestep);
    */
}

void TpsSimulationPluginHOOMD::setVelocities(
	std::vector< std::vector<double> >& v)
{
	const ParticleDataArrays& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadWrite();

	int natom = arrays.nparticles;
	for (int i=0; i<natom; i++) {
		arrays.vx[arrays.rtag[i]] = v[i][0];
		arrays.vy[arrays.rtag[i]] = v[i][1];
		arrays.vz[arrays.rtag[i]] = v[i][2];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}

void TpsSimulationPluginHOOMD
	::copyPositionsTo(std::vector< std::vector<double> >& x_copy)
{
	const ParticleDataArraysConst& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadOnly();

	int natom = arrays.nparticles;
	int dim = 3;
	x_copy.resize(natom);

	for (int i=0; i<natom; i++) {
		x_copy[i].resize(dim);
		x_copy[i][0] = arrays.x[arrays.rtag[i]];
		x_copy[i][1] = arrays.y[arrays.rtag[i]];
		x_copy[i][2] = arrays.z[arrays.rtag[i]];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}

void TpsSimulationPluginHOOMD
	::copyVelocitiesTo(std::vector< std::vector<double> >& v_copy)
{
	const ParticleDataArraysConst& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadOnly();

	int natom = arrays.nparticles;
	int dim = 3;
	v_copy.resize(natom);

	for (int i=0; i<natom; i++) {
		v_copy[i].resize(dim);
		v_copy[i][0] = arrays.vx[arrays.rtag[i]];
		v_copy[i][1] = arrays.vy[arrays.rtag[i]];
		v_copy[i][2] = arrays.vz[arrays.rtag[i]];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}

void TpsSimulationPluginHOOMD::copyTypesTo(std::vector<int>& types)
{
	const ParticleDataArraysConst& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadOnly();

	int natom = arrays.nparticles;
	types.resize(natom);

	for (int i=0; i<natom; i++) {
		types[i] = arrays.type[arrays.rtag[i]];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}


int TpsSimulationPluginHOOMD::getNumberOfAtoms()
{
	const ParticleDataArraysConst& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadOnly();
	int np = arrays.nparticles;
	_hoomd->getSystemDefinition()->getParticleData()->release();
	return np;
}

void TpsSimulationPluginHOOMD::copyBoxTo(BoxInfo& box)
{
	box.period.resize(3);
	box.periodic.resize(3);
	box.boxlo.resize(3);
	box.boxhi.resize(3);
	
	const BoxDim& hdbox = 
        _hoomd->getSystemDefinition()->getParticleData()->getBox();	
	
	box.period[0] = hdbox.xhi - hdbox.xlo;
	box.period[1] = hdbox.yhi - hdbox.ylo;
	box.period[2] = hdbox.zhi - hdbox.zlo;		

	box.boxlo[0] = hdbox.xlo;
	box.boxlo[1] = hdbox.ylo;
	box.boxlo[2] = hdbox.zlo;

	box.boxhi[0] = hdbox.xhi;
	box.boxhi[1] = hdbox.yhi;
	box.boxhi[2] = hdbox.zhi;
}

void TpsSimulationPluginHOOMD::invertVelocities()
{
	std::vector< std::vector<double> > v;
	copyVelocitiesTo(v);
	
	for (unsigned int i=0; i<v.size(); i++) {
		v[i][0] = -v[i][0];
		v[i][1] = -v[i][1];
		v[i][2] = -v[i][2];
	}
	setVelocities(v);

	//now handle thermostats, barostats, etc.:
    boost::shared_ptr<IntegratorData> idata = 
        _hoomd->getSystemDefinition()->getIntegratorData();
    
    unsigned int index=0;
    unsigned int nintegrators = idata->getNumIntegrators();
    
    for (unsigned int i=0; i<nintegrators; i++) {
        IntegratorVariables v = idata->getIntegratorVariables(i);
        if (v.type == "nvt") {
            v.variable[0] *= -1.0;
            v.variable[1] *= -1.0;          
        }
        else if (v.type == "npt") {
            v.variable[0] *= -1.0;            
            v.variable[1] *= -1.0;            
        }        
        idata->setIntegratorVariables(i, v);
    }
}

double TpsSimulationPluginHOOMD::computeKinetic()
{
    int t = _hoomd->getCurrentTimeStep();
	return _hoomd->getIntegrator()->computeKineticEnergy(t);
}

int TpsSimulationPluginHOOMD::getDimensions()
{
	return _hoomd->getSystemDefinition()->getNDimensions();
}

void TpsSimulationPluginHOOMD::setTimestep(double timestep)
{
	_hoomd->getIntegrator()->setDeltaT((Scalar)timestep);
}

double TpsSimulationPluginHOOMD::getTimestep()
{
	return _hoomd->getIntegrator()->getDeltaT();
}

#pragma mark HOOMD-SPECIFIC FUNCTIONS

void TpsSimulationPluginHOOMD::setExpectedTemperature(double temperature)
{
    _temperature = temperature;
}

void TpsSimulationPluginHOOMD::setBox(BoxInfo& box)
{
    double Lx = box.period[0];
    double Ly = box.period[1];
    double Lz = box.period[2];
    _hoomd->getSystemDefinition()->getParticleData()->setBox(
        BoxDim(Lx, Ly, Lz));
}

void TpsSimulationPluginHOOMD::setAccelerations(
	std::vector< std::vector<double> >&a)
{
	const ParticleDataArrays& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadWrite();

	int natom = arrays.nparticles;
	for (int i=0; i<natom; i++) {
		arrays.ax[arrays.rtag[i]] = a[i][0];
		arrays.ay[arrays.rtag[i]] = a[i][1];
		arrays.az[arrays.rtag[i]] = a[i][2];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}

void TpsSimulationPluginHOOMD
	::copyAccelerationsTo(std::vector< std::vector<double> >& a_copy)
{
	const ParticleDataArraysConst& arrays = 
		_hoomd->getSystemDefinition()->getParticleData()->acquireReadOnly();

	int natom = arrays.nparticles;
	int dim = 3;
	a_copy.resize(natom);

	for (int i=0; i<natom; i++) {
		a_copy[i].resize(dim);
		a_copy[i][0] = arrays.ax[arrays.rtag[i]];
		a_copy[i][1] = arrays.ay[arrays.rtag[i]];
		a_copy[i][2] = arrays.az[arrays.rtag[i]];
	}
	_hoomd->getSystemDefinition()->getParticleData()->release();
}

/**
\note this function is only used if we are writing the restart files to memory
*/
void TpsSimulationPluginHOOMD::getExtraRestartInfo(std::vector<double>& info)
{
    boost::shared_ptr<IntegratorData> idata = 
        _hoomd->getSystemDefinition()->getIntegratorData();
    
    info.clear();
    unsigned int nintegrators = idata->getNumIntegrators();
    for (unsigned int i=0; i<nintegrators; i++) {
        unsigned int nv = idata->getIntegratorVariables(i).variable.size();
        for (unsigned int j=0; j<nv; j++) { 
            info.push_back(idata->getIntegratorVariables(i).variable[j]);
        }
    }
}

/**
\note this function is only used if we are writing the restart files to memory
*/
void TpsSimulationPluginHOOMD::setExtraRestartInfo(std::vector<double>& info)
{
    boost::shared_ptr<IntegratorData> idata = 
        _hoomd->getSystemDefinition()->getIntegratorData();
    
    unsigned int index=0;
    unsigned int nintegrators = idata->getNumIntegrators();
    for (unsigned int i=0; i<nintegrators; i++) {
        IntegratorVariables v = idata->getIntegratorVariables(i);
        unsigned int nv = v.variable.size();
        for (unsigned int j=0; j<nv; j++) {
            v.variable[j] = info[index++];
        }
        idata->setIntegratorVariables(i, v);
    }
}

/**
\brief save restart information to memory
*/
void TpsSimulationPluginHOOMD::useRAM(bool b)
{
	_use_disk = !b;
}

/**
\brief save restart information on the disk
*/
void TpsSimulationPluginHOOMD::useDisk(bool b)
{
	_use_disk = b;
}

boost::shared_ptr<System> TpsSimulationPluginHOOMD::getHOOMD()
{
    return _hoomd;
}

TpsSimulationPluginHOOMD& safeDowncastToHOOMDPlugin(TpsSimulationPlugin& sim)
{
	try {
		TpsSimulationPluginHOOMD& sim_lammps 
			= dynamic_cast<TpsSimulationPluginHOOMD&>(sim);
			return sim_lammps;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		int cast_to_HOOMD_simulation = 0;
		assert(cast_to_HOOMD_simulation);
        //throw 
	}
}

