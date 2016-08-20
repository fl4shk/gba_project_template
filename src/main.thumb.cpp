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



void sa_list_test() __attribute__((noinline));
void tasks_test() __attribute__((noinline));


int main()
{
	irq_init();
	irqEnable(irq_vblank);
	
	
	//arr_memfill8( (u8*)ewram_test_arr, '#', ewram_test_arr_size );
	memset( ewram_test_arr, '#', ewram_test_arr_size );
	
	ewram_test_arr[0] = '9';
	
	//memcpy( &ewram_test_arr[1], test_str, 5 );
	memset( &ewram_test_arr[1], '3', 9 );
	
	sa_list_test();
	//tasks_test();
	
	
	
	for (;;)
	{
		//debug_arr_group::clear_debug_vars();
		//clear_oam_mirror();
		
		// In Sherwin's Adventure, key polling is done in
		// game_manager::vblank_func().  
		// However, as of this comment being written, GBA Project Template
		// doesn't have a game_manager class.
		//key_poll();
		
		bios_wait_for_vblank();
	}
	
	return 0;
}



static constexpr u32 total_num_nodes = 200;
//typedef sa_list_stuff::sorted_always_list< u32, total_num_nodes >
//	the_sorted_always_list_type;
//
//the_sorted_always_list_type test_list __attribute__((_ewram));

sa_list_stuff::regular_list< u32, total_num_nodes > test_list 
	__attribute__((_iwram));
//sa_list_stuff::regular_list< temp_data_type, total_num_nodes > test_list 
//	__attribute__((_iwram));




// This is intended for debugging sa_list stuff.
class temp_debug_vars_group
{
public:		// variables and constants
	static constexpr size_t max_num_nodes_per_loop = debug_str::max_size,
		max_num_loops = 20;
	
	static constexpr size_t total_num_indices_per_arr_2d 
		= max_num_nodes_per_loop * max_num_loops;
	
	
	u32 outer_index = 0, inner_index = 0;
	
	debug_str str_arr[max_num_loops];
	
	
	static constexpr size_t num_indices_per_index_group = 3;
	// Technically, this is a 3D array.
	s16 index_group_arr_2d[max_num_loops][max_num_nodes_per_loop]
		[num_indices_per_index_group];
	
public:		// functions
	inline temp_debug_vars_group()
	{
		reset();
	}
	
	inline void reset()
	{
		outer_index = inner_index = 0;
		
		for ( u32 i=0; i<max_num_loops; ++i )
		{
			str_arr[i].total_clear();
		}
		
		memfill32( reinterpret_cast<void*>(index_group_arr_2d), -1,
			sizeof(index_group_arr_2d) / sizeof(u32) );
		
	}
	
	inline void write_str_and_inc_outer_index( const char* to_write )
	{
		str_arr[outer_index++] = to_write;
	}
	inline void write_three_indices_and_inc_inner_index
		( s32 the_index, s32 the_next_index, 
		s32 the_prev_index )
	{
		index_group_arr_2d[outer_index][inner_index][0] = the_index;
		index_group_arr_2d[outer_index][inner_index][1] = the_next_index;
		index_group_arr_2d[outer_index][inner_index][2] = the_prev_index;
		
		++inner_index;
	}
	
} __attribute__((_align4));

temp_debug_vars_group tdvg;


template< typename type, u32 the_total_num_nodes >
void show_small_regular_sa_list
	( sa_list_stuff::regular_list< type, the_total_num_nodes >& to_show )
{
	asm_comment("show_small_regular_sa_list_as_str()");
	
	tdvg.inner_index = 0;
	
	
	static constexpr u32 to_write_max_size = debug_str::max_size;
	char to_write[to_write_max_size];
	
	memset( to_write, 0, to_write_max_size );
	
	u32 real_size = 0;
	
	for ( int i=to_show.get_front_index();
		i!=-1;
		i=to_show.get_node_array()[i].next_index() )
	{
		to_write[real_size++] = (char)(to_show.get_node_array()[i]
			.data);
		
		tdvg.write_three_indices_and_inc_inner_index( i, 
			to_show.get_node_array()[i].next_index(),
			to_show.get_node_array()[i].prev_index() );
		
		if ( real_size >= to_write_max_size )
		{
			break;
		}
	}
	
	//debug_arr_group::write_str_and_inc(static_cast<const char*>(to_write));
	
	tdvg.write_str_and_inc_outer_index(static_cast<const char*>(to_write));
	
}
inline void show_test_list()
{
	show_small_regular_sa_list(test_list);
}



void reinit_test_list_and_profile_deallocate()
{
	profile_start();
	test_list.fully_deallocate_via_unlink();
	show_profile_stop();
	
	//if ( test_list.get_front_index() == 105 )
	//{
	//	debug_arr_group::gdb_breakpoint_helper = 0;
	//}
	
	s32& test_list_front_index = test_list.get_front_index();
	
	s32 test_list_end = test_list.push_back('g');
	
	for ( u32 i=0; i<5; ++i )
	{
		test_list.push_front('1' + i );
	}
	
	//s32 second_index = test_list.insert_before( test_list_front_index, 
	//	'h' );
	//test_list.insert_before( test_list_end, 'f' );
	
	s32 second_index = test_list.insert_after( test_list_front_index, 
		'h' );
	//s32 third_index = test_list.get_node_at(second_index).next_index();
	
	test_list_end = test_list.insert_after( test_list_end, 'f' );
	
	
	
	
	for ( u32 i=0; i<150; ++i )
	{
		//test_list.push_front('a' + ( i % 25 ) );
		test_list.push_front('a' + ( i % 4 ) );
	}
}

//static constexpr size_t test_cbuf_size = 4;
//s32 test_cbuf[test_cbuf_size];
//sa_list_stuff::circ_buf_helper test_cbuf_helper( test_cbuf, 
//	test_cbuf_size );


void sa_list_test()
{
	// Part 1
	asm_comment("Part 1");
	reinit_test_list_and_profile_deallocate();
	show_test_list();
	
	
	// Part 2
	asm_comment("Part 2");
	profile_start();
	test_list.merge_sort_via_array();
	show_profile_stop();
	show_test_list();
	
	
	// Part 3
	asm_comment("Part 3");
	reinit_test_list_and_profile_deallocate();
	show_test_list();
	
	// Part 4
	asm_comment("Part 4");
	profile_start();
	test_list.insertion_sort();
	//test_list.merge_sort_via_array();
	show_profile_stop();
	show_test_list();
	
	
	
	// Part 5
	asm_comment("Part 5");
	reinit_test_list_and_profile_deallocate();
	show_test_list();
	
	// Part 6
	asm_comment("Part 6");
	profile_start();
	test_list.insertion_sort();
	//test_list.merge_sort_via_array();
	show_profile_stop();
	show_test_list();
	
	
	
	
	
	//profile_start();
	//
	//for ( u32 i=0; i<total_num_nodes; ++i )
	//{
	//	test_list.insert_and_sort(i);
	//}
	//
	//test_profile_result_0 = profile_stop();
	//
	//
	//
	//
	//test_list.fully_deallocate();
	//
	//profile_start();
	//for ( u32 i=0; i<total_num_nodes; ++i )
	//{
	//	test_list.push_front(i);
	//}
	//test_list.insertion_sort();
	//
	//test_profile_result_1 = profile_stop();
	
	
}






void test_func_0()
{
	show_debug_str_group("test_func_0");
}

void test_func_1()
{
	show_debug_str_group("test_func_1");
}

class test_class
{
public:		// variables
	
	
public:		// functions
	static void test_func_2()
	{
		show_debug_str_group("test_func_2");
	}
	
	static void test_func_3()
	{
		show_debug_str_group("test_func_3");
	}
	
} __attribute__((_align4));


void tasks_test()
{
	static constexpr size_t max_num_tasks = 20;
	static constexpr size_t tasks_arr_size = max_num_tasks;
	task tasks_arr[tasks_arr_size];
	
	tasks_arr[0].set_func(&test_func_0);
	tasks_arr[1].set_func(&test_func_1);
	tasks_arr[2].set_func(&test_class::test_func_2);
	tasks_arr[3].set_func(&test_class::test_func_3);
	
	for ( s32 i=tasks_arr_size-1; i>=0; --i )
	{
		task& curr_task = tasks_arr[i];
		
		if ( curr_task.has_func() )
		{
			curr_task();
		}
	}
}
