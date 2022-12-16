mkdir build
cd build

cmake -G "Visual Studio 17" ..

cd ..

copy "FlareEngine\deps\mono\build\bin\mono-2.0-sgen.dll" "bin\mono-2.0-sgen.dll"
xcopy "FlareEngine\deps\mono\build\lib\*.*" "bin\lib\*" /y /s
xcopy "FlareEngine\deps\mono\build\etc\*.*" "bin\etc\*" /y /s