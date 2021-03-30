#!/bin/bash

createFiles()
{
	touch file1.txt file2.txt
	mkdir dir1
	cd ./dir1
	touch file3.txt file4.txt
	cd ..
	mkdir dir2
	cd ./dir2
	touch file5.txt
	mkdir dir3
	mkdir dir4
	cd ./dir3
	touch file6.txt
	cd ..
	cd ./dir4
	touch file7.txt file8.txt
	cd ..
	cd ..	
}

destroyFiles()
{
	rm FILE1.TXT tile2.txt
	rm -r dir1
	rm -r dir2
}

touch test1.txt

clear
echo 'All the following commands produce errors and do not change the name '
echo 'of any file, as they do not get to the actual execution stage'
echo 'After them commands producing results will be shown and the '
echo '"find" command will be used to show the changed file names'
read -p "Press enter to continue"

parameters=('-hrlut' '-hrlu' '-h someArgument' '-h' '-rlu' '-lu' '-l' '-u'
'-r' 'xxx' 'nonExistentFile.txt' '-l nonExistentFile.txt' '-r test1.txt' 
'-l -r test1.txt -x' '-l -r test1.txt someArgument' 's/valid/pattern/'
's/valid/pattern/ nonExistentFile.txt')

for par in "${parameters[@]}"; do
	clear
	echo "Command to execute"
	echo './modify.sh '"$par"
	read -p "Press enter to execute"
	echo -e "\n"
	./modify.sh $par
	echo -e "\n"
	read -p "Press enter to continue"
done

clear 
echo 'Now commands that get to the execution stage and produce results will be shown'
read -p "Press enter to continue" 

createFiles

parameters1=('-l file1.txt' '-u file1.txt' '-r -u dir2' 's/file/tile/ file2.txt' '45p tile2.txt'
'-l dir2')

for par1 in "${parameters1[@]}"; do
	clear
	echo "Command to execute"
	echo './modify.sh '"$par1"
	echo -e "find before the execution\n"
	find
	echo -e "\n"
	read -p "Press enter to execute"
	echo -e "\n"
	clear
	./modify.sh $par1
	echo -e "\n"
	echo -e "find after the execution\n"
	find
	echo -e "\n"
	read -p "Press enter to continue"
done

destroyFiles

