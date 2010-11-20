@echo off

set src=c:\users\workspace\dorian
set build=C:\S60\devices\S60_5th_Edition_SDK_v1.0\epoc32\BUILD\users\workspace

if not exist %src%\dorian.pro goto badsrc

if exist %src%\.make.cache del /q %src%\.make.cache
if exist %src%\abld.bat del /q %src%\abld.bat
if exist %src%\bld.inf del /q %src%\bld.inf
if exist %src%\dorian.loc del /q %src%\dorian.loc
if exist %src%\dorian.rss del /q %src%\dorian.rss
if exist %src%\dorian.sis del /q %src%\dorian.sis
if exist %src%\*.nmp del /q %src%\*.mmp
if exist %src%\dorian_installer.pkg del /q %src%\dorian_installer.pkg
if exist %src%\dorian_reg.rss del /q %src%\dorian_reg.rss
if exist %src%\dorian_template.pkg del /q %src%\dorian_template.pkg
if exist %src%\Makefile del /q %src%\Makefile
if exist %src%\Makefile_* del /q %src%\Makefile_*
if exist %src%\moc rmdir /s /q %src%\moc
if exist %src%\obj rmdir /s /q %src%\obj
if exist %src%\rcc rmdir /s /q %src%\rcc
if exist %src%\tmp rmdir /s /q %src%\tmp
if exist %src%\ui rmdir /s /q %src%\ui

if not exist %build% goto badbuild
if exist %build%\dorian rmdir /s /q %build%\dorian

goto exit

:badsrc
echo Source directory %src% does not exist
goto exit

:badbuild
echo Build directory %build% does not exist
goto exit

:exit
