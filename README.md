## Requirements
- QT
- OpenCV (see https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows)
	- additional problems might pop up if Cygwin or some other shell-exposing directory is in system path
		- windres and/or other programs might be found in directories other than the QT MinGW directory

## Building and running
- modify .pro file to point library paths to where you installed OpenCV
- put the OpenCV library directory in system path
- if you get error code 0xc0000135 when running, put cbw32.dll and cbw64.dll in the output build directory (build-LKTracking-.../debug or release)