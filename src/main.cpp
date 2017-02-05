// This file is part of GBA Project Template.
// 
// Copyright 2015-2017 by Andy Clark (FL4SHK).
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


#include "gba_specific_stuff/asm_funcs.hpp"

#include "gba_specific_stuff/button_stuff.hpp"
#include "gba_specific_stuff/gfx_reg_stuff.hpp"
#include "gba_specific_stuff/oam_entry_classes.hpp"
#include "gba_specific_stuff/interrupt_stuff.hpp"
#include "gba_specific_stuff/bios_function_wrappers.hpp"
#include "gba_specific_stuff/dma_stuff.hpp"
//#include "gba_specific_stuff/sound_reg_stuff.hpp"
#include "gba_specific_stuff/timer_stuff.hpp"
#include "gba_specific_stuff/profiling_stuff.hpp"

#include "general_utility_stuff/debug_vars.hpp"
#include "general_utility_stuff/misc_utility_funcs.hpp"
#include "general_utility_stuff/range_funcs.hpp"
#include "general_utility_stuff/task_stuff.hpp"


#include "general_utility_stuff/extended_sa_list_class_stuff.hpp"


// This is an assembly function.  It doesn't do very much.
extern "C" int ewram_test_func();


static const u32 ewram_test_arr_size = 10;
u8 ewram_test_arr[ewram_test_arr_size] __attribute__((_ewram));

//static const char* test_str = "Birds2";
static const char test_str[] = "Birds2";
//static const u32 test_str_size = sizeof(test_str) / sizeof(char);
static const u32 test_str_size = ( sizeof(test_str) / sizeof(char) ) - 1;



//static constexpr u32 total_num_nodes = 200;
static constexpr u32 total_num_nodes = 1000;


static constexpr size_t temp_sram_buf_size = ( total_num_nodes + 2 ) 
	* sizeof(s32);
u8 temp_sram_buf[temp_sram_buf_size] __attribute__((_sram));

const size_t get_temp_sram_buf_size()
{
	return temp_sram_buf_size;
}







void fixed_point_division_test() __attribute__((noinline));


int main()
{
	irq_init();
	irqEnable(irq_vblank);
	
	
	//arr_memfill8( (u8*)ewram_test_arr, '#', ewram_test_arr_size );
	memset( ewram_test_arr, '#', ewram_test_arr_size );
	
	ewram_test_arr[0] = '9';
	
	//memcpy( &ewram_test_arr[1], test_str, 5 );
	memset( &ewram_test_arr[1], '3', 9 );
	
	//sa_list_test();
	
	for (;;)
	{
		//debug_arr_group::clear_debug_vars();
		clear_oam_mirror();
		
		// In Sherwin's Adventure, key polling is done in
		// game_manager::vblank_func().  
		// However, as of this comment being written, GBA Project Template
		// doesn't have a game_manager class.
		//key_poll();
		
		bios_wait_for_vblank();
	}
	
	return 0;
}


void fixed_point_division_test()
{
	fixed24p8 temp0;
	fixed8p8 temp1;
	fixed24p8 temp2;
	
	temp0 = make_f24p8( 20, 0xa9 );
	temp1 = make_f8p8( 10, 0x82 );;
	temp2 = temp0 / temp1;
	show_debug_f24p8_group( temp0, (fixed24p8)temp1, temp2 );
	
	temp0 = make_f24p8( -20, 0xa9 );
	//temp1 = make_f8p8( 10, 0x82 );
	temp2 = temp0 / temp1;
	show_debug_f24p8_group( temp0, (fixed24p8)temp1, temp2 );
}

