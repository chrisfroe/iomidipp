#!/usr/bin/env bash
echo "pwd: $(pwd)"
INSTALL_PREFIX="$(pwd)/${REL_INSTALL_PREFIX}"
BUILD_DIR="$(pwd)/${REL_BUILD_DIR}"
SOURCE_DIR="$(pwd)/${REL_SOURCE_DIR}"
echo "install prefix: ${INSTALL_PREFIX}"
echo "build dir: ${BUILD_DIR}"
echo "source dir: ${SOURCE_DIR}"
mkdir "${REL_INSTALL_PREFIX}"
mkdir "${REL_INSTALL_PREFIX}"/bin
mkdir "${REL_INSTALL_PREFIX}"/lib
mkdir "${REL_BUILD_DIR}"
echo "ls: $(ls)"
cd "${BUILD_DIR}" || exit
cmake -DIOMIDIPP_CREATE_TEST_TARGET=BOOL:ON -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" -DCMAKE_BUILD_TYPE=Release "${SOURCE_DIR}"
cmake --build .
cmake --build . --target install
cd "${INSTALL_PREFIX}"/bin || exit
err_code=0
ret_code=0
./iomidipp_tests
err_code=$?
echo "err_code ${err_code}"
echo "ret_code ${ret_code}"
if [ ${err_code} -ne 0 ]; then
    ret_code=${err_code}
    echo "tests failed with ${ret_code}"
fi
echo "err_code ${err_code}"
echo "ret_code ${ret_code}"
exit ${ret_code}
