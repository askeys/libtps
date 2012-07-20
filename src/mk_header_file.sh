#!/bin/sh

echo "
#ifndef TPS_H
#define TPS_H

#include \"config.h\"
" > tps.h

for i in $(ls Tps*.h); do
	echo "#include \"$i\"" >> tps.h
done

echo "
#endif" >> tps.h
