@echo off
@set Path=C:\WINNT\system32;C:\WINNT;C:\WINNT\System32\Wbem;c:\program files\gnu\wincvs;C:\usr\local\sce\ee\gcc\bin;$(SN_PATH)

@echo Build started: %time%

rem -=- Run Wade's preprocessor chain on the assembler code -=-
/NGL/tools/bin/m4 < hwosps2/krvu1.dsm > bootable/hwosps2/krvu1.pp
ee-gcc -x c++ -E bootable/hwosps2/krvu1.pp > bootable/hwosps2/krvu1.pp2
ee-dvp-as -g -I/usr/local/sce/ee/include -I/usr/local/sce/ee/gcc/ee/include -I/usr/local/sce/ee/gcc/include/g++-2 -I/sm/src/vsim -I/sm/src -o bootable/hwosps2/krvu1.o bootable/hwosps2/krvu1.pp2 > bootable/hwosps2/krvu1.lst

rem -=- make the iop modules -=-
rem cd hwosps2\gas_iop
rem make
rem copy *.irx ..\..\..
rem cd ..\..
if e%NUMBER_OF_PROCESSORS%==e2 goto dual_processor

:single_processor

@make --makefile=ks_makefile bootable BUILD=bootable

goto exit

:dual_processor

@make --makefile=ks_makefile -j2 bootable BUILD=bootable

goto exit

:exit

@echo Build finished: %time%

build_done.bat
