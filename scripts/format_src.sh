#!/bin/bash

# directories to be formatted (recursive search)
DIRS="HighMap/include HighMap/src examples tests"
FORMAT_CMD="clang-format -style=file:scripts/clang_style -i"

echo "- clang-format"

# format opencl kernels
for D in ${DIRS}; do
    for F in `find ${D}/. -type f -iname \*.cl`; do
	echo ${F}
	sed '1d;$d' ${F} > ${F}_tmp
	${FORMAT_CMD} ${F}_tmp
	sed -i '1s/^/R""(\n/' ${F}_tmp
	echo ')""' >> ${F}_tmp
	mv ${F}_tmp ${F}
    done
done

# format C++
for D in ${DIRS}; do
    for F in `find ${D}/. -type f \( -iname \*.hpp -o -iname \*.cpp \)`; do
	echo ${F}
	${FORMAT_CMD} ${F}
    done
done


# format cmake files
echo "- cmake-format"

cmake-format -i CMakeLists.txt HighMap/CMakeLists.txt external/CMakeLists.txt external/*.cmake

