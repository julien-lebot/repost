@echo off

if not defined DevEnvDir (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86
	REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
)
cd %~dp0\protobuf
if not exist gmock (
	git clone -b release-1.7.0 https://github.com/google/googlemock.git gmock
	cd gmock
	git clone -b release-1.7.0 https://github.com/google/googletest.git gtest
	cd ..
)
cd cmake
if not exist build (
	mkdir build
)
cd build
cmake -G "Visual Studio 15 2017" -DCMAKE_INSTALL_PREFIX=../../../install ..
msbuild protoc.vcxproj /p:Configuration=Debug
msbuild protoc.vcxproj /p:Configuration=Release
