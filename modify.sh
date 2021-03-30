#!/bin/bash

helpOpt=0
recOpt=0
lowerOpt=0
upperOpt=0

# parsing option arguments
while getopts ":hrlu" opt; do
	case "$opt" in
		h)
			helpOpt=1
			;;
		r)
			recOpt=1
			;;
		l)
			lowerOpt=1
			;;
		u)
			upperOpt=1
			;;
		\?)
			echo "$0: invalid option -- '$OPTARG'"
			echo "Try '$0 -h' for more information."
			exit 1
			;;
	esac
done
shift $((OPTIND-1))

# Enforce a strict './modify.sh -h' command format if -h is present.
# No other options or arguments allowed
if [[ $helpOpt -eq 1 ]]; then
	if [[ $lowerOpt -eq 1 || $upperOpt -eq 1 || $recOpt -eq 1 ]]; then
		echo "Error: cannot use -h and other options at the same time."
		echo "Try '$0 -h' for more information."
		exit 1
	elif [[ "$#" -ne 0 ]]; then
		echo "Error: cannot use -h with arguments."
		echo "Try '$0 -h' for more information."
		exit 1
	else
		echo "$0: $0 [-r] [-l|-u] <dir/file names...>"
		echo -e "\t     $0 [-r] <sed pattern> <dir/file names...>"
		echo -e "\t     $0 [-h]"
		echo "Modifies file names."
		echo -e "\nOptions:"
		echo -e "\t-r\t names are changed recursively i.e. all subfolders"
		echo -e "\t  \t will also have their files' names changed"
		echo -e "\t-l\t changes file names to lowercase"
		echo -e "\t-u\t changes file names to uppercase"
		exit 0
	fi
fi

# check for -lu, -ul, -l -u and -u -l
if [[ $lowerOpt -eq 1 ]] && [[ $upperOpt -eq 1 ]]; then
	echo "Error: cannot use -l and -u at the same time."
	echo "Try '$0 -h' for more information."
	exit 1
fi

# neither -l or -u was chosen, get the sed pattern from the next argument
# and check if it's a valid one. If it is, set it to be the one that is used
if [[ $lowerOpt -eq 0 ]] && [[ $upperOpt -eq 0 ]]; then	
	echo a | sed "$1" > /dev/null 2>/dev/null
	if [[ $? != 0 ]]; then 
		echo "Error: sed pattern not valid."
		echo "Try '$0 -h' for more information."
		exit 1
	fi
	sedPattern="$1"
	shift
fi

# check if there are arguments present
if [[ "$#" -eq 0 ]]; then
	echo "Error: no arguments provided."
	echo "Try '$0 -h' for more information."
	exit 1
fi

# check is all arguments are files or directories
for arg in "$@"; do
	if [[ ! -f "$arg" ]] && [[ ! -d "$arg" ]]; then
		echo "Error: $arg is not a valid argument."
		echo "Try '$0 -h' for more information."
		exit 1
	fi
done

# set change pattern to lowercase all
if [[ $lowerOpt -eq 1 ]]; then
	sedPattern='s/\(.*\)/\L\1/'
fi
# or uppercase all
if [[ $upperOpt -eq 1 ]]; then
	sedPattern='s/\(.*\)/\U\1/'
fi

# iterate over all arguments
for arg in "$@"; do

	# get all files and directories that from the argument
	if [[ $recOpt -eq 1 ]]; then
		filePaths=( $(find "$arg") )
	else	
		filePaths=( $(find "$arg" -maxdepth 1) )
	fi
	
	# iterate over all files and directories in filePaths
	for x in "${filePaths[@]}"; do
		
		# if we are currently working on a directory (not a file), leave it be and continue
		if [[ -d "$x" ]]; then
			continue
		fi
	
		# get directory, full file name, file extension and name without extension
		dirName="$(dirname ${x})"
		fileBaseName=$(basename -- "$x")
		fileExt="${fileBaseName##*.}"
		fileJustName="${fileBaseName%.*}"

		fileNameNew="$(echo $fileJustName | sed -e $sedPattern)"

		mv "$x" "$dirName/$fileNameNew.$fileExt"
	
	done
	
done
