echo "
/* File: tps.i */
%module(directors=\"1\") tps
%{
	#include \"../../CXX/tps.h\"
%}

%include \"std_vector.i\"
%include \"std_string.i\"
namespace std
{
	%template(stlVectorInt) vector<int>;
	%template(stlVectorBool) vector<bool>;
	%template(stlVectorDouble) vector<double>;
	%template(stlVectorDouble2D) vector< vector<double> >;
}

%typemap(out) string * {
   \$result = PyString_FromString(\$1->c_str());
}

%feature(\"director\");
%feature(\"director:except\") {
    if (\$error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
%exception {
    try { \$action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}
"
for i in $(ls ../../CXX/Tps*.h); do
	echo "%include \"$i\""
done

#for i in $(ls ../../CXX/TestSystems/*.h); do
#	echo "%include \"$i\""
#done
