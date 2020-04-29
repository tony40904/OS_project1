#!/bin/bash

for i in $(ls test_data)
do
	echo $i
	sudo ./main.out < test_data/$i > output/${i%%.*}_stdout.txt
	dmesg | grep Project1 > output/${i%%.*}_dmesg.txt
	sudo dmesg --clear
done
