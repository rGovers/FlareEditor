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
make -j6
echo
echo "------------------------"
echo
echo "Building Editor CS"
echo
echo "------------------------"
echo
../../FlareEngine/deps/flare-mono/build/bin/xbuild ../../FlareEditorBuildEngine/FlareEditorBuildEngine.csproj /p:Configuration=Debug
../../FlareEngine/deps/flare-mono/build/bin/xbuild ../../FlareEditorCS/FlareEditorCS.csproj /p:Configuration=Debug
cd ..
mv build/FlareNative FlareNative
mv ../FlareEditorBuildEngine/bin/*.dll .
mv ../FlareEditorCS/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorCS/bin/FlareEditorCS.dll FlareEditorCS.dll
cp -r ../FlareEngine/deps/flare-mono/build/lib .
cp -r ../FlareEngine/deps/flare-mono/build/etc .
echo
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
