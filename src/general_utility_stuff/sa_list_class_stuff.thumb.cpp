// This file is part of GBA Project Template.
// 
// Copyright 2015-2016 by Andrew Clark (FL4SHK).
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


#include "sa_list_class_stuff.hpp"

#include "debug_vars.hpp"

namespace sa_list_stuff
{

void list_backend::init( void* n_node_array, 
	sa_free_list_backend* n_the_free_list_backend_ptr,
	u32 n_total_num_nodes, u32 n_specific_type_size,
	u32 n_whole_node_size, 
	
	generic_void_2arg_fp n_specific_type_copy_fp, 
	generic_void_2arg_fp n_specific_type_move_fp,
	generic_void_1arg_fp n_specific_type_reset_fp, 
	generic_u32_2arg_fp n_specific_type_less_fp, 
	
	generic_void_ptr_1arg_fp n_get_node_data_fp, 
	generic_vec2_s16_ptr_1arg_fp n_get_index_pair_fp,
	generic_void_2arg_fp n_conv_node_to_contents_fp,
	generic_void_2arg_fp n_insertion_sort_inner_loop_fp )
{
	node_array = n_node_array;
	the_free_list_backend_ptr = n_the_free_list_backend_ptr;
	total_num_nodes = n_total_num_nodes;
	
	specific_type_size = n_specific_type_size;
	whole_node_size = n_whole_node_size;
	
	specific_type_copy_fp = n_specific_type_copy_fp;
	specific_type_move_fp = n_specific_type_move_fp;
	specific_type_reset_fp = n_specific_type_reset_fp;
	specific_type_less_fp = n_specific_type_less_fp;
	
	get_node_data_fp = n_get_node_data_fp;
	get_index_pair_fp = n_get_index_pair_fp;
	
	conv_node_to_contents_fp = n_conv_node_to_contents_fp;
	insertion_sort_inner_loop_fp = n_insertion_sort_inner_loop_fp;
}

void list_backend::fully_deallocate()
{
	s32& the_front_index = get_front_index();
	//while ( get_front_index() != -1 )
	while ( the_front_index >= 0 )
	{
		//erase_at(get_front_index());
		erase_at(the_front_index);
	}
}

void list_backend::fully_deallocate_via_unlink()
{
	s32& the_front_index = get_front_index();
	//while ( get_front_index() != -1 )
	while ( the_front_index >= 0 )
	{
		//unlink_at(get_front_index());
		unlink_at(the_front_index);
	}
}

void list_backend::internal_func_allocate_and_assign_to_node
	( s32& index, node_contents& node,
	const void* n_data, u32 can_move_value )
{
	index = get_the_free_list_backend().peek_top_and_pop();
	
	//index = get_the_free_list_backend().peek_top();
	//get_the_free_list_backend().pop();
	node = get_node_contents_at(index);
	
	assign_to_node_data( node, n_data, can_move_value );
}


// push_front() CAN affect back_index
s32 list_backend::push_front( const void* to_push,
	u32 can_move_value )
{
	s32& the_front_index = get_front_index();
	s32 old_front_index = the_front_index;
	
	//sa_free_list_backend& the_free_list_backend 
	//	= get_the_free_list_backend();
	
	//the_front_index = the_free_list_backend.peek_top();
	//the_free_list_backend.pop();
	the_front_index = get_the_free_list_backend().peek_top_and_pop();
	
	
	//node<type>& the_front_node 
	//	= get_node_at(the_front_index);
	
	node_contents the_front_node = get_node_contents_at(the_front_index);
	
	s32 new_next_index = -1;
	
	// If there was nothing in the list before the push
	if ( old_front_index < 0 )
	{
		get_back_index() = the_front_index;
	}
	// If there's at least one element in the list
	else //if ( old_front_index >= 0 )
	{
		new_next_index = old_front_index;
		
		//get_node_at(old_front_index).prev_index()
		//	= the_front_index;
		get_prev_index_at_index(old_front_index) = the_front_index;
	}
	
	
	//the_front_node.data = to_push;
	//the_front_node.next_index() = new_next_index;
	//the_front_node.prev_index() = -1;
	assign_to_whole_node( the_front_node, to_push, 
		vec2_s16( new_next_index, -1 ), can_move_value );
	
	return the_front_index;
}

//s32 list_backend::internal_func_move_unlinked_node_to_front
//	( s32 to_move_index, node_contents& node_to_move )
//{
//}


// insert_before() won't affect back_index
s32 list_backend::insert_before( s32 index, const void* to_insert,
	u32 can_move_value )
{
	//s32 old_prev_index = get_node_at(index)
	//	.prev_index();
	////s32 old_next_index = get_node_at(index)
	////	.next_index()
	
	// If index == front_index
	//if ( old_prev_index == -1 )
	if ( index == get_front_index() )
	{
		return push_front( to_insert, can_move_value );
	}
	else
	{
		//node<type>& node_at_index 
		//	= get_node_at(index);
		node_contents node_at_index = get_node_contents_at(index);
		
		const s32 old_prev_index = node_at_index.prev_index();
		
		//const s32 new_index = get_the_free_list_backend()
		//	.peek_top();
		//get_the_free_list_backend().pop();
		const s32 new_index = get_the_free_list_backend()
			.peek_top_and_pop();
		
		
		//node<type>& node_at_new_index
		//	= get_node_at(new_index);
		node_contents node_at_new_index = get_node_contents_at(new_index);
		
		
		//get_node_at(old_prev_index).next_index()
		//	= new_index;
		
		get_next_index_at_index(old_prev_index) = new_index;
		
		//node_at_new_index.data = to_insert;
		//node_at_new_index.next_index() = index;
		//node_at_new_index.prev_index() = old_prev_index;
		
		assign_to_whole_node( node_at_new_index, to_insert, 
			vec2_s16( index, old_prev_index ), can_move_value );
		
		
		node_at_index.prev_index() = new_index;
		
		return new_index;
	}
	
	//return index;
	//return new_index;
}

//s32 list_backend::internal_func_move_unlinked_node_before
//	( s32 to_move_before_index, s32 to_move_index, 
//	node_contents& node_to_move )
//{
//}


// insert_after() CAN affect back_index
s32 list_backend::insert_after( s32 index, const void* to_insert,
	u32 can_move_value )
{
	////s32 old_prev_index = get_node_at(index)
	////	.prev_index();
	//s32 old_next_index = get_node_at(index)
	//	.next_index();
	
	//node<type>& node_at_index = get_node_at(index);
	//const s32 old_next_index = node_at_index
	//	.next_index();
	
	node_contents node_at_index = get_node_contents_at(index);
	const s32 old_next_index = node_at_index.next_index();
	
	
	////s32 new_index = get_the_free_list_backend().pop();
	//const s32 new_index = get_the_free_list_backend().peek_top();
	//get_the_free_list_backend().pop();
	const s32 new_index = get_the_free_list_backend().peek_top_and_pop();
	
	
	node_at_index.next_index() = new_index;
	
	//node<type>& node_at_new_index 
	//	= get_node_at(new_index);
	node_contents node_at_new_index = get_node_contents_at(new_index);
	
	s32 new_next_index = -1;
	
	// Special code is used for inserting an element at the end of the
	// list.
	if ( old_next_index < 0 )
	{
		get_back_index() = new_index;
	}
	else if ( old_next_index >= 0 )
	{
		//get_node_at(old_next_index).prev_index() 
		//	= new_index;
		get_prev_index_at_index(old_next_index) = new_index;
		new_next_index = old_next_index;
	}
	
	//node_at_new_index.data = to_insert;
	//node_at_new_index.next_index() = new_next_index;
	//node_at_new_index.prev_index() = index;
	
	assign_to_whole_node( node_at_new_index, to_insert,
		vec2_s16( new_next_index, index ), can_move_value );
	
	
	//return index;
	return new_index;
}


//s32 list_backend::internal_func_move_unlinked_node_after
//	( s32 to_move_after_index, s32 to_move_index, 
//	node_contents& node_to_move )
//{
//}





// This function is ever so slightly slower than it used to be, but it
// allows more flexiblity since some functions will call unlink_at (or
// erase_at) instead of calling this function directly.  
// 
// Functions that call this one directly are still able to pass in a
// pointer to the node_at_index.
//void* list_backend::internal_func_unlink_at_without_dealloc( s32 index,
//	node_contents* node_at_index_ptr )
//{
//}


// unlink_at() CAN affect back_index
void* list_backend::unlink_at( s32 index )
{
	//s32 old_prev_index = get_node_at(index)
	//	.prev_index(),
	//	old_next_index = get_node_at(index)
	//	.next_index();
	
	//node<type>& node_at_index = get_node_at(index);
	//
	//const s32 old_prev_index = node_at_index
	//	.prev_index(),
	//	old_next_index = node_at_index.next_index();
	
	
	
	s32& the_front_index = get_front_index();
	s32& the_back_index = get_back_index();
	
	const s32 old_front_index = the_front_index,
		old_back_index = the_back_index;
	
	//vec2_s16& index_pair_at_index = *get_index_pair_at_index(index);
	
	node_contents node_at_index = get_node_contents_at(index);
	vec2_s16& index_pair_at_index = *node_at_index.index_pair_ptr;
	
	const s32 old_next_index = index_pair_at_index
		[node_contents::vec2_index_for_next_index],
	old_prev_index = index_pair_at_index
		[node_contents::vec2_index_for_prev_index];
	
	index_pair_at_index = { -1, -1 };
	
	
	
	////node_at_index.data = type();
	////node_at_index.next_index() = -1;
	////node_at_index.prev_index() = -1;
	//node_at_index = node<type>();
	
	
	get_the_free_list_backend().push(index);
	
	if ( index == old_front_index )
	{
		the_front_index = old_next_index;
		
		if ( old_next_index >= 0 )
		{
			////front().prev_index() = -1;
			//get_node_at(the_front_index).prev_index() = -1;
			get_prev_index_at_index(the_front_index) = -1;
		}
	}
	else if ( index == old_back_index )
	{
		the_back_index = old_prev_index;
		
		if ( old_prev_index >= 0 )
		{
			get_next_index_at_index(the_back_index) = -1;
		}
	}
	else
	{
		//node<type>& old_prev_node 
		//	= get_node_at(old_prev_index);
		s16& the_next_index = get_next_index_at_index(old_prev_index);
		
		if ( old_next_index >= 0 )
		{
			//old_prev_node.next_index() = old_next_index;
			//get_node_at(old_next_index).prev_index()
			//	= old_prev_index;
			
			the_next_index = old_next_index;
			get_prev_index_at_index(old_next_index) = old_prev_index;
		}
		else
		{
			//old_prev_node.next_index() = -1;
			the_next_index = -1;
		}
	}
	
	return node_at_index.data_ptr;
}

void list_backend::internal_func_unlink_from_connected_index_at( s32 index, 
	u32 index_to_vec2 )
{
	vec2_s16& index_pair_at_index = *get_index_pair_at_index(index);
	
	const s32 old_other_index = index_pair_at_index[index_to_vec2];
	
	
	// Check whether index is at the front or back of the list.
	if ( old_other_index < 0 )
	{
		return;
	}
	
	index_pair_at_index[index_to_vec2] = -1;
	
	vec2_s16& index_pair_at_other_index = *get_index_pair_at_index
		(old_other_index);
	
	// Generic code
	index_pair_at_other_index[!index_to_vec2] = -1;
	
}



// This is not quite the same algorithm as insertion sort.  In fact, it is
// possible to optimize it, using extra space, by exploiting the fact that
// this algorithm SEARCHES FORWARD to find ONLY ONE node to move.
s32 list_backend::insertion_sort()
{
	s32& the_front_index = get_front_index();
	
	// Don't do anything if this list has zero or one nodes.
	if ( the_front_index < 0 )
	{
		return the_front_index;
	}
	
	
	//if ( get_node_at(the_front_index).next_index() < 0 )
	node_contents the_front_node = get_node_contents_at(the_front_index);
	
	s32 front_node_next_index = the_front_node.next_index();
	
	if ( front_node_next_index < 0 )
	{
		return the_front_index;
	}
	
	
	list_backend sorted_list(*this);
	
	s32& temp_front_index = sorted_list.get_front_index();
	
	
	
	//s32 curr_index = temp_front_index;
	
	for ( s32 i=the_front_index;
		i!=-1; )
		//i=get_next_index_at_index(i) )
	{
		s32 index_low = i;
		
		call_insertion_sort_inner_loop_fp(index_low);
		
		node_contents node_at_index_low = get_node_contents_at(index_low);
		
		if ( i == index_low )
		{
			i = node_at_index_low.next_index();
		}
		
		//void* data_to_move = unlink_at(index_low);
		//sorted_list.push_back( data_to_move, true );
		sorted_list.push_back( unlink_at(index_low), true );
		
		//internal_func_unlink_at_without_dealloc( index_low,
		//	&node_at_index_low );
		
		//if ( temp_front_index < 0 )
		//{
		//	////move_linked_node_to_front( index_low, node_at_index_low, 
		//	////	sorted_list );
		//	//
		//	//sorted_list.internal_func_move_unlinked_node_to_front
		//	//	( index_low, node_at_index_low );
		//	sorted_list.push_front( data_to_move, true );
		//	curr_index = temp_front_index;
		//}
		//else
		//{
		//	////move_linked_node_after( curr_index, index_low,
		//	////	node_at_index_low, sorted_list );
		//	//
		//	//sorted_list.internal_func_move_unlinked_node_after( curr_index,
		//	//	index_low, node_at_index_low );
		//	sorted_list.insert_after( curr_index, data_to_move, true );
		//	curr_index = sorted_list.get_next_index_at_index(curr_index);
		//}
	}
	
	
	the_front_index = temp_front_index;
	
	// Prevent any nodes from being erased when sorted_list's destructor is
	// called.
	temp_front_index = -1;
	
	
	return the_front_index;
}

//s32 list_backend::insertion_sort()
//{
//	return get_front_index();
//}







//void list_backend::internal_func_merge( merge_args& args )
//{
//	s32& the_front_index = get_front_index();
//	
//	//node_contents left_node = get_node_contents_at(args.get_left_index()),
//	//	right_node = get_node_contents_at(args.get_right_index()),
//	//	out_node = get_node_contents_at(args.get_out_index());
//	node_contents left_node = args.get_left_node(this),
//		right_node = args.get_right_node(this);
//	
//	size_t i = 0, j = 0;
//	
//	
//	
//	auto merge_left_node = [&]() -> void
//	{
//		
//		
//		++i;
//	};
//	
//	auto merge_right_node = [&]() -> void
//	{
//		
//		
//		++j;
//	};
//	
//	for ( size_t k = 0; k<args.get_out_size(); ++k )
//	{
//		if ( i >= args.get_left_size() )
//		{
//			merge_right_node();
//		}
//		else if ( j >= args.get_right_size() )
//		{
//			merge_left_node();
//		}
//		else // if ( i < args.get_left_size() 
//			// && j < args.get_right_size() )
//		{
//			if ( call_specific_type_less_func( left_node.data_ptr,
//				right_node.data_ptr ) )
//			{
//				merge_left_node();
//			}
//			else
//			{
//				merge_right_node();
//			}
//		}
//	}
//	
//}


s32 list_backend::merge_sort()
{
	s32& the_front_index = get_front_index();
	
	
	// Don't do anything if this list has zero or one nodes.
	if ( the_front_index < 0 )
	{
		return the_front_index;
	}
	if ( get_next_index_at_index(the_front_index) < 0 )
	{
		return the_front_index;
	}
	
	
	
	return the_front_index;
}


} // end of namespace sa_list_stuff

