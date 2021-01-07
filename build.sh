rm -rf build
mkdir "build"
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_ENV=rpi_armv7l ..
make -j4