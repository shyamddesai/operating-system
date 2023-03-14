#!/bin/bash
clear
cd src
make clean
if [[ $1 == "clean" ]]; then
    cd ..    
    rm -rf env
    rm output.txt
    exit
fi
make mysh
cd ..
rm -rf env
mkdir env
cp src/mysh env/mysh
cp -a testcases/assignment1/. env
cp -a testcases/assignment2/. env
if [[ $1 == "run" ]]; then
    cd env
    ./mysh
    cd ..
elif [[ $1 == "test" ]]; then
    cd env
    ./mysh < $2
else
    echo "bad command"
fi
