#!/bin/sh

BINARY=qt-gui.app/Contents/MacOS/qt-gui
FRAMEWORKS=qt-gui.app/Contents/Frameworks/

# Find path to OpenCV
OPENCV_LIBRARIES=( $(otool -L ${BINARY} | grep libopencv | sed -E 's/^[[:space:]]+(.+\.dylib).*$/\1/') )
OPENCV_PATH=$(dirname $OPENCV_LIBRARIES[0])

if [ ! -d $FRAMEWORKS ] ; then
    mkdir $FRAMEWORKS
fi

for i in ${OPENCV_LIBRARIES[*]} ; do
    echo cp $i $FRAMEWORKS
    cp $i $FRAMEWORKS
done

NEW_OPENCV_LIBRARIES=$(for lib in ${OPENCV_LIBRARIES[*]} ; do echo ${FRAMEWORKS}/$(basename $lib) ; done )

EDIT_BINARIES="${BINARY} ${NEW_OPENCV_LIBRARIES}"

for lib in ${OPENCV_LIBRARIES[*]} ; do    
    to="@loader_path/../Frameworks/$(basename $lib)"
    echo install_name_tool -id "$to" $FRAMEWORKS/$(basename $lib)
    install_name_tool -id "$to" $FRAMEWORKS/$(basename $lib)
    for i in $EDIT_BINARIES ; do
	echo install_name_tool -change $lib "$to" $i
	install_name_tool -change $lib "$to" $i
    done 
done