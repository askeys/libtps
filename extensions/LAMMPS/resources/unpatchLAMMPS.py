import shutil
import os

if os.path.exists('src/fix_nvt.h.backup.0'):
	shutil.copyfile('src/fix_nvt.h.backup.0', 'src/fix_nvt.h')

if os.path.exists('src/fix_npt.h.backup.0'):
	shutil.copyfile('src/fix_npt.h.backup.0', 'src/fix_npt.h')

if os.path.exists('src/fix_nvt.cpp.backup.0'):
	shutil.copyfile('src/fix_nvt.cpp.backup.0', 'src/fix_nvt.cpp')

if os.path.exists('src/fix_npt.cpp.backup.0'):
	shutil.copyfile('src/fix_npt.cpp.backup.0', 'src/fix_npt.cpp')
