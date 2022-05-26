set my_pwd=%~dp0
echo %my_pwd%

echo -----------------set env--------------------
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cd %my_pwd%

echo -----------------config main exe--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
-S%my_pwd% ^
--preset Ninja_release



echo -----------------config maya 2018--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
-S%my_pwd% ^
--preset Ninja_release_plug -DMaya_Version=2018

echo -----------------build maya 2018--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
--build ^
--preset release_maya_plug

rmdir %my_pwd%build\doodle_maya\src\doodle_exe
rmdir %my_pwd%build\doodle_maya\plug
del  %my_pwd%build\doodle_maya\CMakeCache.txt


echo -----------------config maya 2019--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
-S%my_pwd% ^
--preset Ninja_release_plug -DMaya_Version=2019

echo -----------------build maya 2019--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
--build ^
--preset release_maya_plug

rmdir %my_pwd%build\doodle_maya\src\doodle_exe
rmdir %my_pwd%build\doodle_maya\plug
del  %my_pwd%build\doodle_maya\CMakeCache.txt

echo -----------------config maya 2020--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
-S%my_pwd% ^
--preset Ninja_release_plug -DMaya_Version=2020

echo -----------------build maya 2020--------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
--build ^
--preset release_maya_plug

rmdir %my_pwd%build\doodle_maya\src\doodle_exe
rmdir %my_pwd%build\doodle_maya\plug
del  %my_pwd%build\doodle_maya\CMakeCache.txt



echo -----------------pack---------------------
"C:\Program Files\CMake\bin\cmake.exe" ^
--build ^
--preset gen_light_file

echo -----------------copy file--------------------
robocopy %my_pwd%build\Ninja_release\html %my_pwd%build\html /s /NFL /NDL
mkdir %my_pwd%build\html\file
robocopy %my_pwd%build\install %my_pwd%build\html\file *.msi
robocopy %my_pwd%build\install %my_pwd%build\html\file *.7z
py %my_pwd%build\generate_directory_index_caddystyle.py %my_pwd%\build\html\file

rmdir %my_pwd%build\install\bin
rmdir %my_pwd%build\install\maya
rmdir %my_pwd%build\install\ue425_Plug
rmdir %my_pwd%build\install\ue426_Plug
rmdir %my_pwd%build\install\ue427_Plug

Exit 0
