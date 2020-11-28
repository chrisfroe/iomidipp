$env:INSTALL_PREFIX="$(pwd)\$env:REL_INSTALL_PREFIX"
$env:BUILD_DIR="$(pwd)\$env:REL_BUILD_DIR"
$env:SOURCE_DIR="$(pwd)\$env:REL_SOURCE_DIR"
echo "install prefix: $env:INSTALL_PREFIX"
echo "build dir: $env:BUILD_DIR"
echo "source dir: $env:SOURCE_DIR"
mkdir $env:REL_INSTALL_PREFIX
mkdir $env:REL_INSTALL_PREFIX/bin
mkdir $env:REL_INSTALL_PREFIX/lib
mkdir $env:REL_BUILD_DIR
cd $env:BUILD_DIR
cmake -DIOMIDIPP_CREATE_TEST_TARGET=BOOL:ON -DCMAKE_INSTALL_PREFIX="$env:INSTALL_PREFIX" -DCMAKE_BUILD_TYPE=Release $env:SOURCE_DIR
cmake --build .
cmake --build . --target install
cd $env:INSTALL_PREFIX/bin
$env:err_code=1
./iomidipp_tests.exe
$env:err_code=$?
exit $env:err_code
