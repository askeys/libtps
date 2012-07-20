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

#include "TpsRNG.h"
#include <cmath>
#include <cassert>

TpsRNG::TpsRNG() : _save(false)
{
}

TpsRNG::~TpsRNG()
{
}

double TpsRNG::randUniform(double min, double max)
{
	double range = max-min;
	return min + range*randDouble();
}

double TpsRNG::randNormal(double mean, double stdev)
{
	double first,v1,v2,rsq,fac;
	
	if (!_save) {
		int again = 1;
		while (again) {
			v1 = 2.0*randDouble()-1.0;
			v2 = 2.0*randDouble()-1.0;
			rsq = v1*v1 + v2*v2;
			if (rsq < 1.0 && rsq != 0.0) 
				again = 0;
			}
			fac = sqrt(-2.0*log(rsq)/rsq);
			_second = v1*fac;
			first = v2*fac;
			_save = true;
	}
	else {
		first = _second;
		_save = false;
	}
	return (mean + first*stdev);
}

double TpsRNG::randDouble()
{
	int virtual_function_was_overriden = 0;
	assert(virtual_function_was_overriden);
	return 0.0;
}

int TpsRNG::randInt()
{
	int virtual_function_was_overriden = 0;
	assert(virtual_function_was_overriden);
	return 0;
}
