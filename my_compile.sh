#!/bin/bash
#set the property of this file to execute mode. Or chmod command thing. Permissions thing
#1 means error and 0 means no error for the last command executed
#USAGE: ./my_compile.sh <cpp filename with extension>

setofcommands(){
	
    local filetype='.cpp'
    local file1='Arrays.cpp'
    local objfile1='Arrays.o'
    local file2='Dynarr.cpp'
    local objfile2='Dynarr.o'
    local ext='.o'
    local finalname=${1::-4}
    local objfile3=$finalname$ext
    
    #Arrays.o and Dynarr.o is already created by compiling and available in the directory
    #Compiled by default method that is no std=c++11 etc options
	#g++ -c -std=c++11 $1
	
    g++ -c $1    
   
	if [ $? -eq 0 ]
	then
    g++ $objfile1 $objfile2 $objfile3 -o $finalname
    #executing the program
    ./$finalname
    fi
}

setofcommands $1
