// This file is part of GBA Project Template.
// 
// Copyright 2015-2017 by Andrew Clark (FL4SHK).
// 
// GBA Project Template is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
// 
// GBA Project Template is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with GBA Project Template.  If not, see <http://www.gnu.org/licenses/>.


#include "lut_division_funcs.hpp"

extern const u32 udiv_table[];
extern const s32 sdiv_table[];


u64 unsafe_lut_udiv( u32 numerator, u16 divisor )
{
	u64 temp = static_cast<u64>(numerator);
	const u32 one_slash_divisor = udiv_table[divisor];
	u64 temp_2 = static_cast<u64>(one_slash_divisor);
	
	return ( temp * temp_2 );
}

s64 unsafe_lut_sdiv( s32 numerator, s16 divisor )
{
	s64 temp = static_cast<s64>(numerator);
	const s32 one_slash_divisor = sdiv_table[divisor];
	s64 temp_2 = static_cast<s64>(one_slash_divisor);
	
	return ( temp * temp_2 );
}


u64 safe_lut_udiv( u32 numerator, u16 divisor, int& valid )
{
	if ( divisor == 0 )
	{
		valid = false;
		return 0;
	}
	
	valid = true;
	u64 temp = static_cast<u64>(numerator);
	//u64 temp_divisor = static_cast<u64>(divisor);
	u32 one_slash_divisor = udiv_table[divisor];
	u64 temp_2 = static_cast<u64>(one_slash_divisor);
	
	return ( temp * temp_2 );
}

s64 safe_lut_sdiv( s32 numerator, s16 divisor, int& valid )
{
	if ( divisor == 0 )
	{
		valid = false;
		return 0;
	}
	
	valid = true;
	s64 temp = static_cast<s64>(numerator);
	//s64 temp_divisor = static_cast<s64>(divisor);
	s32 one_slash_divisor = sdiv_table[divisor];
	s64 temp_2 = static_cast<s64>(one_slash_divisor);
	
	return ( temp * temp_2 );
}

