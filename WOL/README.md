# wol.exe


## Description
Simple cross-platform (compilation tested on Windows-10 64bit // Visual Studio 2017 & 2019 AND Ubuntu 19.10 // gcc 9.2.1) "wake on lan" command line tool.  
Compilation on Ubuntu with gcc: Put both files (wol.h and wol.c) in one folder and run " gcc wol.c -o wol_lnx -L. --static ". Run the program with " ./wol_lnx ".  
Compilation on Windows with Visual Studio: Put both files in one folder. Then start " Developer Command Prompt for VS 2019 " and run ' cl wol.c /TC /Fe"./wol.exe" /MT '. Run the program from command prompt with " wol.exe ".  


## Usage
Syntax:
 >     wol.exe mac-address
 >     wol.exe mac-address /tar=valid-target
 valid target can be any valid ip-address or hostname; if switch is omitted, ethernet broadcast is assumed  
Examples:
 >     wol.exe 0019ef3ecc0b
 >     wol.exe 3e77aa00317d /tar=192.168.10.255