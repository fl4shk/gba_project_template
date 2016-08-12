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



void sa_list_backend::init( void* n_node_array, 
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
	generic_void_2arg_fp n_loop_with_j_during_insertion_sort_fp )
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
	loop_with_j_during_insertion_sort_fp
		= n_loop_with_j_during_insertion_sort_fp;
}

void sa_list_backend::fully_deallocate()
{
	s32& the_front_index = get_front_index();
	//while ( get_front_index() != -1 )
	while ( the_front_index >= 0 )
	{
		//erase_at(get_front_index());
		erase_at(the_front_index);
	}
}

void sa_list_backend::fully_deallocate_via_unlink()
{
	s32& the_front_index = get_front_index();
	//while ( get_front_index() != -1 )
	while ( the_front_index >= 0 )
	{
		//unlink_at(get_front_index());
		unlink_at(the_front_index);
	}
}

void sa_list_backend::internal_func_allocate_and_assign_to_node
	( s32& index, sa_list_node_contents& node,
	const void* n_data, u32 can_move_value )
{
	index = get_the_free_list_backend().peek_top_and_pop();
	
	//index = get_the_free_list_backend().peek_top();
	//get_the_free_list_backend().pop();
	node = get_node_contents_at(index);
	
	assign_to_node_data( node, n_data, can_move_value );
}



s32 sa_list_backend::push_front( const void* to_push,
	u32 can_move_value )
{
	s32& the_front_index = get_front_index();
	const s32 old_front_index = the_front_index;
	
	sa_free_list_backend& the_free_list_backend 
		= get_the_free_list_backend();
	
	the_front_index = the_free_list_backend.peek_top();
	the_free_list_backend.pop();
	
	
	sa_list_node_contents the_front_node
		= get_node_contents_at(the_front_index);
	
	
	
	s32 new_next_index = -1;
	
	// If there's nothing in the list
	//if ( the_front_index < 0 )
	//if ( old_front_index < 0 )
	//{
	//	
	//}
	//// If there's at least one element in the list
	//else
	if ( old_front_index >= 0 )
	{
		new_next_index = old_front_index;
		
		//get_node_at(old_front_index).prev_index
		//	= the_front_index;
		get_prev_index_at_index(old_front_index)
			= the_front_index;
	}
	
	
	//copy_node_data( the_front_node.data_ptr, to_push );
	//the_front_node.next_index() = new_next_index;
	//the_front_node.prev_index() = -1;
	
	assign_to_whole_node( the_front_node, to_push,
		vec2_s16( new_next_index, -1 ), can_move_value );
	
	
	
	return the_front_index;
}

//s32 sa_list_backend::move_node_to_front( s32 to_move_index,
//	sa_list_node_contents& node_to_move )
//{
//	s32& the_front_index = get_front_index();
//	const s32 old_front_index = the_front_index;
//	
//	the_front_index = to_move_index;
//	
//	
//	s32 new_next_index = -1;
//	
//	//// If there's nothing in the list
//	//if ( old_front_index < 0 )
//	//{
//	//}
//	//// If there's at least one element in the list
//	//else
//	if ( old_front_index >= 0 )
//	{
//		new_next_index = old_front_index;
//		
//		get_prev_index_at_index(old_front_index)
//			= the_front_index;
//	}
//	
//	//copy_node_data( the_front_node.data_ptr, to_push );
//	//the_front_node.next_index() = new_next_index;
//	//the_front_node.prev_index() = -1;
//	
//	//assign_to_whole_node( the_front_node, to_push,
//	//	vec2_s16( new_next_index, -1 ), can_move_value );
//	
//	copy_vec2_s16_via_ptr( *node_to_move.index_pair_ptr,
//		vec2_s16( new_next_index, -1 ) );
//	
//	
//	return the_front_index;
//}

s32 sa_list_backend::insert_before( s32 index, const void* to_insert,
	u32 can_move_value )
{
	// If index == front_index
	//if ( old_prev_index == -1 )
	if ( index == get_front_index() )
	{
		return push_front(to_insert);
	}
	else
	{
		sa_list_node_contents node_at_index
			= get_node_contents_at(index);
		
		const s32 old_prev_index = node_at_index
			.prev_index();
		
		const s32 new_index = get_the_free_list_backend()
			.peek_top();
		get_the_free_list_backend().pop();
		
		
		//sa_list_node<type>& node_at_new_index
		//	= get_node_at(new_index);
		sa_list_node_contents node_at_new_index
			= get_node_contents_at(new_index);
		
		
		get_next_index_at_index(old_prev_index)
			= new_index;
		
		
		//copy_node_data( node_at_new_index.data_ptr, 
		//	to_insert );
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

//s32 sa_list_backend::move_node_before( s32 to_move_before_index, 
//		s32 to_move_index, sa_list_node_contents& node_to_move )
//{
//	// If index == front_index
//	if ( to_move_before_index == get_front_index() )
//	{
//		return move_node_to_front( to_move_index, node_to_move );
//	}
//	else
//	{
//		sa_list_node_contents node_to_move_before
//			= get_node_contents_at(to_move_before_index);
//		
//		const s32 old_prev_index = node_to_move_before
//			.prev_index();
//		
//		
//		// Allocating a node index in this function is no longer necessary
//		// since this function is now only a node-moving function.
//		
//		
//		get_next_index_at_index(old_prev_index)
//			= to_move_index;
//		
//		
//		//copy_node_data( node_at_new_index.data_ptr, 
//		//	to_insert );
//		//node_at_new_index.next_index() = index;
//		//node_at_new_index.prev_index() = old_prev_index;
//		//assign_to_whole_node( node_at_new_index, to_insert,
//		//	vec2_s16( index, old_prev_index ), can_move_value );
//		
//		copy_vec2_s16_via_ptr( *node_to_move.index_pair_ptr,
//			vec2_s16( to_move_before_index, old_prev_index ) );
//		
//		
//		//node_at_index.prev_index() = new_index;
//		node_to_move_before.prev_index() = to_move_index;
//		
//		//return new_index;
//		return to_move_index;
//	}
//	
//	////return index;
//	////return new_index;
//	//return to_move_index;
//}



s32 sa_list_backend::insert_after( s32 index, const void* to_insert,
	u32 can_move_value )
{
	//////s32 old_prev_index = get_node_at(index)
	//////	.prev_index();
	////s32 old_next_index = get_node_at(index)
	////	.next_index();
	//sa_list_node<type>& node_at_index = get_node_at(index);
	sa_list_node_contents node_at_index = get_node_contents_at
		(index);
	const s32 old_next_index = node_at_index
		.next_index();
	
	//s32 new_index = get_the_free_list_backend().pop();
	//const s32 new_index = get_the_free_list_backend().peek_top();
	//get_the_free_list_backend().pop();
	const s32 new_index = get_the_free_list_backend()
		.peek_top_and_pop();
	
	
	node_at_index.next_index() = new_index;
	
	//sa_list_node<type>& node_at_new_index 
	//	= get_node_at(new_index);
	sa_list_node_contents node_at_new_index
		= get_node_contents_at(new_index);
	
	s32 new_next_index = -1;
	
	// Special code is used for inserting an element at the end of the
	// list.
	//if ( old_next_index < 0 )
	//{
	//	
	//}
	//else
	if ( old_next_index >= 0 )
	{
		//get_node_at(old_next_index).prev_index() 
		//	= new_index;
		get_prev_index_at_index(old_next_index)
			= new_index;
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


//s32 sa_list_backend::move_node_after( s32 to_move_after_index, 
//	s32 to_move_index, sa_list_node_contents& node_to_move )
//{
//	sa_list_node_contents node_to_move_after = get_node_contents_at
//		(to_move_after_index);
//	const s32 old_next_index = node_to_move_after.next_index();
//	
//	
//	// Allocating a node index in this function is no longer necessary
//	// since this function is now only a node-moving function.
//	
//	node_to_move_after.next_index() = to_move_index;
//	
//	s32 new_next_index = -1;
//	
//	// Special code is used for inserting an element at the end of the
//	// list.
//	//if ( old_next_index < 0 )
//	//{
//	//	
//	//}
//	//else
//	if ( old_next_index >= 0 )
//	{
//		get_prev_index_at_index(old_next_index)
//			= to_move_index;
//		new_next_index = old_next_index;
//	}
//	
//	//node_at_new_index.data = to_insert;
//	//node_at_new_index.next_index() = new_next_index;
//	//node_at_new_index.prev_index() = index;
//	//assign_to_whole_node( node_at_new_index, to_insert, 
//	//	vec2_s16( new_next_index, index ), can_move_value );
//	
//	copy_vec2_s16_via_ptr( *node_to_move.index_pair_ptr, 
//		vec2_s16( new_next_index, to_move_after_index ) );
//	
//	return to_move_index;
//}





// This function is ever so slightly slower than it used to be, but it
// allows more flexiblity since some functions will call unlink_at (or
// erase_at) instead of calling this function directly.  
// 
// Functions that call this one directly are still able to pass in a
// pointer to the node_at_index.
void* sa_list_backend::unlink_at_without_dealloc( s32 index,
	sa_list_node_contents* node_at_index_ptr )
{
	//s32 old_prev_index = get_node_at(index)
	//	.prev_index(),
	//	old_next_index = get_node_at(index)
	//	.next_index();
	
	//sa_list_node<type>& node_at_index = get_node_at(index);
	
	//const s32 old_next_index = node_at_index
	//	.next_index(),
	//	old_prev_index = node_at_index.prev_index();
	
	//sa_list_node_contents node_at_index = get_node_contents_at
	//	(index);
	
	sa_list_node_contents local_node_at_index;
	sa_list_node_contents* local_node_at_index_ptr;
	
	if (node_at_index_ptr)
	{
		local_node_at_index_ptr = node_at_index_ptr;
	}
	else //if ( node_at_index_ptr == NULL )
	{
		local_node_at_index = get_node_contents_at(index);
		local_node_at_index_ptr = &local_node_at_index;
	}
	
	//const s32 old_next_index = local_node_at_index_ptr
	//	->next_index(),
	//old_prev_index = local_node_at_index_ptr
	//	->prev_index();
	
	vec2_s16& the_index_pair = *local_node_at_index_ptr
		->index_pair_ptr;
	const s32 old_next_index = the_index_pair
		[sa_list_node_contents::vec2_index_for_next_index],
	old_prev_index = the_index_pair
		[sa_list_node_contents::vec2_index_for_prev_index];
	
	
	
	s32& the_front_index = get_front_index();
	const s32 old_front_index = the_front_index;
	
	////node_at_index.data = type();
	////node_at_index.next_index() = -1;
	////node_at_index.prev_index() = -1;
	//node_at_index = sa_list_node<type>();
	//
	//get_the_free_list_backend().push(index);
	
	// Simply erase index_pair
	//*node_at_index.index_pair_ptr = { -1, -1 };
	//*local_node_at_index_ptr->index_pair_ptr = { -1, -1 };
	the_index_pair = { -1, -1 };
	
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
	else
	{
		//sa_list_node<type>& old_prev_node 
		//	= get_node_at(old_prev_index);
		
		//sa_list_node_contents old_prev_node = get_node_contents_at
		//	(old_prev_index);
		s16& old_prev_node_next_index 
			= get_next_index_at_index(old_prev_index);
		
		if ( old_next_index >= 0 )
		{
			//old_prev_node.next_index() = old_next_index;
			//get_node_at(old_next_index).prev_index()
			//	= old_prev_index;
			
			old_prev_node_next_index = old_next_index;
			get_prev_index_at_index(old_next_index)
				= old_prev_index;
		}
		else
		{
			//old_prev_node.next_index() = -1;
			old_prev_node_next_index = -1;
		}
	}
	
	//return node_at_index.data_ptr;
	return local_node_at_index_ptr->data_ptr;
}



// This is not quite the same algorithm as insertion sort.  In fact, it is
// possible to optimize it, using extra space, by exploiting the fact that
// this algorithm SEARCHES FORWARD to find ONLY ONE node to move.
s32 sa_list_backend::insertion_sort()
{
	s32& the_front_index = get_front_index();
	
	// Don't do anything if this list has zero or one nodes.
	if ( the_front_index < 0 )
	{
		return the_front_index;
	}
	//if ( get_node_at(the_front_index).next_index() < 0 )
	if ( get_next_index_at_index(the_front_index) < 0 )
	{
		return the_front_index;
	}
	
	////s32 temp_front_index = -1;
	////sa_list_backend<type> sorted_list( &temp_front_index, 
	////	node_array, the_free_list_backend_ptr, 
	////	total_num_nodes );
	//externally_allocated_sa_list<type> sorted_list( node_array, 
	//	the_free_list_backend_ptr, get_total_num_nodes() );
	sa_list_backend sorted_list(*this);
	
	s32& temp_front_index = sorted_list.get_front_index();
	
	s32 curr_index = temp_front_index;
	
	
	
	
	s32 index_low = the_front_index;
	s32 outer_i = the_front_index;
	
	
	
	// Unroll the first outer loop
	get_loop_with_j_during_insertion_sort_fp()
		( get_node_array(), &index_low );
	
	//sa_list_node<type>& node_at_index_low = get_node_at(index_low);
	//const type data_to_move = node_at_index_low.data;
	sa_list_node_contents node_at_index_low = get_node_contents_at
		(index_low);
	
	if ( outer_i == index_low )
	{
		outer_i = node_at_index_low.next_index();
	}
	//outer_i = node_at_index_low.next_index();
	//outer_i = get_next_index_at_index(the_front_index);
	
	
	//erase_at(index_low);
	void* outer_data_to_move = unlink_at(index_low);
	sorted_list.push_front( outer_data_to_move, true );
	
	//unlink_at_without_dealloc( index_low, &node_at_index_low );
	////unlink_at_without_dealloc(index_low);
	//sorted_list.move_node_to_front( index_low, node_at_index_low );
	
	curr_index = temp_front_index;
	
	
	
	for ( s32 i=outer_i;
		i!=-1;  )
		////i=get_node_at(i).next_index() )
		//i=get_next_index_at_index(i) )
	{
		index_low = i;
		
		get_loop_with_j_during_insertion_sort_fp()
			( get_node_array(), &index_low );
		
		//sa_list_node<type>& node_at_index_low = get_node_at(index_low);
		//const type data_to_move = node_at_index_low.data;
		node_at_index_low = get_node_contents_at(index_low);
		
		// if the current node 
		if ( i == index_low )
		{
			i = node_at_index_low.next_index();
		}
		
		//erase_at(index_low);
		void* data_to_move = unlink_at(index_low);
		
		sorted_list.insert_after( curr_index, data_to_move, true );
		curr_index = sorted_list.get_next_index_at_index
			(curr_index);
	}
	
	
	the_front_index = temp_front_index;
	
	temp_front_index = -1;
	
	
	return the_front_index;
}

//s32 sa_list_backend::insertion_sort()
//{
//	return get_front_index();
//}



// This specific merge sort algorithm was borrowed from here:  
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html

s32 sa_list_backend::merge_sort()
{
	s32& the_front_index = get_front_index();
	
	//if ( the_front_index < 0 )
	//{
	//	return the_front_index;
	//}
	//
	//s32 index_p_start, index_q_start, index_e_start, end_of_sorted_list;
	//s32 in_size = 1, num_merges, p_size, size_q, i;
	//
	//sa_list_backend sorted_list(*this);
	//
	//for (;;)
	//{
	//	
	//}
	//
	
	return the_front_index;
}

