
/* File: tps.i */
%module(directors="1") tps
%{
	#include "../../CXX/tps.h"
%}

%include "std_vector.i"
%include "std_string.i"
namespace std
{
	%template(stlVectorInt) vector<int>;
	%template(stlVectorBool) vector<bool>;
	%template(stlVectorDouble) vector<double>;
	%template(stlVectorDouble2D) vector< vector<double> >;
}

%typemap(out) string * {
   $result = PyString_FromString($1->c_str());
}

%feature("director");
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}

%include "../../CXX/TpsAlgorithm.h"
%include "../../CXX/TpsAlgorithmAimlessShooting.h"
%include "../../CXX/TpsAlgorithmBruteForce.h"
%include "../../CXX/TpsAlgorithmFFS.h"
%include "../../CXX/TpsAlgorithmSEnsemble.h"
%include "../../CXX/TpsAlgorithmTIS.h"
%include "../../CXX/TpsAlgorithmTPS.h"
%include "../../CXX/TpsAnalysis.h"
%include "../../CXX/TpsAnalysisCommitter.h"
%include "../../CXX/TpsAnalysisDistinguishingReactionCoordinate.h"
%include "../../CXX/TpsAnalysisTimeCorrelationFunctionC.h"
%include "../../CXX/TpsAnalysisTimeCorrelationFunctionCAB.h"
%include "../../CXX/TpsAnalysisTransitionStates.h"
%include "../../CXX/TpsAnalysisTransitionTimes.h"
%include "../../CXX/TpsAnalysisWriteCoordsAtomic.h"
%include "../../CXX/TpsAnalysisWriteOrderParameter.h"
%include "../../CXX/TpsAnalysisWriteTrajectory.h"
%include "../../CXX/TpsAnalysisWriteXYZAtomic.h"
%include "../../CXX/TpsInitializer.h"
%include "../../CXX/TpsInitializerBruteForce.h"
%include "../../CXX/TpsInitializerExplicitTrajectory.h"
%include "../../CXX/TpsInitializerMidpointTS.h"
%include "../../CXX/TpsInitializerTransitionState.h"
%include "../../CXX/TpsOrderParameter.h"
%include "../../CXX/TpsOrderParameterAggregate.h"
%include "../../CXX/TpsRNG.h"
%include "../../CXX/TpsRNG48.h"
%include "../../CXX/TpsSimulationPlugin.h"
%include "../../CXX/TpsSimulationPluginAtomic.h"
%include "../../CXX/TpsSimulationPluginLAMMPS.h"
%include "../../CXX/TpsSimulationPluginLattice.h"
%include "../../CXX/TpsTestDiatomicLAMMPS.h"
%include "../../CXX/TpsTestDiatomicOrderParam.h"
%include "../../CXX/TpsTestPeSurf2d.h"
%include "../../CXX/TpsTestPeSurf2dBD.h"
%include "../../CXX/TpsTestPeSurf2dMC.h"
%include "../../CXX/TpsTestPeSurf2dMD.h"
%include "../../CXX/TpsTestPeSurf2dOrderParam.h"
%include "../../CXX/TpsTestRandomWalk.h"
%include "../../CXX/TpsTestRandomWalkOrderParam.h"
%include "../../CXX/TpsTimeslice.h"
%include "../../CXX/TpsTimesliceModifier.h"
%include "../../CXX/TpsTimesliceModifierBoltzmannVdist.h"
%include "../../CXX/TpsTimesliceModifierPerturbVelocities.h"
%include "../../CXX/TpsTrajectory.h"
%include "../../CXX/TpsTrajectoryContinuous.h"
%include "../../CXX/TpsTrajectoryEnsemble.h"
%include "../../CXX/TpsTrajectoryIterator.h"
%include "../../CXX/TpsTrajectoryRandomStep.h"
%include "../../CXX/TpsTrajectoryUniformStep.h"
%include "../../CXX/TpsTrajectoryUserDefined.h"
%include "../../CXX/TpsTrialMove.h"
%include "../../CXX/TpsTrialMovePathReversal.h"
%include "../../CXX/TpsTrialMoveShift.h"
%include "../../CXX/TpsTrialMoveShiftBackward.h"
%include "../../CXX/TpsTrialMoveShiftForward.h"
%include "../../CXX/TpsTrialMoveShot.h"
%include "../../CXX/TpsTrialMoveShotAimless.h"
%include "../../CXX/TpsTrialMoveShotBackward.h"
%include "../../CXX/TpsTrialMoveShotFlexible.h"
%include "../../CXX/TpsTrialMoveShotFlexibleV2.h"
%include "../../CXX/TpsTrialMoveShotForward.h"
