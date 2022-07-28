#!/bin/bash

files=( "main.c" "src/mchnSpk.c" )
args=( "-O2" "-Wfatal-errors" "-Wno-unused-result" )

mkdir bin 2> /dev/null
gcc ${args[@]} ${files[@]} -o bin/machinespeak || read