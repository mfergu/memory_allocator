#!/bin/bash
make clean
clear
sleep .5
clear
make &> compile_out.txt
echo -e "clang -Weverything -g -o some_test_file some_test_file.c libmyalloc.so"
clang -Wall -g -o test test4.c myalloc.c &>> compile_out.txt
echo -e " LD_PRELOAD=./libmyalloc.so ./test"
LD_PRELOAD=./libmyalloc.so ./test
grep --color=always -rn "error" compile_out.txt
