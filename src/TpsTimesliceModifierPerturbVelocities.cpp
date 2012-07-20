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
 *  TpsTimesliceModifierVelocities.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/21/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTimesliceModifierPerturbVelocities.h"
#include "TpsSimulationPluginAtomic.h"
#include <cassert>
#include <cmath>
#include <iostream>

TpsTimesliceModifierPerturbVelocities::TpsTimesliceModifierPerturbVelocities(
		double max_perturbation, 
		double coupling_strength, 
		bool zero_net_velocity, 
		bool conserve_energy)
:	_max_perturbation(max_perturbation),
	_coupling_strength(coupling_strength),
	_zero_net_velocity(zero_net_velocity),
	_conserve_energy(conserve_energy)
{	
}

TpsTimesliceModifierPerturbVelocities::
	~TpsTimesliceModifierPerturbVelocities()
{	
}

void TpsTimesliceModifierPerturbVelocities::setConserveEnergy(bool b)
{
	_conserve_energy = b;
}

void TpsTimesliceModifierPerturbVelocities::setPerturbationSize(double dv)
{
	_max_perturbation = dv;
}

void TpsTimesliceModifierPerturbVelocities::setCouplingStrength(double str)
{
	_coupling_strength = str;
}

void TpsTimesliceModifierPerturbVelocities::setZeroNetVelocity(bool z)
{
	_zero_net_velocity = z;
}

bool TpsTimesliceModifierPerturbVelocities::modify(TpsSimulationPlugin& sim)
{
	if (_conserve_energy) {
		return modifyConstEnergy(sim);
	}
	else {
		return modifyBoltzmann(sim);
	}

}

bool TpsTimesliceModifierPerturbVelocities::modifyConstEnergy(
	TpsSimulationPlugin& sim_base)
{
	TpsSimulationPluginAtomic& sim = sim_base.safeDowncastToAtomicPlugin();
	
	double e_kinetic_old = sim.computeKinetic();
	int dim = sim.getDimensions();
		
	std::vector< std::vector<double> > v, v_old;
	sim.copyVelocitiesTo(v);
	for (unsigned int i=0; i<v.size(); i++) {
		v_old.push_back(std::vector<double>(dim));
		for (int k=0; k<dim; k++) {
			v_old[i][k] = v[i][k];
		}
	}
	
	for (unsigned int i=0; i<v.size(); i++) {
		if (_rng.randDouble() < _coupling_strength) {
			for (int k=0; k<dim; k++) {
				v[i][k] += _max_perturbation*_rng.randNormal();
			}
		}
	}
	if (_zero_net_velocity) {
		std::vector<double> v_cm(dim, 0.0);
		for (unsigned int i=0; i<v.size(); i++) {
			for (int k=0; k<dim; k++) {
				v_cm[k] += v[i][k];
			}
		}
		for (int k=0; k<dim; k++) {
			v_cm[k] /= (double)v.size();
		}
		
		for (unsigned int i=0; i<v.size(); i++) {
			for (int k=0; k<dim; k++) {
				v[i][k] -= v_cm[k];
			}
		}		
	}
		
	double e_kinetic_new = sim.computeKinetic();
	double scaling_factor = sqrt(e_kinetic_old / e_kinetic_new);
	
	for (unsigned int i=0; i<v.size(); i++) {
		for (int k=0; k<dim; k++) {
			v[i][k] *= scaling_factor;
		}
	}
	sim.setVelocities(v);
	return true;
}

bool TpsTimesliceModifierPerturbVelocities::modifyBoltzmann(
	TpsSimulationPlugin& sim_base)
{
	TpsSimulationPluginAtomic& sim = sim_base.safeDowncastToAtomicPlugin();
	int dim = sim.getDimensions();
	
	double beta = 1.0/sim.getExpectedTemperature();
	double e_old = sim.computeHamiltonian();
	
	std::vector< std::vector<double> > v, v_old;
	sim.copyVelocitiesTo(v);
	for (unsigned int i=0; i<v.size(); i++) {
		v_old.push_back(std::vector<double>(dim));
		for (int k=0; k<dim; k++) {
			v_old[i][k] = v[i][k];
		}
	}
	
	for (unsigned int i=0; i<v.size(); i++) {
		if (_rng.randDouble() < _coupling_strength) {
			for (int k=0; k<dim; k++) {
				v[i][k] += _max_perturbation*_rng.randNormal();
			}
		}
	}
	if (_zero_net_velocity) {
		std::vector<double> v_cm(dim, 0.0);
		for (unsigned int i=0; i<v.size(); i++) {
			for (int k=0; k<dim; k++) {
				v_cm[k] += v[i][k];
			}
		}
		for (int k=0; k<dim; k++) {
			v_cm[k] /= (double)v.size();
		}
		for (unsigned int i=0; i<v.size(); i++) {
			for (int k=0; k<dim; k++) {
				v[i][k] += v_cm[k];
			}
		}		
	}
		
	double e_new = sim.computeHamiltonian();
	double probability = exp(-beta*(e_new-e_old));
	
	//std::cerr << beta <<"\t" << e_old <<"\t" << e_new <<"\t" << probability << "\n";
	
	if(probability > _rng.randDouble()) {
		sim.setVelocities(v);
		return true;
	}
	else {
		return false;
	}
}
