import shutil
import os

i=0
while os.path.exists('src/fix_nvt.h.backup.' + `i`):
	i=i+1
shutil.copyfile('src/fix_nvt.h', str('src/fix_nvt.h.backup.' + `i`))

f1i=open('src/fix_nvt.h.backup.0', 'r')
f1o=open('src/fix_nvt.h', 'w');

for line in f1i:
	if line.find('protected:') != -1:
		print >> f1o, '//--TPS--\n'
		print >> f1o,  '  void invert_velocities();'
		print >> f1o, '  void get_restart_info(double*);'
		print >> f1o, '  void set_restart_info(double*);\n'		
		print >> f1o, '//--TPS--\n\n'
	print >> f1o, line,

f1i.close()
f1o.close()

############

i=0
while os.path.exists('src/fix_npt.h.backup.' + `i`):
	i=i+1
shutil.copyfile('src/fix_npt.h', str('src/fix_npt.h.backup.' + `i`))

f2i=open('src/fix_npt.h.backup.0', 'r')
f2o=open('src/fix_npt.h', 'w');

for line in f2i:
	if line.find('protected:') != -1:
		print >> f2o, '//--TPS--\n'
		print >> f2o,  '  void invert_velocities();'
		print >> f2o, '  void get_restart_info(double*);'
		print >> f2o, '  void set_restart_info(double*);\n'		
		print >> f2o, '//--TPS--\n\n'
	print >> f2o, line,

f2i.close()
f2o.close()

############
i=0
while os.path.exists('src/fix_nvt.cpp.backup.' + `i`):
	i=i+1
shutil.copyfile('src/fix_nvt.cpp', str('src/fix_nvt.cpp.backup.' + `i`))
shutil.copyfile('src/fix_nvt.cpp.backup.0', 'src/fix_nvt.cpp')

f3o=open('src/fix_nvt.cpp', 'a');
print >> f3o, '\n//--TPS--\n'
print >> f3o, 'void FixNVT::invert_velocities()'
print >> f3o, '{'
print >> f3o, '  eta_dot = -1.0*eta_dot;'
print >> f3o, '}\n'

print >> f3o, 'void FixNVT::get_restart_info(double *list)'
print >> f3o, '{'
print >> f3o, '  list[0] = eta;'
print >> f3o, '  list[1] = eta_dot;'
print >> f3o, '}\n'

print >> f3o, 'void FixNVT::set_restart_info(double *list)'
print >> f3o, '{'
print >> f3o, '  eta = list[0];'
print >> f3o, '  eta_dot = list[1];'
print >> f3o, '}\n'

print >> f3o, '//--TPS--\n'
f3o.close()

############
i=0
while os.path.exists('src/fix_npt.cpp.backup.' + `i`):
	i=i+1
shutil.copyfile('src/fix_npt.cpp', str('src/fix_npt.cpp.backup.' + `i`))
shutil.copyfile('src/fix_npt.cpp.backup.0', 'src/fix_npt.cpp')

f4o=open('src/fix_npt.cpp', 'a');
print >> f4o, '\n//--TPS--\n'
print >> f4o, 'void FixNPT::invert_velocities()'
print >> f4o, '{'
print >> f4o, '  eta_dot = -1.0*eta_dot;'
print >> f4o, '  omega_dot[0] = -1.0*omega_dot[0];'
print >> f4o, '  omega_dot[1] = -1.0*omega_dot[1];'
print >> f4o, '  omega_dot[2] = -1.0*omega_dot[2];'
print >> f4o, '}\n'

print >> f4o, 'void FixNPT::get_restart_info(double* list)'
print >> f4o, '{'
print >> f4o, '  int n = 0;'
print >> f4o, '  list[n++] = eta;'
print >> f4o, '  list[n++] = eta_dot;'
print >> f4o, '  list[n++] = omega[0];'
print >> f4o, '  list[n++] = omega[1];'
print >> f4o, '  list[n++] = omega[2];'
print >> f4o, '  list[n++] = omega_dot[0];'
print >> f4o, '  list[n++] = omega_dot[1];'
print >> f4o, '  list[n++] = omega_dot[2];'
print >> f4o, '}\n'

print >> f4o, 'void FixNPT::set_restart_info(double* list)'
print >> f4o, '{'
print >> f4o, '  int n = 0;'
print >> f4o, '  eta = list[n++];'
print >> f4o, '  eta_dot = list[n++];'
print >> f4o, '  omega[0] = list[n++];'
print >> f4o, '  omega[1] = list[n++];'
print >> f4o, '  omega[2] = list[n++];'
print >> f4o, '  omega_dot[0] = list[n++];'
print >> f4o, '  omega_dot[1] = list[n++];'
print >> f4o, '  omega_dot[2] = list[n++];'
print >> f4o, '}\n'

print >> f4o, '//--TPS--\n'
f4o.close()
