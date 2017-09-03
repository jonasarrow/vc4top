## VC4TOP

A performance monitoring tool for the VC4 GPU.

This GPU is prominently featured in the Raspberry Pi devices.

Needs to be run as root.

### Compile
To compile, use cmake

    cmake . && make && sudo make install/strip
    sudo vc4top

To generate a debian `deb` file, use

    cmake . && make && make package

This generates `vc4top-${VERSION}-Linux.deb`
### Requirements
gtk-3.0 and a recent enough gcc is necessary.

### Todo
* ~~Add installer script/debian package creator~~
	* Get it to a quality where it might be considered to be included into raspbian/...
* Make out-of-tree builds possible
* Various constants in the source should not be hardcoded and adjustable
* Add all those other performance counter sources
* Make the interface much more configurable
	* e.g. select the counters you want to show
	* Then pay attention which most likely need to be counted together
* Make the graphs nicer (library?)

* Is there an error how we count or is the GPU (QPU) truly this underused all the time?
