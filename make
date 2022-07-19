#!/bin/bash

files=( "main.c" )
args=( "-O2" "-Wfatal-errors" )

mkdir bin 2> /dev/null
gcc ${args[@]} ${files[@]} -o bin/machinespeak || read