#!/bin/sh

SOURCE_FILES="
Configuration.plist \
../avatar/resources/CyWee.avatar \
../tracker/resources/face.mytracker \
../tracker/resources/face.mytrackerparams
../tracker/resources/face.con
"

mkdir $1/Contents/Resources || true

for i in $SOURCE_FILES ; do
    echo cp $i $1/Contents/Resources/
    cp $i $1/Contents/Resources/
done