#!/bin/bash

#echo -n "Input the need to Build code file : "
#read Code_File
echo -n "Input the file Type : 1.c  2.cpp : "
read Code_Object

if [ $Code_Object = "1" ];then
	FileName="c"
elif [ $Code_Object = "2" ];then
	FileName="cpp"
else
	echo "No this option"
	exit 0
fi

echo -n "Input the need to Build code file : "
read Code_File
	
BuildMsg="Build =============> $Code_File.$FileName"

if [ -f $Code_File.$FileName ] && [ $FileName = "c" ];then
	if [ $Code_File.$FileName == "WorkEth.c" ];then
		echo $BuildMsg
		gcc -pthread -std=c99 -o $Code_File"_S" $Code_File.$FileName
	else
		echo $BuildMsg
		gcc -std=c99 -o $Code_File"_S" $Code_File.$FileName
	fi
elif [ -f $Code_File.$FileName ] && [ $FileName = "cpp" ];then
	echo $BuildMsg
	g++ -o $Code_File"_S" $Code_File.$FileName
else
	echo "No This file , Please check it again!"
	ls
fi


