#!/bin/bash

# This file is part of GBA Project Template.
# 
# Copyright 2015-2017 by Andrew Clark (FL4SHK).
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


# Man I love the find program.

find . \( -path ./.git -o -path ./small_code_testing_things \
	-o -path ./deps -o -path ./objs -o -path ./asmouts \
	-o -path ./lookup_tables \
	\
	-o -iname "*.swp" -o -iname "*.swo" -o -iname "*.gdb_history*" \
	-o -path ./util/convert_project_name_in_license.sh \
	-o -iname "*tags" -o -iname "*types*.taghl" \
	-o -iname "*.gba" -o -iname "*.elf" \
	-o -iname "*README*" -o -iname "*COPYING*" \
	-o -iname "*linker_map.txt*" \
	\) -prune \
	\
	-o -type f \
	-exec sed -i 's/Sherwin'\''s Adventure/GBA Project Template/g' {} +
	#-print


