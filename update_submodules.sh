# This script pulls up the project submodules to the expected commits. We can't do a typical
# "git submodule update --init --recursive" because the tinyusb submodule in the PicoSDK will
# clone down a bunch of unnecessary drivers. This severely impacts the performance of VSCode
# git tooling.
ICHNAEA_ROOT=$(pwd)

echo "Initializing core submodules"
git submodule update --init

echo "Updating CppUTest"
cd $ICHNAEA_ROOT/lib/cpputest
git submodule update --init --recursive

echo "Updating FreeRTOS"
cd $ICHNAEA_ROOT/lib/freertos-kernel
git submodule update --init

echo "Updating MbedUtils"
cd $ICHNAEA_ROOT/lib/mbedutils
git submodule update --init

echo "Updating PicoMock"
cd $ICHNAEA_ROOT/lib/pico-mock
git submodule update --init --recursive

echo "Updating PicoSDK"
cd $ICHNAEA_ROOT/lib/pico-sdk
git submodule update --init
