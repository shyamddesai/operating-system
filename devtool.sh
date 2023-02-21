#!/bin/bash
cd src
make clean
if [[ $1 == "clean" ]]; then    
    exit
fi
make mysh
cd ..
if [[ $1 == "run" ]]; then
    ./src/mysh
elif [[ $1 == "testA1" ]]; then
    ./src/mysh < testcases/assignment1/$2
else
    echo "bad command"
fi
