#!/bin/bash


if (( $# != 1 ))
then
	echo "syntax:  $0 other_project_path"
	echo "Exiting..."
	exit
fi

other_project_path="$1"

if [ ! -e $other_project_path ]
then
	echo "\"$other_project_path\" does not appear to exist!"
	echo "Exiting..."
	exit
elif [ ! -d $other_project_path ]
then
	echo "\"$other_project_path\" does not appear to be a directory!"
	echo "Exiting..."
	exit
fi


#find src -type f \( -iname 
all_src_files=( $(find src -type f \( -iregex '.*\..*pp' -o -iregex '.*\.s' \) -exec echo {} +) )
total_num_src_files=${#all_src_files[@]}

main_file_basename=main.thumb.cpp

# These are for non-main
src_basenames=()
src_dirnames=()

count=0

for file in ${all_src_files[@]}
do
	file_basename=$(basename $file)
	file_dirname=$(dirname $file)
	
	if [ "$file_basename" == "$main_file_basename" ]
	then
		echo "Ignoring $main_file_basename"
	else
		src_basenames[$count]=$file_basename
		src_dirnames[$count]=$file_dirname
		
		#count=$((count + 1))
		let count++
	fi
done

echo $count

for (( i=0; i<$count; ++i ))
do
	echo "basname:  ${src_basenames[$i]};  " \
		"dirname: ${src_dirnames[$i]}"
done
