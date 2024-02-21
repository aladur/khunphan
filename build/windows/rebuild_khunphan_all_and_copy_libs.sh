#!/usr/bin/sh
#
# syntax:
#    rebuild_khunphan_all_and_copy_libs.sh
# parameters:
#    none
#

# Create rebuild BATCH script file for all combinations of
# platform (Win32/x64) and configuration (Debug/Release).
# Parameters:
#  $1 MSVC solution file (*.sln)
#  $2 result BATCH script file
function create_rebuild_script() {
    echo \@echo off >$2
    echo set DevEnvDir=$VS140COMNTOOLS\..\\IDE >>$2
    echo \"\%DevEnvDir%\\devenv.exe\" $1 /Rebuild \"Release\|Win32\" >>$2
    echo \"\%DevEnvDir%\\devenv.exe\" $1 /Rebuild \"Release\|x64\" >>$2
    echo \"\%DevEnvDir%\\devenv.exe\" $1 /Rebuild \"Debug\|Win32\" >>$2
    echo \"\%DevEnvDir%\\devenv.exe\" $1 /Rebuild \"Debug\|x64\" >>$2
}

if [ ! -d lib ]; then
    echo "**** First call download_and_rebuild_libs.sh to create libraries!"
    exit 1
fi

echo === build khunphan...
cd ../..
# Create and execute a batch file to build all four configurations
create_rebuild_script khunphan.sln rebuild.bat
$COMSPEC //C rebuild.bat

# check existence of result files and copy them to corresponding
# khunphan build directory

echo === copy libraries...
for platform in Win32 x64;
do
    if [ ! -d $platform ]; then mkdir $platform; fi
    for configuration in Debug Release;
    do
        if [ ! -d $platform/$configuration ]; then
            mkdir $platform/$configuration
        fi
        cp -f build/windows/lib/${platform}/${configuration}/*.dll $platform/$configuration
    done
done
cd build/windows
