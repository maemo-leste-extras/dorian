@echo off
rem Add directories to stop warnings during build
c:
cd \S60\devices\
if not exist Nokia_N97_SDK_v1.0 goto sdkmissing

IF NOT EXIST .\epoc32\include\osextensions mkDIR .\epoc32\include\osextensions
IF NOT EXIST .\epoc32\include\osextensions\stdapis mkDIR .\epoc32\include\osextensions\stdapis
IF NOT EXIST .\epoc32\include\osextensions\stdapis\sys mkDIR .\epoc32\include\osextensions\stdapis\sys
IF NOT EXIST .\epoc32\include\osextensions\stdapis\stlport mkDIR .\epoc32\include\osextensions\stdapis\stlport
IF NOT EXIST .\epoc32\include\oem mkDIR .\epoc32\include\oem
IF NOT EXIST .\epoc32\include\domain\osextensions mkDIR .\epoc32\include\domain\osextensions
IF NOT EXIST .\epoc32\include\domain\osextensions\loc mkDIR .\epoc32\include\domain\osextensions\loc
IF NOT EXIST .\epoc32\include\domain\osextensions\loc\sc mkDIR .\epoc32\include\domain\osextensions\loc\sc
IF NOT EXIST .\epoc32\include\domain\middleware\loc\sc mkDIR .\epoc32\include\domain\middleware\loc\sc

IF NOT EXIST .\epoc32\include\osextensions echo the directory not created .\epoc32\include\osextensions
IF NOT EXIST .\epoc32\include\osextensions\stdapis echo the directory not created .\epoc32\include\osextensions\stdapis
IF NOT EXIST .\epoc32\include\osextensions\stdapis\sys echo the directory not created .\epoc32\include\osextensions\stdapis\sys
IF NOT EXIST .\epoc32\include\osextensions\stdapis\stlport echo the directory not created .\epoc32\include\osextensions\stdapis\stlport
IF NOT EXIST .\epoc32\include\oem echo the directory not created .\epoc32\include\oem
IF NOT EXIST .\epoc32\include\domain\osextensions echo the directory not created .\epoc32\include\domain\osextensions
IF NOT EXIST .\epoc32\include\domain\osextensions\loc echo the directory not created .\epoc32\include\domain\osextensions\loc
IF NOT EXIST .\epoc32\include\domain\osextensions\loc\sc echo the directory not created .\epoc32\include\domain\osextensions\loc\sc
IF NOT EXIST .\epoc32\include\domain\middleware\loc\sc echo the directory not created .\epoc32\include\domain\middleware\loc\sc

goto end

:sdkmissing
    echo No Nokia_N97_SDK_v1.0 in C:\S60\devices
    goto end
    
:end

