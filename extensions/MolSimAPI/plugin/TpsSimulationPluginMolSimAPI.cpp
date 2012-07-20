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
 *  TpsSimulationPluginEtomica.cpp
 *  tpsapi
 *
 *  Created by askeys on 7/1/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsSimulationPluginAtomicMolSimAPI.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

TpsSimulationPluginAtomicMolSimAPI::TpsSimulationPluginAtomicMolSimAPI(
	IAPISimulation* sim, IAPIIntegrator* integrator, 
	IAPIPotentialMaster* potenential_master, int dim)
:	
	_sim(sim),
	_integrator(integrator),
	_potential_master(potenential_master),
	_box_id(0),
	_dimensions(dim),
	_temperature(1.0),
	_restart_mode(RAM)
{
}

TpsSimulationPluginAtomicMolSimAPI::~TpsSimulationPluginAtomicMolSimAPI()
{
}

#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGIN

void TpsSimulationPluginAtomicMolSimAPI::run(int nsteps)
{
	for (int i=0; i<nsteps; i++) {
		_integrator->doStep();
	}
}

/**
\bug this function doesn't work yet, so we can't do many types of shooting moves
\todo figure out a way to compute the kinetic + potential energy and return the 
	expected temperature using the API
*/
double TpsSimulationPluginAtomicMolSimAPI::computeHamiltonian()
{	
	return 0.0;// _potential_master->getTotalEnergy();	
}

/**
\bug this function doesn't work yet, so we can't do many types of shooting moves
\todo get the expected temperature from the API or require this information be 
	passed to the plugin on construction
*/
double TpsSimulationPluginAtomicMolSimAPI::getExpectedTemperature()
{
	return _temperature;
}

void TpsSimulationPluginAtomicMolSimAPI::freeRestart(const char* filename)
{
	if (_restart_mode & DISK) {
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

void TpsSimulationPluginAtomicMolSimAPI::copyRestart(
	const char* src_file, const char* dest_file)
{
	if (_restart_mode & DISK) {
		std::ostringstream os;
		os << "cp " << src_file << " " << dest_file;
		system(os.str().c_str());
	}
	else {
		_mem_timeslice[dest_file] = _mem_timeslice[src_file];
	}
}

void conv1dto2d(
	double* x1d, std::vector< std::vector<double> >& x2d, int n, int dim)
{
	x2d.resize(n);
	for (int i=0; i<n; i++) {
		x2d[i].resize(dim);
		for (int j=0; j<dim; j++) {
			x2d[i][j] = x1d[i*dim + j];
		}
	}
}

void conv2dto1d(
	std::vector< std::vector<double> >& x2d, double* x1d, int n, int dim)
{
	for (int i=0; i<n; i++) {
		for (int j=0; j<dim; j++) {
			x1d[i*dim + j] = x2d[i][j];
		}
	}
}

void TpsSimulationPluginAtomicMolSimAPI::writeRestart(const char* filename)
{		
	if (_restart_mode & DISK) {
		if (_restart_mode & BINARY) {
			std::ofstream os(filename, std::ios::binary);
			_MemoryMappedTimesliceAtomic m;
			copyPositionsTo(m.x);
			copyVelocitiesTo(m.v);
			callbackCopyExtraRestartInfoTo(m.extra);
			int ndim = getDimensions();
			int n;
			n = m.x.size()*ndim;
			os.write((char*)&n, sizeof(int));
			double x[n];
			conv2dto1d(m.x, x, m.x.size(), ndim);
			os.write((char*)x, n*sizeof(int));

			n = m.v.size()*ndim;
			os.write((char*)&n, sizeof(int));
			double v[n];
			conv2dto1d(m.v, v, m.v.size(), ndim);
			os.write((char*)&n, sizeof(int));
			os.write((char*)v, n*sizeof(double));

			n = m.extra.size();
			os.write((char*)&n, sizeof(int));
			os.write((char*)&m.extra[0], n*sizeof(int));
			os.close();
		}
		else {
			std::ofstream os(filename);
			_MemoryMappedTimesliceAtomic m;
			copyPositionsTo(m.x);
			copyVelocitiesTo(m.v);
			callbackCopyExtraRestartInfoTo(m.extra);
			int ndim = getDimensions();
			int n;
			n = m.x.size()*ndim;
			os << n << "\t";			
			double x[n];
			conv2dto1d(m.x, x, m.x.size(), ndim);
			for (int i=0; i<n; i++) {
				os << x[i] << "\t";
			}
			os << "\n";

			n = m.v.size()*ndim;
			os << n << "\t";			
			double v[n];
			conv2dto1d(m.v, v, m.v.size(), ndim);
			for (int i=0; i<n; i++) {
				os << v[i] << "\t";
			}
			os << "\n";

			n = m.extra.size();
			os << n << "\t";
			for (int i=0; i<n; i++) {
				os << m.extra[i] << "\t";
			}
			os << "\n";
			os.close();
		}
	}
	else {
		_MemoryMappedTimesliceAtomic m;
		copyPositionsTo(m.x);
		copyVelocitiesTo(m.v);
		callbackCopyExtraRestartInfoTo(m.extra);
		_mem_timeslice[filename] = m;
	}
}
 
void TpsSimulationPluginAtomicMolSimAPI::readRestart(const char* filename)
{
	if (_restart_mode & DISK) {
		
		if (_restart_mode & BINARY) {
			std::ifstream is(filename, std::ios::binary);
			if (is.fail()) {
				std::cerr << "TpsSimulationPluginAtomicMolSimAPI: ";
				std::cerr << "could not find restart file " << filename;
				std::cerr << "\n";
				exit(1);
			}
			
			int ndim = getDimensions();
			std::vector< std::vector<double> > x, v;
			int n;
			is.read((char*)&n, sizeof(int));
			double x1d[n];
			is.read((char*)x1d, n*sizeof(double));
			conv1dto2d(x1d, x, n/ndim, ndim); 
			setPositions(x);
			
			is.read((char*)&n, sizeof(int));
			double v1d[n];
			is.read((char*)v1d, n*sizeof(double));
			conv1dto2d(v1d, v, n/ndim, ndim); 
			setVelocities(v);

			is.read((char*)&n, sizeof(int));
			std::vector<double> extra(n);
			is.read((char*)&extra[0], n*sizeof(double));
			callbackSetExtraRestartInfo(extra);
			is.close();
		}
		else {
			std::ifstream is(filename);
			if (is.fail()) {
				std::cerr << "TpsSimulationPluginAtomicMolSimAPI: ";
				std::cerr << "could not find restart file " << filename;
				std::cerr << "\n";
				exit(1);
			}
			int ndim = getDimensions();
			std::vector< std::vector<double> > x, v;
			int n;
			is >> n;
			double x1d[n];
			for (int i=0; i<n; i++) {
				double tmp;
				is >> tmp;
				x1d[i] = tmp;
			}
			conv1dto2d(x1d, x, n/ndim, ndim);
			setPositions(x);

			is >> n;
			double v1d[n];
			for (int i=0; i<n; i++) {
				double tmp;
				is >> tmp;
				v1d[i] = tmp;
			}
			conv1dto2d(v1d, v, n/ndim, ndim); 
			setVelocities(v);
			
			is >> n;
			std::vector<double> extra(n);
			for (int i=0; i<n; i++) {
				double tmp;
				is >> tmp;
				extra[i] = tmp;
			}
			callbackSetExtraRestartInfo(extra);
			is.close();
		}		
	}
	else {
		if (_mem_timeslice.find(filename) == _mem_timeslice.end()) {
			std::cerr << "TpsSimulationPluginAtomicMolSimAPI: ";
			std::cerr << "could not find memory mapped file " << filename;
			std::cerr << "\n";
			exit(1);
		}
		_MemoryMappedTimesliceAtomic& m = _mem_timeslice[filename];		
		setPositions(m.x);
		setVelocities(m.v);
		callbackSetExtraRestartInfo(m.extra);
	}
}


#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGINATOMIC

void TpsSimulationPluginAtomicMolSimAPI::setVelocities(
	std::vector< std::vector<double> >& v)
{
	int natom = v.size();
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();
	for (int i=0; i<natom; i++) {
		IAPIAtomKinetic *atom = dynamic_cast<IAPIAtomKinetic*>(al->getAtom(i));
		atom->getVelocity()->E(&v[i][0]);
	}	
}

void TpsSimulationPluginAtomicMolSimAPI::setPositions(
	std::vector< std::vector<double> >&x)
{
	int natom = x.size();
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();
	for (int i=0; i<natom; i++) {
		IAPIAtomKinetic *atom = dynamic_cast<IAPIAtomKinetic*>(al->getAtom(i));
		atom->getPosition()->E(&x[i][0]);
	}
}

void TpsSimulationPluginAtomicMolSimAPI
	::copyPositionsTo(std::vector< std::vector<double> >& x_copy)
{
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();
	int natom = getNumberOfAtoms();
	int dim = getDimensions();
	x_copy.resize(natom);
		
	for (int i=0; i<natom; i++) {
		x_copy[i].resize(dim);
		IAPIAtomPositioned *atom = 
			dynamic_cast<IAPIAtomPositioned*>(al->getAtom(i));
		IAPIVector* x = atom->getPosition();
		for (int k=0; k<dim; k++) {
			x_copy[i][k] = x->x(k);
		}
	}
}

void TpsSimulationPluginAtomicMolSimAPI
	::copyVelocitiesTo(std::vector< std::vector<double> >& v_copy)
{
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();
	int natom = getNumberOfAtoms();
	int dim = getDimensions();
	v_copy.resize(natom);
		
	for (int i=0; i<natom; i++) {
		v_copy[i].resize(dim);
		IAPIAtomKinetic *atom = dynamic_cast<IAPIAtomKinetic*>(al->getAtom(i));
		IAPIVector* v = atom->getVelocity();
		for (int k=0; k<dim; k++) {
			v_copy[i][k] = v->x(k);
		}
	}
}

void TpsSimulationPluginAtomicMolSimAPI::copyTypesTo(std::vector<int>& types)
{
	int natom = getNumberOfAtoms();
	types.resize(natom);
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();

	for (int i=0; i<natom; i++) {
		types[i] = al->getAtom(i)->getType()->getIndex();
	}
}

int TpsSimulationPluginAtomicMolSimAPI::getNumberOfAtoms()
{
	return _sim->getBox(_box_id)->getLeafList()->getAtomCount();
}

/**
\note not yet working for boxes not centered at the origin
*/
void TpsSimulationPluginAtomicMolSimAPI::copyBoxTo(BoxInfo& box)
{	
	int d = getDimensions();
	
	box.period.resize(d);
	box.periodic.resize(d);
	box.boxlo.resize(d);
	box.boxhi.resize(d);
	
	//get dimensions = get box dimensions
	IAPIVector* p = _sim->getBox(_box_id)->getBoundary()->getDimensions();
	
	for (int i=0; i<d; i++) {
		box.period[i] = p->x(i);
		box.periodic[i] = _sim->getBox(_box_id)->getBoundary()->getPeriodicity(i);
		box.boxlo[i] = -0.5*p->x(i);
		box.boxhi[i] = 0.5*p->x(i);
	}
}

/**
\brief Invert the velocities for time reversal
\warning Function does not check if underlying integrator is not time-reversible
\note Does not yet work for extended system models (i.e., thermostats and 
	barostats with positions/velocities).  One problem is that this is 
	nearly impossible to generalize.  Current solution: add a callback
	for user-defined reading writing or extra restart info, such as extended
	system x, v, f, etc.  
*/
void TpsSimulationPluginAtomicMolSimAPI::invertVelocities()
{
	int natom = getNumberOfAtoms();
	int dim = getDimensions();
	std::vector< std::vector<double> > v;
	copyVelocitiesTo(v);
	
	//first the easy part:
	for(int i=0; i<natom; i++)	{
		for (int j=0; j<dim; j++) {
			v[i][j] = -1.0 * v[i][j];
		}
	}
	
	setVelocities(v);	
	callbackOnInvertVelocities();
}

/**
\brief A callback that is called every time invertVelocities() is called 
\note possibly avoid this by use IAPIAtoms to contain extended system info

This function is typically re-implemented if velocities other than the particle
velocities need to be inverted (e.g., extended system velocities)
*/
void TpsSimulationPluginAtomicMolSimAPI::callbackOnInvertVelocities()
{	
	//override this function to do something if necessary
}

/**
\brief computes the kinetic energy
\note would be better to compute this from the API to generalize linear 
	and angular contributions (we assume linear only for now)
The kinetic energy is used for two things
	-# Computing the total Hamiltonian in the case that velocities are changed
		during a trial move
	-# Ensuring that the total energy is conserved in a constant energy trial	
		move (often used for NVE simulations)
*/
double TpsSimulationPluginAtomicMolSimAPI::computeKinetic()
{
	std::vector <std::vector<double> > v;
	copyVelocitiesTo(v);
	int natom = getNumberOfAtoms();
	int dim = getDimensions();
	IAPIAtomList *al = _sim->getBox(_box_id)->getLeafList();

	double ke = 0.0;
	for (int i=0; i<natom; i++) {
		double e = 0.0;
		for (int j=0; j<dim; j++) {
			e += v[i][j]*v[i][j]; 
		}
		ke += al->getAtom(i)->getType()->getMass() * e;
	}
	return 0.5*ke;
}

/**
\returns the dimensionality of space
\note this information is currently passed on construction, but it could be 
	possible to get it from the API
*/
int TpsSimulationPluginAtomicMolSimAPI::getDimensions()
{
	return _dimensions;
}

/**
\brief sets the integration timestep (if applicable)
\note had to add non-API methods here
*/
void TpsSimulationPluginAtomicMolSimAPI::setTimestep(double timestep)
{
	if (_integrator->stepSizeIsMutable()) {
		IAPIIntegratorMD* i = dynamic_cast<IAPIIntegratorMD*>(_integrator);
		i->setTimestep(timestep);
	}
}

/**
\brief sets the integration timestep (if applicable)
\note had to add non-API methods here
*/
double TpsSimulationPluginAtomicMolSimAPI::getTimestep()
{
	if (_integrator->stepSizeIsMutable()) {
		IAPIIntegratorMD* i = dynamic_cast<IAPIIntegratorMD*>(_integrator);
		return i->getTimestep();
	}
	else return 0.0;
}

#pragma mark MolSimAPI-SPECIFIC FUNCTIONS

/**
\brief a callback to copy additional restart information to a vector
*/
void TpsSimulationPluginAtomicMolSimAPI::callbackCopyExtraRestartInfoTo(std::vector<double>& info)
{
}

/**
\brief a callback to load additional restart information into the simulation
*/
void TpsSimulationPluginAtomicMolSimAPI::callbackSetExtraRestartInfo(std::vector<double>& info)
{
}

/**
\return a pointer to the underlying MolSimAPI simulation
\warning manipulating the MolSimAPI object may produce unexpected plugin behavior

For some applications it is necessary to get a pointer to the MolSimAPI simulation 
to manipulate MolSimAPI directly.  For example, for custom analysis codes or 
to read/write custom restart information.
*/
IAPISimulation* TpsSimulationPluginAtomicMolSimAPI::getIAPISimulation()
{
	return _sim;
}

/**
\brief save restart information to memory
*/
void TpsSimulationPluginAtomicMolSimAPI::setRestartMode(int i)
{
	_restart_mode = i;
}
