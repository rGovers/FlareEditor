#!/bin/bash

echo "------------------------"
echo
echo "Building Engine"
echo
echo "------------------------"
echo
mkdir bin/build
cd bin/build
cmake -DCMAKE_BUILD_TYPE=Release -DGENERATE_CONFIG=ON -DENABLE_TRACE=ON -DENABLE_PROFILER=ON ../../FlareEngine/cpp/
make -j6
echo
echo "------------------------"
echo
echo "Building Editor CS"
echo
echo "------------------------"
echo
../../FlareEngine/deps/mono/build/bin/xbuild ../../FlareEditorMonitor/FlareEditorMonitor.csproj
cd ..
mv build/FlareNative FlareNative
mv ../FlareEditorBuildEngine/bin/*.dll .
mv ../FlareEditorCS/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorCS/bin/FlareEditorCS.dll FlareEditorCS.dll
cp -r ../FlareEngine/deps/mono/build/lib .
cp -r ../FlareEngine/deps/mono/build/etc .
echo "------------------------"
echo
echo "Building Editor"
echo
echo "------------------------"
echo
cd ..
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../FlareEditor
echo
make -j6
