@echo off

SETLOCAL
set ENV=core2
rem set PROJECT=firmware
rem set ADDRESS=0x400d2965
set ELFFILE=.pio/build/%ENV%/firmware.elf
set TOOLCHAIN=%HOME%\.platformio\packages\toolchain-xtensa-esp32\bin
rem echo on

echo.

@pushd %~dp0
rem echo cd %~dp0
%TOOLCHAIN%/xtensa-esp32-elf-addr2line -pfiaC -e %ELFFILE% %*
@popd

