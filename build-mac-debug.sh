#! /bin/bash -e

cd libraries
./build-boost_1_53_0-mac.sh
cd ..

echo "[whery] Building whery"

if [ ! -d build ]
then
	echo "[whery] ...Configuring using CMake..."
	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/Users/stuart/programs/whery/install ../source
	cd ..
fi

cd build

echo "[whery] ...Running build..."
make

echo "[whery] ...Finished building whery."
