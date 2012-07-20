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
 *  TpsTestPeSurf2d.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestPeSurf2d.h"
#include <iostream>

void default_force_function(double x, double y, double& fx, double& fy)
{
	double x2 = x*x;
	double y2 = y*y;
	double xpy2 =(x+y)*(x+y);

	double expo1 = exp(-(x-1)*(x-1)-y2);
	double expo2 = exp(-(x+1)*(x+1)-y2);
	double expo3 = exp(-0.32*(x2 + y2 +20*xpy2));

	fx = -( 2 * (x-1) * expo1 + 2 * (x+1) * expo2 - 
		1.6 * expo3 * (2*x+40*(x+y)) + (128./1875)*x*x2);


	fy = -(2 * y * expo1 + 2 * y * expo2 - 
		1.6 * expo3* (2*y+40*(x+y)) + (128./1875)*y*y2 -
		(8./15)*exp(-2- 4*y));
}

double default_potential_function(double x, double y)
{
	double x2, y2, v; //,ke;
	
	x2 = x*x;
	y2 = y*y;

	v = (2./15) *exp (-2 - 4 * y) -
	exp (-(x-1)*(x-1)-y2) - exp(-(x+1)*(x+1)-y2) +
	5*exp(-0.32*(x2 + y2 +20*(x+y)*(x+y))) + (32./1875)*(x2*x2 +y2*y2);
	
	return v;
}

TpsTestPeSurf2d::TpsTestPeSurf2d()
{	
	//these member variables are specific to the TpsTestPeSurf2d class
	_beta = 1.0;
	_current_timeslice.x[0] = 2.0;
	_current_timeslice.x[1] = 2.0;
	_current_timeslice.v[0] = 0.0;
	_current_timeslice.v[1] = 0.0;
	_potential_function = default_potential_function;
	_force_function = default_force_function;
}
		
void TpsTestPeSurf2d::invertVelocities()
{
	_current_timeslice.v[0] = -_current_timeslice.v[0];
	_current_timeslice.v[1] = -_current_timeslice.v[1];
}

int TpsTestPeSurf2d::getDimensions()
{
	return 2;
}

void TpsTestPeSurf2d::copyPositionsTo(std::vector< std::vector<double> >&x)
{
	x.resize(1);
	std::vector<double> xi(2, 0.0);
	xi[0] = _current_timeslice.x[0];
	xi[1] = _current_timeslice.x[1];
	x[0] = xi;
}

void TpsTestPeSurf2d::writeRestart(const char* filename)
{
	std::string timeslice_name = filename;
	_saved_timeslices[timeslice_name] = _current_timeslice;
}

void TpsTestPeSurf2d::readRestart(const char* filename)
{			
	std::string timeslice_name = filename;
	_current_timeslice = _saved_timeslices[timeslice_name];
}

void TpsTestPeSurf2d::freeRestart(const char* filename)
{
	std::string timeslice_name = filename;
	_saved_timeslices.erase(timeslice_name);
}

void TpsTestPeSurf2d::copyRestart(const char* src_file, const char* dest_file)
{
	std::string name1 = src_file;
	std::string name2 = dest_file;				
	_saved_timeslices[name2] = _saved_timeslices[name1];
}

double TpsTestPeSurf2d::computeHamiltonian()
{
	double x, y, vx, vy;

	x = _current_timeslice.x[0];
	y = _current_timeslice.x[1];
	vx = _current_timeslice.v[0];
	vy = _current_timeslice.v[1];
	
	double pe = computePotential(x,y);
	double ke = 0.5*(vx*vx + vy*vy);

	return pe + ke;
}

double TpsTestPeSurf2d::getExpectedTemperature()
{
	return 1.0 / _beta;
}

void TpsTestPeSurf2d::setTemperature(double temp)
{
	_beta = 1.0 / temp;
}

//everything defined below this point is not required...

void TpsTestPeSurf2d::setParticlePosition(double x, double y)
{
	_current_timeslice.x[0] = x;
	_current_timeslice.x[1] = y;
}

void TpsTestPeSurf2d::setParticleVelocity(double vx, double vy)
{
	_current_timeslice.v[0] = vx;
	_current_timeslice.v[1] = vy;
}

TpsTestPeSurf2dTimeslice& 
TpsTestPeSurf2d::getCurrentTimeslice()
{
	return _current_timeslice;
}

void TpsTestPeSurf2d::updateForce()
{
	double &x = _current_timeslice.x[0];
	double &y = _current_timeslice.x[1];
	double &fx = _current_timeslice.f[0];
	double &fy = _current_timeslice.f[1];
	computeForce(x, y, fx, fy);
}

double TpsTestPeSurf2d::computePotential(double x, double y)
{
	return _potential_function(x,y);
}

void TpsTestPeSurf2d::computeForce(
	double x, double y, double& fx, double& fy)
{
	_force_function(x, y, fx, fy);	
}

void TpsTestPeSurf2d::setPotentialField(
	double(*potential_function)(double, double), 
	void(*force_function)(double, double, double&, double&))
{
	_potential_function = potential_function;
	_force_function = force_function;
}

std::vector< std::vector<double> > TpsTestPeSurf2d::getPotentialField(
	double xmin, double xmax, double ymin, double ymax, double step)
{	
	std::vector< std::vector<double> > xy;	
	for (double x=xmin; x<=xmax; x+=step) {
		for (double y=ymin; y<=ymax; y+=step) {
			std::vector<double> tmp(3);
			tmp[0] = x;
			tmp[1] = y;
			tmp[2] = computePotential(x, y);
			xy.push_back(tmp);
		}
	}	
	return xy;
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS
double TpsTestPeSurf2dTimeslice::getPositionX()
{
	return x[0];
}

double TpsTestPeSurf2dTimeslice::getPositionY()
{
	return x[1];
}
#endif

