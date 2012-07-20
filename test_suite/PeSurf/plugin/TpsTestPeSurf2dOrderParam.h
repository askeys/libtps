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
 *  TpsTestPeSurf2dMD.h
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#ifndef PESURFORDERPARAM
#define PESURFORDERPARAM

#include "TpsOrderParameter.h"
#include "TpsTrajectory.h"
#include "TpsTestPeSurf2d.h"
/**
\brief A simple order parameter that tracks the x position of the particle in 
	the TpsTestPeSurf2d class of examples
\ingroup testsuite
*/
class TpsTestPeSurf2dOrderParamX : public TpsOrderParameter
{
	public:
		TpsTestPeSurf2dOrderParamX(double=2);

		void sampleA(TpsTestPeSurf2d&, int=1000, int=10);
		void sampleB(TpsTestPeSurf2d&, int=1000, int=10 );
		
		bool hA(TpsTrajectory&);		
		bool hB(TpsTrajectory&);
		double evaluate(TpsTrajectory&, int);
	
	protected:
		double _nstdev;
		double _xA, _xB;
		double _stdev_xA, _stdev_xB;
};

/**
\brief A simple order parameter that tracks the y position of the particle in 
	the TpsTestPeSurf2d class of examples
\ingroup testsuite
*/
class TpsTestPeSurf2dOrderParamY : public TpsOrderParameter
{
	public:
		TpsTestPeSurf2dOrderParamY(double=2);

		void sampleA(TpsTestPeSurf2d&, int=1000, int=10);
		void sampleB(TpsTestPeSurf2d&, int=1000, int=10);

		bool hA(TpsTrajectory&);		
		bool hB(TpsTrajectory&);
		double evaluate(TpsTrajectory&, int);
	
	protected:
		double _nstdev;
		double _yA, _yB;
		double _stdev_yA, _stdev_yB;


};

#endif
