#!/bin/bash

echo "------------------------"
echo
echo "Building Engine"
echo
echo "------------------------"
echo
mkdir bin/build
cd bin/build
cmake -DCMAKE_BUILD_TYPE=Debug -DGENERATE_CONFIG=ON ../../FlareEngine/cpp/
echo
make -j6
echo
msbuild ../../FlareEditorMonitor/FlareEditorMonitor.csproj
cd ..
mv build/FlareNative FlareNative
mv ../FlareEditorMonitor/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorMonitor/bin/FlareEditorMonitor.dll FlareEditorMonitor.dll
echo "------------------------"
echo
echo "Building Editor"
echo
echo "------------------------"
echo
cd ..
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../FlareEditor
echo
make -j6
