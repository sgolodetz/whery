#! /bin/bash -e

LOG=../build-boost_1_53_0.log

echo "[whery] Building Boost 1.53.0"

if [ -d boost_1_53_0 ]
then
	echo "[whery] ...Skipping build (already built)"
	exit
fi

if [ -d boost-setup ]
then
	echo "[whery] ...Skipping archive extraction (already extracted)"
else
	echo "[whery] ...Extracting archive..."
	/bin/rm -fR tmp
	mkdir tmp
	cd tmp
	tar xzf ../setup/boost_1_53_0/boost_1_53_0.tar.gz
	cd ..
	mv tmp/boost_1_53_0 boost-setup
	rmdir tmp
fi

cd boost-setup

if [ -e b2 ]
then
	echo "[whery] ...Skipping bootstrapping (b2 already exists)"
else
	echo "[whery] ...Bootstrapping..."
	./bootstrap.sh > $LOG
fi

echo "[whery] ...Running build..."
./b2 -j2 --libdir=../boost_1_53_0/lib --includedir=../boost_1_53_0/include --abbreviate-paths --with-chrono --with-date_time --with-filesystem --with-test --with-thread --build-type=complete --layout=tagged toolset=darwin install >> $LOG

echo "[whery] ...Finished building Boost 1.53.0."
