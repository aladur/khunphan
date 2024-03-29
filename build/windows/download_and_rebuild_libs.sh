#!/usr/bin/sh
#
# syntax:
#    download_and_rebuild_libs.sh [-d]
# parameters:
#   -d:    delete packages before downloading them
#

delete=
MSBUILDDISABLENODEREUSE=1
export MSBUILDDISABLENODEREUSE

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

# Copy libraries
# Parameters:
#   $1 list of library files
function copy_libraries() {
    libraries=("$@")
    fail="no"
    if [ ! -d lib ]; then
        mkdir lib
    fi

    # check existence of library files and copy them to lib directory
    for platform in Win32 x64;
    do
        if [ ! -d lib/$platform ]; then mkdir lib/$platform; fi
        for configuration in Debug Release;
        do
            if [ ! -d lib/$platform/$configuration ]; then
                mkdir lib/$platform/$configuration
            fi
            echo "Platform:" $platform " Configuration:" $configuration
            for librarypath in ${libraries[@]};
            do
                file=$(basename $librarypath)
                if [ $configuration = Debug ]; then
                    nameonly=${file%%.*}
                    if [ $nameonly = freeglut ]; then
                        file=`echo $file | sed -ne "s/\(freeglut\)\(\.[a-z]\+\)/\1d\2/p"`
                    fi
                fi
                srcpath=$(dirname $librarypath)/${platform}/${configuration}
                if [ ! -f $srcpath/$file ]; then
                    echo "*** Missing file: $srcpath/$file"
                    fail="yes"
                else
                    echo "   copying $file"
                    cp -f    $srcpath/$file lib/${platform}/${configuration}/
                fi
            done
        done
    done

    if [ "$fail" = "yes" ]; then
        exit 2
    fi
}

# Copy glew libraries
# The glew libraries have to be handled slightly different than the other
# libraries
# Parameters:
#   none
function copy_glew_libraries() {
    fail="no"
    if [ ! -d lib ]; then
        mkdir lib
    fi

    for platform in Win32 x64;
    do
        if [ ! -d lib/$platform ]; then mkdir lib/$platform; fi
        for configuration in Debug Release;
        do
            if [ ! -d lib/$platform/$configuration ]; then
                mkdir lib/$platform/$configuration
            fi
            echo "Platform:" $platform " Configuration:" $configuration
            for ext in lib dll;
            do
                if [ $ext = dll ]; then
                    srcpath=glew/bin/$configuration/$platform
                else
                    srcpath=glew/lib/$configuration/$platform
                fi
                file=glew32
                if [ $configuration = Debug ]; then file=${file}d; fi
                if [ ! -f $srcpath/$file.$ext ]; then
                    echo "*** Missing file: $srcpath/$file.$ext"
                else
                    echo "   copying $file.$ext..."
                    cp -f $srcpath/$file.$ext lib/${platform}/${configuration}/
                fi
            done
        done
    done

    if [ "$fail" = "yes" ]; then
        exit 2
    fi
}

while :
do
    case "$1" in
    	-d) delete=yes;;
    	--) shift; break;;
    	*) break;;
    esac
    shift
done

wgetpath=`which wget 2>/dev/null`
if [ "x$wgetpath" = "x" ]; then
    echo "*** Error: wget not found."
    echo "  wget can be downloaded from"
    echo "  https://sourceforge.net/projects/mingw/Other/mingwPORT/Current%20Release/"
    echo "  The executable has to be copied into the git installation"
    echo "  mingw32/bin or mingw64/bin."
fi

urls="
    http://downloads.sourceforge.net/freeglut/freeglut-3.0.0.tar.gz
    http://downloads.sourceforge.net/glew/glew-1.13.0.tgz
    http://zlib.net/zlib-1.3.1.tar.gz
    http://downloads.sourceforge.net/libpng/libpng-1.6.23.tar.gz
    ftp://xmlsoft.org/libxml2/libxml2-2.9.4.tar.gz
    http://www.libsdl.org/release/SDL2-2.0.4.tar.gz
    http://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz"

libiconvgitsrc=https://github.com/winlibs/libiconv.git

# Option -d: delete previously downloaded packages and intermediate directories
if [ "$delete" = "yes" ]; then
    echo deleting all...
    for url in $urls;
    do
        file=$(basename "$url")
        if [ -r $file ]; then
            echo deleting $file...
            rm -f $file
        fi
        directory=`echo "$file" | sed -e "s/\([A-Za-z0-9_]\+\)-.*/\1/"`
        if [ -d $directory ]; then
            echo deleting $directory...
            rm -rf $directory
        fi
    done
    rm -rf lib
    rm -rf libiconv
fi

# Download files (Only if package not already downloaded or deleted before)
for url in $urls;
do
    file=$(basename "$url")
    if [ ! -r $file ]; then
	    protocol=${url:0:3}
		if [ $protocol = "ftp" ]; then
		    wget --passive-ftp $url
		else
            wget $url
		fi
        if [ $? != 0 ]; then
            exit 1
        fi
    fi
done

# Unpacking files
# Supported extensions: tar.gz, tar.xz or tgz
for url in $urls;
do
    file=$(basename "$url")
    if [ -r $file ]; then
       extension=`echo "$file" | sed -e "s/.\+\(tgz\|tar\.gz\|tar\.xz\)/\1/"`
       filebase=`echo "$file" | sed -e "s/\(.\+\)\.\(tgz\|tar\.gz\|tar\.xz\)/\1/"`
       directory=`echo "$file" | sed -e "s/\([A-Za-z0-9_]\+\)-.*/\1/"`
       exclude=""
       if [ "$directory" = "SDL2_mixer" ]; then
           exclude="--exclude=${filebase}/Xcode"
       fi
       if [ ! -d $directory ]; then
           echo unpacking $file...
           case "$extension" in
               tar.gz) tar xfz $file $exclude ;;
               tgz) tar xfz $file $exclude ;;
               tar.xz) tar xfJ $file $exclude ;;
           esac
           # Wait for 2 seconds otherwise the mv could fail
           sleep 2
           mv $filebase $directory
       fi
    fi
done

# libiconv copy source from gitlab
if [ ! -d libiconv ]; then
    git clone $libiconvgitsrc libiconv
fi

# libglew modify project and compile with VS2015:
if [ -d glew ]; then
    cd glew/build
    rebuild=`test -d vc14 ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build libglew...
        cp -rp vc12 vc14
        cd vc14
        sed -i -e "s/12\.0/14.0/" *.vcxproj
        sed -i -e "s/v120/v140/" *.vcxproj
        create_rebuild_script glew.sln rebuild.bat
        $COMSPEC //C rebuild.bat
        cd ..
    fi
    cd ../..
    if [ $rebuild != 0 ]; then
        copy_glew_libraries
    fi
fi

# freeglut copy project and compile with VS2015:
if [ -d freeglut ]; then
    cd freeglut
    rebuild=`test -f config.h ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build freeglut...
        # Copy predefined Visual Studio 2015 solution and project
        cp ../freeglut.sln .
        cp ../freeglut.vcxproj .
        # Get Version number from CMakeLists.txt and set it in freeglut.rc, freeglutdll.def and fg_version.h
        vmajor=`sed -n -e "s/set(VERSION_MAJOR\s\+\([0-9]\+\))/\1/p" CMakeLists.txt`
        vminor=`sed -n -e "s/set(VERSION_MINOR\s\+\([0-9]\+\))/\1/p" CMakeLists.txt`
        vpatch=`sed -n -e "s/set(VERSION_PATCH\s\+\([0-9]\+\))/\1/p" CMakeLists.txt`
        sed -e "s/@VERSION_MAJOR@/$vmajor/g" freeglut.rc.in >freeglut.rc
        sed -i -e "s/@VERSION_MINOR@/$vminor/g" freeglut.rc
        sed -i -e "s/@VERSION_PATCH@/$vpatch/g" freeglut.rc
        sed -e "s/@VERSION_MAJOR@/$vmajor/g" src/fg_version.h.in >src/fg_version.h
        sed -i -e "s/@VERSION_MINOR@/$vminor/g" src/fg_version.h
        sed -i -e "s/@VERSION_PATCH@/$vpatch/g" src/fg_version.h
        sed -e "s/@VERSION_MAJOR@/$vmajor/g" src/freeglutdll.def.in >src/freeglutdll.def
        sed -i -e "s/@VERSION_MINOR@/$vminor/g" src/freeglutdll.def
        # Prepare config.h, on Windows unistd.h, sys/time.h and sys/param.h is not available
        sed -e "s/#cmakedefine/#define/" config.h.in >config.h
        sed -i -e "s-\(#define HAVE_UNISTD_H\)-\/*\1*\/-" config.h
        sed -i -e "s-\(#define HAVE_SYS_TIME_H\)-\/*\1*\/-" config.h
        sed -i -e "s-\(#define HAVE_SYS_PARAM_H\)-\/*\1*\/-" config.h
        # A *.def file is sufficient as __declspec(dllexport).
        # Especially on x64 there would be a warning otherwise.
        sed -i -e "s-\(#\s\+define\s\+FGAPI\)\s\+\([a-z(_]\+ex[a-z)]\+\)-/* \2 is already defined by *.DEF file.*/\n/* See also: https://support.microsoft.com/en\-us/kb/835326 */\n/*\1 \2*/\n\1-" include/GL/freeglut_std.h
        create_rebuild_script freeglut.sln rebuild.bat
        $COMSPEC //C rebuild.bat
    fi
    cd ..
    if [ $rebuild != 0 ]; then
        copy_libraries "freeglut/freeglut.lib" "freeglut/freeglut.dll"
    fi
fi

# For zlib nothing has to be done. the Visual Studio project is part of libpng.

# libpng copy projects and compile with VS2015:
if [ -d libpng ]; then
    cd libpng
    rebuild=`test -d lib ; echo $?`
    if [ $rebuild != 0 ]; then
        cd projects/vstudio
        cp ../../../libpng_3264.sln .
        cp ../../../libpng_3264.vcxproj libpng
        cp ../../../pnglibconf_3264.vcxproj pnglibconf
        cp ../../../zlib_3264.vcxproj zlib
        sed -i -e "s/\(zlib\)-[0-9]\+\.[0-9]\+\.[0-9]\+/\1/" zlib.props
        create_rebuild_script libpng_3264.sln rebuild.bat
        echo === build libpng...
        $COMSPEC //C rebuild.bat
        cd ../..
    fi
    cd ..
    if [ $rebuild != 0 ]; then
        copy_libraries "libpng/lib/libpng16.lib" "libpng/lib/libpng16.dll"
    fi
fi

# libiconv copy source/projects from github and compile with VS2015:
projdir=libiconv
if [ -d $projdir ]; then
    cd $projdir/MSVC14
    rebuild=`test -f rebuild.bat ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build libiconv...
        create_rebuild_script libiconv.sln rebuild.bat
        $COMSPEC //C rebuild.bat
    fi
    cd ../..
fi

# libxml2 copy projects and compile with VS2015:
# It implicitly uses the result libraries from libiconv.
if [ -d libxml2 ]; then
    cd libxml2
    rebuild=`test -d lib ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build libxml2...
        sed -i -e "s/\(#define\s\+LIBXML_LZMA_ENABLED\)/\/\* \1 \*\//" include/libxml/xmlversion.h 
        sed -i -e "s/\(#elif\s\+defined(_WIN32_WCE)\)/\1 || defined(_WIN32)/" libxml.h
        sed -i -e "s/\(Windows\s\+CE\)/Windows and \1/" libxml.h
        cd win32/VC10
        cp ../../../libxml2.sln .
        cp ../../../libxml2.vcxproj .
        cp ../../../runsuite.vcxproj .
        create_rebuild_script libxml2.sln rebuild.bat
        $COMSPEC //C rebuild.bat   
        cd ../..
    fi
    cd ..
    if [ $rebuild != 0 ]; then
        copy_libraries "libxml2/lib/libxml2.lib" "libxml2/lib/libxml2.dll"
    fi
fi

# libSDL2 modify projects and compile with VS2015:
if [ -d SDL2 ]; then
    cd SDL2
    rebuild=`test -d VisualC/SDL/Win32 ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build SDL2...
        cd VisualC
        sed -i -e "s/\"4.0\"/\"14.0\"/" */*.vcxproj
        sed -i -e "s/\"4.0\"/\"14.0\"/" */*/*.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" */*.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" */*/*.vcxproj
        sed -i -e "s/copy\s\+\(%(FullPath)\)\s\+\([$](ProjectDir)\)/copy \"\1\" \"\2\"/" */*/*.vcxproj  
        create_rebuild_script SDL.sln rebuild.bat
        $COMSPEC //C rebuild.bat
        cd ..
    fi
    cd ..
    if [ $rebuild != 0 ]; then
        copy_libraries "SDL2/VisualC/SDL2.lib" "SDL2/VisualC/SDL2.dll"
    fi
fi

# libSDL2_mixer modify projects and compile with VS2015:
if [ -d SDL2_mixer ]; then
    cd SDL2_mixer
    rebuild=`test -d VisualC/Win32 ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build SDL2_mixer...
        cd VisualC
        sed -i -e "s/\"4.0\"/\"14.0\"/" *.vcxproj
        sed -i -e "s/\"4.0\"/\"14.0\"/" */*.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" *.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" */*.vcxproj
        sed -i -e "s/copy\s\+\(%(FullPath)\)\s\+\([$](SolutionDir)[\\][$](Platform)[\\][$](Configuration)[\\]\)/copy \"\1\" \"\2\"/" SDL_mixer.vcxproj  
        sed -i -e "s/\(%(AdditionalIncludeDirectories)\)/..\\\\..\\\\SDL2\\\\include;\1/" *.vcxproj
        sed -i -e "s/\(%(AdditionalIncludeDirectories)\)/..\\\\..\\\\..\\\\SDL2\\\\include;\1/" */*.vcxproj
        sed -i -e "s/\(<\/SubSystem>\)/\1\n      <AdditionalLibraryDirectories>..\\\\..\\\\SDL2\\\\VisualC\\\\\$(Platform)\\\\\$(Configuration)<\/AdditionalLibraryDirectories>/" *.vcxproj
        sed -i -e "s/\(<\/SubSystem>\)/\1\n      <AdditionalLibraryDirectories>..\\\\..\\\\..\\\\SDL2\\\\VisualC\\\\\$(Platform)\\\\\$(Configuration)<\/AdditionalLibraryDirectories>/" */*.vcxproj
        create_rebuild_script SDL_mixer.sln rebuild.bat
        $COMSPEC //C rebuild.bat
        cd ..
    fi
    cd ..
    if [ $rebuild != 0 ]; then
        copy_libraries "SDL2_mixer/VisualC/SDL2_mixer.lib" "SDL2_mixer/VisualC/SDL2_mixer.dll"
    fi

    # libogg modify projects and compile with VS2015: 
    cd SDL2_mixer/external
    rebuild=`test -d libogg ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build libogg...
        olddir=`ls -1 | sed -ne "s/\(libogg-[0-9]\.[0-9]\.[0-9]\)[\/]*$/\1/p"`
        mv $olddir libogg # remove version number
        cd libogg/win32/VS2010
        sed -i -e "s/\"4.0\"/\"14.0\"/" *.vcxproj
        sed -i -e 's/\(\s\+<\/Link>.*\)/      <OutputFile>$(OutDir)libogg-0.dll<\/OutputFile>\n\1/' libogg_dynamic.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" *.vcxproj
        create_rebuild_script libogg_dynamic.sln rebuild.bat
        $COMSPEC //C rebuild.bat
        cd ../../..
    fi
    cd ../..
    if [ $rebuild != 0 ]; then
        libs=("SDL2_mixer/external/libogg/win32/VS2010/libogg.lib SDL2_mixer/external/libogg/win32/VS2010/libogg-0.dll")
        copy_libraries ${libs[@]}
    fi

    # libvorbis modify projects and compile with VS2015: 
    cd SDL2_mixer/external
    rebuild=`test -d libvorbis ; echo $?`
    if [ $rebuild != 0 ]; then
        echo === build libvorbis...
        olddir=`ls -1 | sed -ne "s/\(libvorbis-[0-9]\.[0-9]\.[0-9]\)[\/]*$/\1/p"`
        mv $olddir libvorbis # remove version number
        cd libvorbis/win32/VS2010
        sed -i -e "s/\"4.0\"/\"14.0\"/" */*.vcxproj
        sed -i -e "s/\([$](OutDir)libvorbis\)\.dll/\1-0.dll/" libvorbis/libvorbis_dynamic.vcxproj
        sed -i -e "s/\([$](OutDir)libvorbisfile\)\.dll/\1-3.dll/" libvorbisfile/libvorbisfile_dynamic.vcxproj
        sed -i -e "s/\(<\/ConfigurationType>\)/\1\n    <PlatformToolset>v140<\/PlatformToolset>/" */*.vcxproj
        create_rebuild_script vorbis_dynamic.sln rebuild.bat
        $COMSPEC //C rebuild.bat   
        cd ../../..
    fi
    cd ../..
    if [ $rebuild != 0 ]; then
        libs=("
            SDL2_mixer/external/libvorbis/win32/VS2010/libvorbis.lib
            SDL2_mixer/external/libvorbis/win32/VS2010/libvorbis-0.dll
            SDL2_mixer/external/libvorbis/win32/VS2010/libvorbisfile.lib
            SDL2_mixer/external/libvorbis/win32/VS2010/libvorbisfile-3.dll")
        copy_libraries ${libs[@]}
    fi
fi
