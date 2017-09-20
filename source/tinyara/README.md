# Tizen RT

[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)

lightweight RTOS-based platform to support low-end IoT devices.

Please find project details on our [Tizen wiki](https://wiki.tizen.org/wiki/Tizen_RT).

## Quick Start
### Getting the toolchain

Get the build in binaries and libraries, [gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update)

Untar the gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar and export the path like

```bash
tar xvf gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar
export PATH=<Your Toolchain PATH>:$PATH
```

### Getting the sources

```bash
git clone https://github.com/Samsung/TizenRT.git
cd TizenRT
TIZENRT_BASEDIR="$PWD"
```

### How to Build

Configure the build from $TIZENRT_BASEDIR/os/tools directory
```bash
cd os/tools
./configure.sh <board>/<configuration_set>
```
For list of boards and configuration set supported, refer belows.

Above copies the canned configuration-set for the particular board, into the $TIZENRT_BASEDIR/os directory.

Configuration can be modified through make menuconfig from $TIZENRT_BASEDIR/os.
```bash
cd ..
make menuconfig
```

Finally, initiate build by make from $TIZENRT_BASEDIR/os
```bash
make
```

Built binaries are in $TIZENRT_BASEDIR/build/output/bin.

## Supported Board

ARTIK053 [[details]](build/configs/artik053/README.md)

sidk_s5jt200 [[details]](build/configs/sidk_s5jt200/README.md)

Tizen RT currently supports only two boards called artik053 and sidk_s5jt200.
However, those are not available in public markets till now.
sidk_s5jt200 or other boards for Tizen RT will be coming soon.

## Configuration Sets

To build a Tizen RT application, use the default configuration files named 'defconfig' under 'build/configs/<board>/' folder.

To customize your application with specific configuration settings, using the menuconfig tool is recommended  at os folder as shown:
```bash
make menuconfig
```
Please keep in mind that we are actively working on board configurations, and will be posting our updates on the README files under each config

