@echo off
if "%1"=="" goto 1
rem Quick installation on MS-DOS systems using Turbo C++
copy tools\mkfile.%1 code\makefile
echo ---------------------------------------------------
echo Compilation will now start. This will take a while.
echo ---------------------------------------------------
chdir code
make clean
rem The REM'd lines below record start/finish times, and require an
rem   empty file called CR in the root directory
rem type ..\cr|time>time
make all
rem type ..\cr|time>>time
chdir ..
goto 2
:1
echo 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
echo �    Available makefiles under MSDOS                   �
echo �                                                      �
echo �  tc    Turbo C++ version 3.00 and later              �
echo �  gnu   Gnu C++ version 2.6.0 and earlier             �
echo �  gn2   Gnu C++ version 2.6.2 and later               �
echo �                                                      �
echo �  Type 'install OPTION' at the DOS prompt to install  �
echo �  the chosen OPTION; e.g. 'install tc'.               �
echo 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
:2

