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


#include "TpsAnalysisWriteXYZAtomic.h"
#include "TpsSimulationPluginAtomic.h"
#include "TpsTrajectory.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

TpsAnalysisWriteXYZAtomic::TpsAnalysisWriteXYZAtomic(
	const char* filename, int step, std::vector<int> types
)
:	_filename(filename), 
	_step_size(step),
	_user_types(types),
	_use_wildcard(false)
{
	for (unsigned int i=0; i<_filename.size(); i++) {
		if (_filename[i] == '%') {
			_filename[i] = ' ';
			_use_wildcard = true;
		}
	}
	
	if (_use_wildcard) {
		std::istringstream is(_filename);
		is >> _prefix >> _postfix;
	}
	
	if (_filename != "stdout" && _filename != "stderr") {
		std::ofstream os(filename);
		os.close();
	}

}

TpsAnalysisWriteXYZAtomic::~TpsAnalysisWriteXYZAtomic()
{
}

void TpsAnalysisWriteXYZAtomic::analyze(TpsTrajectory& traj)
{	
	std::ostream *os = NULL;
	std::ofstream file;
	
	int num = traj.getID();
	
	if (_use_wildcard) {
		std::ostringstream filename;
		filename << _prefix << num << _postfix;
		file.open(filename.str().c_str());
		assert(file.good());
		os = &file;
	}
	else {
		if (_filename == "stdout") {
			os = &std::cout;
		}
		else if (_filename == "stderr") {
			os = &std::cerr;
		}
		else {
			file.open(_filename.c_str(), std::ios::app);
			os = &file;
		}
	}
	
	for (TpsTrajectoryIterator i=traj.begin(); i!=traj.end(); i+=_step_size) {
		
		TpsSimulationPluginAtomic& sim = 
			i.getSimulationPlugin().safeDowncastToAtomicPlugin();
		
		std::vector<int> type = _user_types;
		
		if (_user_types.size() == 0) {
			sim.copyTypesTo(type);
		}

		std::vector< std::vector<double> > x;
		sim.copyPositionsTo(x);

		int natom = x.size();
		if (natom == 0) {
			return;
		}
		
		int dim = x[0].size();

		if (type.size() != (unsigned int)natom) {
			type.resize(natom);
		}
		
		assert(os->good());
		*os << natom << "\n\n";
		for (int i=0; i<natom; i++) {
			*os	<< (type)[i] << "\t";
			if (dim == 3) {
				*os << x[i][0] << "\t" << x[i][1] << "\t" << x[i][2] << "\n";
			}
			else if (dim == 2) {
				*os << x[i][0] << "\t" << x[i][1] << "\t" << 0.0 << "\n";
			}
			else if (dim == 1) {
				*os << x[i][0] << "\t" << 0.0 << "\t" << 0.0 << "\n";
			}
		}
	}
}
