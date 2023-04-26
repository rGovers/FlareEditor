#!/bin/bash

echo "------------------------"
echo
echo "Building Engine"
echo
echo "------------------------"
echo
mkdir bin/crossbuild
cd bin/crossbuild
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-O3 -DGENERATE_CONFIG=ON -DENABLE_TRACE=ON -DENABLE_PROFILER=ON -DCMAKE_TOOLCHAIN_FILE=../../FlareEngine/toolchains/WinCross.cmake ../../FlareEngine/FlareNative/
make -j6
echo
echo "------------------------"
echo
echo "Building Editor CS"
echo
echo "------------------------"
echo
../../FlareEngine/deps/flare-mono/build/bin/xbuild ../../FlareEditorBuildEngine/FlareEditorBuildEngine.csproj /p:Configuration=Release
../../FlareEngine/deps/flare-mono/build/bin/xbuild ../../FlareEditorCS/FlareEditorCS.csproj /p:Configuration=Release
cd ..
mv crossbuild/FlareNative.exe FlareNative.exe
mv ../FlareEditorBuildEngine/bin/*.dll .
mv ../FlareEditorCS/bin/FlareCS.exe FlareCS.dll
mv ../FlareEditorCS/bin/FlareEditorCS.dll FlareEditorCS.dll
# cp -rf ../FlareEngine/deps/flare-mono/build/lib .
cp -rf ../FlareEngine/deps/flare-mono/crossbuild/lib .
cp -rf ../FlareEngine/deps/flare-mono/crossbuild/etc .
# cp ../FlareEngine/deps/flare-mono/crossbuild/bin/libmonosgen-2.0.dll libmonosgen-2.0.dll
# cp ../FlareEngine/deps/flare-mono/crossbuild/bin/libMonoPosixHelper.dll libMonoPosixHelper.dll
echo "------------------------"
echo
echo "Building Editor"
echo
echo "------------------------"
echo
cd ..
mkdir crossbuild
cd crossbuild
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-O3 -DCMAKE_TOOLCHAIN_FILE=../FlareEngine/toolchains/WinCross.cmake ../FlareEditor
echo
make -j6

cp ../FlareEngine/deps/flare-mono/crossbuild/bin/libmonosgen-2.0.dll libmonosgen-2.0.dll