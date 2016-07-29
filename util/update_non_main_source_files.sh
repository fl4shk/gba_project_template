#!/bin/bash

# This file is part of GBA Project Template.
# 
# Copyright 2015-2016 by Andy Clark (FL4SHK).
# 
# GBA Project Template is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
# 
# GBA Project Template is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with GBA Project Template.  If not, see <http://www.gnu.org/licenses/>.



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
