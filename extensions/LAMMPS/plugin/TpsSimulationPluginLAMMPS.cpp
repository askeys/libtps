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
 *  TpsSimulationPluginLAMMPS.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#include "TpsSimulationPluginLAMMPS.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

#include <lammps/input.h>
#include <lammps/output.h>
#include <lammps/atom.h>
#include <lammps/domain.h>
#include <lammps/thermo.h>
#include <lammps/modify.h>
#include <lammps/update.h>

#ifdef _USE_LAMMPS_PATCH
#include <lammps/fix_nvt.h>
#include <lammps/fix_npt.h>
#endif

TpsSimulationPluginLAMMPS::TpsSimulationPluginLAMMPS()
:	
	_temperature(1.0),
	_verbose(false),
	_restart_script(""),
	_use_disk(true)
{
	char arg0[] = "./a.out";
	char arg1[] = "-screen";
	char arg2[] = "none";
	
	int argc = 3;
	char** argv = new char*[3];
	argv[0] = arg0;
	argv[1] = arg1;
	argv[2] = arg2;
	
	init(argc, argv);
	
	delete argv;
}

TpsSimulationPluginLAMMPS::TpsSimulationPluginLAMMPS(int narg, char** arg)
:	_temperature(1.0),
	_verbose(false),
	_restart_script(""),
	_use_disk(true)
{
	init(narg, arg);
}

TpsSimulationPluginLAMMPS::~TpsSimulationPluginLAMMPS()
{
	delete _lmp;
}

void TpsSimulationPluginLAMMPS::init(int narg, char** arg)
{
	MPI_Init(&narg, &arg);

	MPI_Comm_rank(MPI_COMM_WORLD, &_procid);
	MPI_Comm_size(MPI_COMM_WORLD, &_nproc);

	_lmp = new LAMMPS_NS::LAMMPS(narg, arg, MPI_COMM_WORLD);
}

#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGIN

void TpsSimulationPluginLAMMPS::run(int nsteps)
{
	std::ostringstream os;
	os << "run " << nsteps;
	_lmp->input->one(os.str().c_str());
	callbackOnRunCommand();
}

double TpsSimulationPluginLAMMPS::computeHamiltonian()
{
	updateLAMMPSInternals();
	double energy = 0.0;
	_lmp->output->thermo->evaluate_keyword("etotal", &energy);
	int natom = _lmp->atom->nlocal;
	return (energy*natom);
}

double TpsSimulationPluginLAMMPS::getExpectedTemperature()
{
	return _temperature;
}

void TpsSimulationPluginLAMMPS::freeRestart(const char* filename)
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

void TpsSimulationPluginLAMMPS::copyRestart(
	const char* src_file, const char* dest_file)
{
	if (_use_disk) {
		std::ostringstream os;
		os << "cp " << src_file << " " << dest_file;
		system(os.str().c_str());
	}
	else {
		_mem_timeslice[dest_file] = _mem_timeslice[src_file];
	}
}

void TpsSimulationPluginLAMMPS::writeRestart(const char* filename)
{
	updateLAMMPSInternals();
	
	if (_use_disk) {
		std::ostringstream os;
		os << "write_restart " << filename;
		_lmp->input->one(os.str().c_str());
	}
	else {
		_MemoryMappedTimesliceAtomic m;
		copyPositionsTo(m.x);
		copyVelocitiesTo(m.v);
		getExtraRestartInfo(m.extra);
		_mem_timeslice[filename] = m;
	}
}

void TpsSimulationPluginLAMMPS::readRestart(const char* filename)
{
	if (_use_disk) {
		_lmp->input->one("clear");
		std::ostringstream os;
		os << "read_restart " << filename;
		_lmp->input->one(os.str().c_str());
		std::string str;
		std::istringstream is(_restart_script.c_str());
		while (1) {
			std::getline(is, str);
			if (is.fail()) {
				break;
			}
			_lmp->input->one(str.c_str());
		}
	}
	else {
		if (_mem_timeslice.find(filename) == _mem_timeslice.end()) {
			std::cerr << "TpsSimulationPluginLAMMPS: ";
			std::cerr << "could not find memory mapped file " << filename;
			std::cerr << "\n";
			exit(1);
		}
		_MemoryMappedTimesliceAtomic& m = _mem_timeslice[filename];		
		setPositions(m.x);
		setVelocities(m.v);
		setExtraRestartInfo(m.extra);
	}
	updateLAMMPSInternals();
}


#pragma mark REIMPLEMENTED FROM TPSSIMULATIONPLUGINATOMIC

void TpsSimulationPluginLAMMPS::setVelocities(
	std::vector< std::vector<double> >& v)
{
	int natom = v.size();
	int dim = getDimensions();
	double **v_lmp = _lmp->atom->v;
	
	for (int i=0; i<natom; i++) {
		for (int k=0; k<dim; k++) {
			v_lmp[i][k] = v[i][k];
		}
	}
	updateLAMMPSInternals();
}

void TpsSimulationPluginLAMMPS::setPositions(
	std::vector< std::vector<double> >&x)
{
	int natom = x.size();
	int dim = getDimensions();
	double **x_lmp = _lmp->atom->x;
	
	for (int i=0; i<natom; i++) {
		for (int k=0; k<dim; k++) {
			x_lmp[i][k] = x[i][k];
		}
	}
	updateLAMMPSInternals();
}

void TpsSimulationPluginLAMMPS
	::copyPositionsTo(std::vector< std::vector<double> >& x_copy)
{
	int natom = getNumberOfAtoms();
	int dim = 3; //getDimensions();
	x_copy.resize(natom);
	double **x = _lmp->atom->x;
	
	for (int i=0; i<natom; i++) {
		x_copy[i].resize(dim);
		for (int k=0; k<dim; k++) {
			x_copy[i][k] = x[i][k];
		}
	}
}

void TpsSimulationPluginLAMMPS
	::copyVelocitiesTo(std::vector< std::vector<double> >& v_copy)
{
	int natom = getNumberOfAtoms();
	int dim = 3; //getDimensions();
	v_copy.resize(natom);
	double **v = _lmp->atom->v;
	
	for (int i=0; i<natom; i++) {
		v_copy[i].resize(dim);
		for (int k=0; k<dim; k++) {
			v_copy[i][k] = v[i][k];
		}
	}
}

void TpsSimulationPluginLAMMPS::copyTypesTo(std::vector<int>& types)
{
	int natom = getNumberOfAtoms();
	types.resize(natom);
	int *type_lmp = _lmp->atom->type;
	
	for (int i=0; i<natom; i++) {
		types[i] = type_lmp[i];
	}
}


int TpsSimulationPluginLAMMPS::getNumberOfAtoms()
{
	return _lmp->atom->nlocal;
}

void TpsSimulationPluginLAMMPS::copyBoxTo(BoxInfo& box)
{
	box.period.resize(3);
	box.periodic.resize(3);
	box.boxlo.resize(3);
	box.boxhi.resize(3);
	
	for (int i=0; i<3; i++) {
		box.period[i] = _lmp->domain->prd[i];
		box.periodic[i] = _lmp->domain->periodicity[i];
		box.boxlo[i] = _lmp->domain->boxlo[i];
		box.boxhi[i] = _lmp->domain->boxhi[i];
	}
}

void TpsSimulationPluginLAMMPS::invertVelocities()
{
	int natom = getNumberOfAtoms();
	double **v = _lmp->atom->v;
	
	//first the easy part:
	for(int i=0; i<natom; i++)	{
		v[i][0] = -1.0 * v[i][0];
		v[i][1] = -1.0 * v[i][1];
		v[i][2] = -1.0 * v[i][2];
	}

	//now the hard part (handle thermostats and barostats):
	#ifdef _USE_LAMMPS_PATCH
	if (_thermostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_thermostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNVT;
			FixNVT* c = (FixNVT*)(_lmp->modify->fix[ifix]);
			c->invert_velocities();
			updateLAMMPSInternals();
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _thermostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	if (_barostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_barostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNPT;
			FixNPT* c = (FixNPT*)(_lmp->modify->fix[ifix]);
			c->invert_velocities();
			updateLAMMPSInternals();
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _barostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	#endif
}


double TpsSimulationPluginLAMMPS::computeKinetic()
{
	updateLAMMPSInternals();
	double energy = 0.0;
	_lmp->output->thermo->evaluate_keyword("ke", &energy);
	return energy;
}

int TpsSimulationPluginLAMMPS::getDimensions()
{
	return _lmp->domain->dimension;
}

void TpsSimulationPluginLAMMPS::setTimestep(double timestep)
{
	std::ostringstream command;
	command << "timestep " << timestep;
	_lmp->input->one(command.str().c_str());
}

double TpsSimulationPluginLAMMPS::getTimestep()
{
	return _lmp->update->dt;
}

#pragma mark LAMMPS-SPECIFIC FUNCTIONS

/**
\note this function is only used if we are writing the restart files to memory
*/
void TpsSimulationPluginLAMMPS::getExtraRestartInfo(std::vector<double>& info)
{
	info.clear();
	#ifdef _USE_LAMMPS_PATCH
	if (_thermostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_thermostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNVT;
			FixNVT* c = (FixNVT*)(_lmp->modify->fix[ifix]);
			double tinfo[2];
			c->get_restart_info(tinfo);
			info.push_back(tinfo[0]);
			info.push_back(tinfo[1]);
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _thermostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	if (_barostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_barostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNPT;
			FixNPT* c = (FixNPT*)(_lmp->modify->fix[ifix]);
			double binfo[8];
			c->get_restart_info(binfo);
			for (int i=0; i<8; i++) {
				info.push_back(binfo[i]);
			}
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _barostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	#endif
}

/**
\note this function is only used if we are writing the restart files to memory
*/
void TpsSimulationPluginLAMMPS::setExtraRestartInfo(std::vector<double>& info)
{
	#ifdef _USE_LAMMPS_PATCH
	if (_thermostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_thermostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNVT;
			FixNVT* c = (FixNVT*)(_lmp->modify->fix[ifix]);
			c->set_restart_info(&(info[0]));
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _thermostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	if (_barostat_name.size() > 0) {
		int ifix = _lmp->modify->find_fix(_barostat_name.c_str());
		if (ifix >= 0) {
			using LAMMPS_NS::FixNPT;
			FixNPT* c = (FixNPT*)(_lmp->modify->fix[ifix]);
			c->set_restart_info(&(info[0]));
		}
		else {
			std::cerr << "ERROR: TpsSimulationWrapper thinks it has fix \"";
			std::cerr << _barostat_name << "\", but LAMMPS disagrees.\n";
			exit(1);
		}
	}
	#endif
}



void TpsSimulationPluginLAMMPS::appendRestartScript(const char* line)
{
	std::istringstream is(line);
	std::string str;
	is >> str;

    bool tpsstrfound = (str.find("TPS") != std::string::npos || 
                        str.find("tps") != std::string::npos);
    bool skipstrfound = (str.find("SKIP") != std::string::npos ||
                        str.find("skip") != std::string::npos);
    bool ignorestrfound = (str.find("IGNORE") != std::string::npos ||
                        str.find("ignore") != std::string::npos);
     
    if ((skipstrfound || ignorestrfound) && tpsstrfound) {
        return;
    }
		
	if (str == "echo" || 
		str == "log" || 
		str == "pair_style" || 
		str == "pair_coeff" ||
		str == "neighbor" ||
		str == "neigh_modify" ||
		str == "fix" || 
		str == "unfix" ||
		str == "timestep")	{
	
		_restart_script = 
			_restart_script + std::string(line) + std::string("\n");
	}	
		
	if (str == "fix") {
        parseFix(is);
    }
}

/**

\bug only supports standard isotropic version of fix NPT 
*/
void TpsSimulationPluginLAMMPS::parseFix(std::istream & is)
{
	std::string name, group, type;
	is >> name >> group >> type;
	if (type[0] == 'n' && type[1] == 'v' && type[2] == 't') {
		//is >> temp;
		_thermostat_name = name;
		double t_stop, t_start;
		is >> t_start >> t_stop;
		if (t_start != t_stop) {
			std::cerr << t_start << "\t" << t_stop << "\n";
			std::cerr << "Warning: Cannot use LAMMPS temperature ";
			std::cerr << "ramping mechanism during path sampling.\n";
			std::cerr << "If the temperature is not reset to a constant ";
			std::cerr << "value, the initial temperature will be used\n"; 
		}
		_temperature = t_start;
	}
	else if (type[0] == 'n' && type[1] == 'p' && type[2] == 't') {
		//is >> temp;
		_barostat_name = name;
		std::string t_damp, tmp;
		double t_stop, t_start, p_start, p_stop;
		is >> t_start >> t_stop;		
		is >> t_damp >> tmp;
		is >> p_start >> p_stop;

		if (t_start != t_stop || p_start != p_stop) {
			std::cerr << t_start << "\t" << t_stop << "\n";
			std::cerr << "Warning: Cannot use LAMMPS temperature/pressure";
			std::cerr << "ramping mechanism during path sampling.\n";
			std::cerr << "If T/P is not reset to a constant ";
			std::cerr << "value, the initial T/P will be used\n"; 
		}
		_temperature = t_start;		
		_pressure = p_start;
	}	
}

void TpsSimulationPluginLAMMPS::readInputScript(std::istream& is)
{
	_restart_script.clear();
	//_restart_script = _restart_script + "echo none\nlog none\n";
	while (1) {
		std::string str;
		std::getline(is, str);		
		if (is.fail()) {
			break;
		}
		_lmp->input->one(str.c_str());
		appendRestartScript(str.c_str());
	}
	//std::cerr << _restart_script << std::endl;
}

/**
\param filename is the script file

In addition to initializing the simulation, the script file will tell LAMMPS 
how to restart from a file.  The pair_coeffs, fixes, etc. will be initialized
as they were in the input file each time a restart file is read.  In the case 
that commands in the initialization script should not occur during restart (box
rescales, equilibration runs, etc.) add the comment "#TPS should skip this" at
the end of the line and the command will not parsed.
*/
void TpsSimulationPluginLAMMPS::readInputScript(const char* filename)
{
	std::ifstream input_script(filename);
	assert(input_script.good());
    readInputScript(input_script);
	input_script.close();
}		

/**
\command is the command to execute

All commands are saved and called again during each LAMMPS restart sequence.  To
run a command that should not be called during restart, add the comment
"#TPS should skip this" at the end of the line and the command will not parsed.
*/
void TpsSimulationPluginLAMMPS::executeCommand(const char* command)
{
	_lmp->input->one(command);
	appendRestartScript(command);
}

void TpsSimulationPluginLAMMPS::updateLAMMPSInternals()
{
	double timestep = _lmp->update->dt;
	_lmp->input->one("timestep 0");
	_lmp->input->one("run 1");
	std::ostringstream reset_timestep;
	reset_timestep << "timestep " << timestep; 
	_lmp->input->one(reset_timestep.str().c_str());	
}

void TpsSimulationPluginLAMMPS::setVerbose(bool v)
{
	_verbose = v;
}

void TpsSimulationPluginLAMMPS::minimize(const char* c)
{
	std::ostringstream os;
	os << "minimize " << c;
	_lmp->input->one(os.str().c_str());
}


/**
\return a pointer to the underlying LAMMPS simulation
\warning manipulating the LAMMPS object may produce unexpected plugin behavior

For some applications it is necessary to get a pointer to the LAMMPS simulation 
to manipulate LAMMPS directly.  For example, let's suppose we are writing an 
analysis code to print the forces from a LAMMPS simulation.  Typically, TPS 
doesn't need the forces, so there is no interface to get them.  We could write
a TpsData class to extract them, but it is pointless to generalize this, since 
it is a specific problem and there is little chance that others would benefit 
from it's inclusion in the main library.  Therefore, it is reasonable to
get the LAMMPS pointer and directly extract the forces in a very LAMMPS-specfic
way.  In general, it is safe to do this so long as we know our functions will 
only ever be used by LAMMPS. 
*/
LAMMPS_NS::LAMMPS* TpsSimulationPluginLAMMPS::getLAMMPS()
{
	return _lmp;
}

/**
\brief save restart information to memory
*/
void TpsSimulationPluginLAMMPS::useRAM(bool b)
{
	_use_disk = !b;
}

/**
\brief save restart information on the disk
*/
void TpsSimulationPluginLAMMPS::useDisk(bool b)
{
	_use_disk = b;
}

TpsSimulationPluginLAMMPS& safeDowncastToLAMMPSPlugin(TpsSimulationPlugin& sim)
{
	try {
		TpsSimulationPluginLAMMPS& sim_lammps 
			= dynamic_cast<TpsSimulationPluginLAMMPS&>(sim);
			return sim_lammps;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		int cast_to_LAMMPS_simulation = 0;
		assert(cast_to_LAMMPS_simulation);
        //throw 
	}
}


