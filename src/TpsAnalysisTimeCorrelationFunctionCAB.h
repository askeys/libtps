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


#ifndef TPSANALYSISTIMECORRELATIONFUNCTIONCAB
#define TPSANALYSISTIMECORRELATIONFUNCTIONCAB

#include "TpsAnalysis.h"
#include <string>
#include <map>

class TpsOrderParameter;


/**
\brief Computes the time correlation function \f$C_{AB}(t)\f$
\warning You must use the HB acceptance criterion rather than hB when creating 
the path ensemble unless there is 0% chance of returning from B once A->B.
\ingroup analysis


*/
class TpsAnalysisTimeCorrelationFunctionCAB : public TpsAnalysis
{
	public:
		TpsAnalysisTimeCorrelationFunctionCAB(
			TpsOrderParameter&, const char* ="CAB_vs_t.txt", double=1.0, int=1);
		virtual ~TpsAnalysisTimeCorrelationFunctionCAB();

		virtual void analyze(TpsTrajectory&);
		
	protected:
		TpsOrderParameter& _order_parameter;
		std::string _filename;
		double _time_bin_size;
		int _write_time;
		int _times;
		std::map<int, double> _cab;
		std::map<int, double> _count;
};

#endif
