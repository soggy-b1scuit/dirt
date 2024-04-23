call premake5 gmake2
call make config=debug_x64

if errorlevel 1 (
    echo Makefile failed. Exiting.
    exit /b 1
)
d:
cd D:\Program Files (x86)\Resource Hacker
ResourceHacker.exe -open "C:/github 2/dirt/dirt.exe" -save "C:/github 2/dirt/dirt.exe" -action addskip -res "C:/github 2/dirt/dirt.ico" -mask ICONGROUP,MAINICON, 
c:
cd C:/github 2/dirt/
call launch