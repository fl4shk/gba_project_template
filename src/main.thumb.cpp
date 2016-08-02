// This file is part of GBA Project Template.
// 
// Copyright 2015-2016 by Andy Clark (FL4SHK).
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

#include "game_engine_stuff/debug_vars.hpp"


// This is an assembly function.  It doesn't do very much.
extern "C" int ewram_test_func();


static const u32 ewram_test_arr_size = 10;
u8 ewram_test_arr[ewram_test_arr_size] __attribute__((_ewram));

//static const char* test_str = "Birds2";
static const char test_str[] = "Birds2";
//static const u32 test_str_size = sizeof(test_str) / sizeof(char);
static const u32 test_str_size = ( sizeof(test_str) / sizeof(char) ) - 1;


int main()
{
	//arr_memfill8( (u8*)ewram_test_arr, '#', ewram_test_arr_size );
	memset( ewram_test_arr, '#', ewram_test_arr_size );
	
	ewram_test_arr[0] = '9';
	
	//memcpy( &ewram_test_arr[1], test_str, 5 );
	memset( &ewram_test_arr[1], '3', 9 );
	
	
	irq_init();
	irqEnable(irq_vblank);
	
	
	for (;;)
	{
		debug_arr_group::clear_debug_vars();
		clear_oam_mirror();
		
		// Key polling is done in game_manager::vblank_func()
		//key_poll();
		
		bios_wait_for_vblank();
	}
	
	
	return 0;
}


