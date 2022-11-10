#!/bin/bash

echo "Building Editor"
echo "------------------------"
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../FlareEditor
make -j6
echo "Building Engine"
echo "------------------------"
cd ..
mkdir bin/build
cd bin/build
cmake -DCMAKE_BUILD_TYPE=Debug -DGENERATE_CONFIG=ON ../../FlareEngine/cpp/
make -j6
msbuild ../../FlareEditorMonitor/FlareEditorMonitor.csproj
cd ..
mv ../FlareEditorMonitor/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorMonitor/bin/FlareEditorMonitor.dll FlareEditorMonitor.dll