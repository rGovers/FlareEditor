#!/bin/bash

echo "------------------------"
echo
echo "Building Engine"
echo
echo "------------------------"
echo
mkdir bin/build
cd bin/build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-O3 -DGENERATE_CONFIG=ON -DENABLE_TRACE=ON -DENABLE_PROFILER=ON ../../FlareEngine/cpp/
make -j6
echo
echo "------------------------"
echo
echo "Building Editor CS"
echo
echo "------------------------"
echo
../../FlareEngine/deps/flare-mono/build/bin/xbuild ../../FlareEditorCS/FlareEditorCS.csproj /p:Configuration=Release
cd ..
mv build/FlareNative FlareNative
mv ../FlareEditorCS/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorCS/bin/FlareEditorCS.dll FlareEditorCS.dll
cp -r ../FlareEngine/deps/flare-mono/build/lib .
cp -r ../FlareEngine/deps/flare-mono/build/etc .
echo "------------------------"
echo
echo "Building Editor"
echo
echo "------------------------"
echo
cd ..
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-O3 ../FlareEditor
echo
make -j6
