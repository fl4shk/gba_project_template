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



void sa_list_backend::init( void* n_the_node_array, 
	sa_free_list_backend* n_ptr_to_the_free_list_backend,
	u32 n_total_num_nodes, u32 n_specific_type_size,
	u32 n_whole_node_size,
	
	decltype(specific_type_copy_fp) n_specific_type_copy_fp, 
	decltype(specific_type_move_fp) n_specific_type_move_fp, 
	decltype(specific_type_reset_fp) n_specific_type_reset_fp,
	decltype(specific_type_less_fp) n_specific_type_less_fp, 
	
	decltype(get_node_data_fp) n_get_node_data_fp, 
	decltype(get_node_index_pair_fp) n_get_node_index_pair_fp,
	decltype(conv_node_to_contents_fp) n_conv_node_to_contents_fp )
{
	the_node_array = n_the_node_array;
	ptr_to_the_free_list_backend = n_ptr_to_the_free_list_backend;
	total_num_nodes = n_total_num_nodes;
	
	specific_type_size = n_specific_type_size;
	whole_node_size = n_whole_node_size;
	
	specific_type_copy_fp = n_specific_type_copy_fp;
	specific_type_move_fp = n_specific_type_move_fp;
	specific_type_reset_fp = n_specific_type_reset_fp;
	specific_type_less_fp = n_specific_type_less_fp;
	
	get_node_data_fp = n_get_node_data_fp;
	get_node_index_pair_fp = n_get_node_index_pair_fp;
	
	conv_node_to_contents_fp = n_conv_node_to_contents_fp;
}



s32 sa_list_backend::push_front( const void* to_push,
	u32 can_move_value )
{
	s32& the_front_node_index = get_front_node_index();
	const s32 old_front_node_index = the_front_node_index;
	
	sa_free_list_backend& the_free_list_backend 
		= get_the_free_list_backend();
	
	the_front_node_index = the_free_list_backend.peek_top();
	the_free_list_backend.pop();
	
	
	sa_list_node_contents the_front_node
		= get_node_contents_at(the_front_node_index);
	
	
	
	s32 new_next_node_index = -1;
	
	// If there's nothing in the list
	//if ( the_front_node_index < 0 )
	//if ( old_front_node_index < 0 )
	//{
	//	
	//}
	//// If there's at least one element in the list
	//else
	if ( old_front_node_index >= 0 )
	{
		new_next_node_index = old_front_node_index;
		
		//get_node_at(old_front_node_index).prev_node_index
		//	= the_front_node_index;
		get_prev_node_index_at_node_index(old_front_node_index)
			= the_front_node_index;
	}
	
	
	//copy_node_data( the_front_node.ptr_to_the_data, to_push );
	//the_front_node.next_node_index() = new_next_node_index;
	//the_front_node.prev_node_index() = -1;
	
	assign_to_whole_node( the_front_node, to_push,
		vec2_s16( new_next_node_index, -1 ), can_move_value );
	
	
	
	return the_front_node_index;
}


s32 sa_list_backend::insert_before( s32 node_index, const void* to_insert,
	u32 can_move_value )
{
	// If node_index == front_node_index
	//if ( old_prev_node_index == -1 )
	if ( node_index == get_front_node_index() )
	{
		return push_front(to_insert);
	}
	else
	{
		sa_list_node_contents node_at_node_index
			= get_node_contents_at(node_index);
		
		const s32 old_prev_node_index = node_at_node_index
			.prev_node_index();
		
		const s32 new_node_index = get_the_free_list_backend()
			.peek_top();
		get_the_free_list_backend().pop();
		
		
		//sa_list_node<type>& node_at_new_node_index
		//	= get_node_at(new_node_index);
		sa_list_node_contents node_at_new_node_index
			= get_node_contents_at(new_node_index);
		
		
		get_next_node_index_at_node_index(old_prev_node_index)
			= new_node_index;
		
		
		//copy_node_data( node_at_new_node_index.ptr_to_the_data, 
		//	to_insert );
		//node_at_new_node_index.next_node_index() = node_index;
		//node_at_new_node_index.prev_node_index() = old_prev_node_index;
		assign_to_whole_node( node_at_new_node_index, to_insert,
			vec2_s16( node_index, old_prev_node_index ), can_move_value );
		
		
		node_at_node_index.prev_node_index() = new_node_index;
		
		return new_node_index;
	}
	
	//return node_index;
	//return new_node_index;
	
}


s32 sa_list_backend::insert_after( s32 node_index, const void* to_insert,
	u32 can_move_value )
{
	//////s32 old_prev_node_index = get_node_at(node_index)
	//////	.prev_node_index();
	////s32 old_next_node_index = get_node_at(node_index)
	////	.next_node_index();
	//sa_list_node<type>& node_at_node_index = get_node_at(node_index);
	sa_list_node_contents node_at_node_index = get_node_contents_at
		(node_index);
	const s32 old_next_node_index = node_at_node_index
		.next_node_index();
	
	//s32 new_node_index = get_the_free_list_backend().pop();
	const s32 new_node_index = get_the_free_list_backend().peek_top();
	get_the_free_list_backend().pop();
	
	
	node_at_node_index.next_node_index() = new_node_index;
	
	//sa_list_node<type>& node_at_new_node_index 
	//	= get_node_at(new_node_index);
	sa_list_node_contents node_at_new_node_index
		= get_node_contents_at(new_node_index);
	
	s32 new_next_node_index = -1;
	
	// Special code is used for inserting an element at the end of the
	// list.
	//if ( old_next_node_index < 0 )
	//{
	//	
	//}
	//else
	if ( old_next_node_index >= 0 )
	{
		//get_node_at(old_next_node_index).prev_node_index() 
		//	= new_node_index;
		get_prev_node_index_at_node_index(old_next_node_index)
			= new_node_index;
		new_next_node_index = old_next_node_index;
	}
	
	//node_at_new_node_index.the_data = to_insert;
	//node_at_new_node_index.next_node_index() = new_next_node_index;
	//node_at_new_node_index.prev_node_index() = node_index;
	assign_to_whole_node( node_at_new_node_index, to_insert, 
		vec2_s16( new_next_node_index, node_index ), can_move_value );
	
	
	//return node_index;
	return new_node_index;
}



void* sa_list_backend::unlink_at( s32 node_index )
{
	////s32 old_prev_node_index = get_node_at(node_index)
	////	.prev_node_index(),
	////	old_next_node_index = get_node_at(node_index)
	////	.next_node_index();
	//
	//sa_list_node<type>& node_at_node_index = get_node_at(node_index);
	sa_list_node_contents node_at_node_index = get_node_contents_at
		(node_index);
	
	//const s32 old_prev_node_index = node_at_node_index
	//	.prev_node_index(),
	//	old_next_node_index = node_at_node_index.next_node_index();
	
	vec2_s16& old_node_index_pair = *node_at_node_index
		.ptr_to_node_index_pair;
	
	const s32 old_next_node_index = old_node_index_pair
		[sa_list_node_contents::index_for_next_node_index],
	old_prev_node_index = old_node_index_pair
		[sa_list_node_contents::index_for_prev_node_index];
	
	
	s32& the_front_node_index = get_front_node_index();
	const s32 old_front_node_index = the_front_node_index;
	
	////node_at_node_index.the_data = type();
	////node_at_node_index.next_node_index() = -1;
	////node_at_node_index.prev_node_index() = -1;
	//node_at_node_index = sa_list_node<type>();
	
	
	// I guess don't erase the actual data.  That should be good for
	// sorting since it won't require copying the data.
	old_node_index_pair = { -1, -1 };
	
	
	get_the_free_list_backend().push(node_index);
	
	
	if ( node_index == old_front_node_index )
	{
		the_front_node_index = old_next_node_index;
		
		if ( old_next_node_index >= 0 )
		{
			////front().prev_node_index() = -1;
			//get_node_at(the_front_node_index).prev_node_index() = -1;
			get_prev_node_index_at_node_index(the_front_node_index)
				= -1;
		}
	}
	else
	{
		//sa_list_node<type>& old_prev_node 
		//	= get_node_at(old_prev_node_index);
		//sa_list_node_contents old_prev_node = get_node_contents_at
		//	(old_prev_node_index)
		
		s16& old_prev_node_next_node_index 
			= get_next_node_index_at_node_index(old_prev_node_index);
		
		if ( old_next_node_index >= 0 )
		{
			//old_prev_node.next_node_index() = old_next_node_index;
			//get_node_at(old_next_node_index).prev_node_index()
			//	= old_prev_node_index;
			
			old_prev_node_next_node_index = old_next_node_index;
			get_prev_node_index_at_node_index(old_next_node_index)
				= old_prev_node_index;
		}
		else // if the erased node was at the end of the list
		{
			//old_prev_node.next_node_index() = -1;
			old_prev_node_next_node_index = -1;
		}
	}
	
	
	return node_at_node_index.ptr_to_the_data;
}

s32 sa_list_backend::insertion_sort()
{
	s32& the_front_node_index = get_front_node_index();
	
	// Don't do anything if this list has zero or one nodes.
	if ( the_front_node_index < 0 )
	{
		return the_front_node_index;
	}
	//if ( get_node_at(the_front_node_index).next_node_index() < 0 )
	if ( get_next_node_index_at_node_index(the_front_node_index) < 0 )
	{
		return the_front_node_index;
	}
	
	
	////s32 temp_front_node_index = -1;
	////sa_list_backend<type> sorted_list( &temp_front_node_index, 
	////	the_node_array, ptr_to_the_free_list_backend, 
	////	total_num_nodes );
	//externally_allocated_sa_list<type> sorted_list( the_node_array, 
	//	ptr_to_the_free_list_backend, get_total_num_nodes() );
	sa_list_backend sorted_list(*this);
	
	s32& temp_front_node_index = sorted_list.get_front_node_index();
	
	
	static constexpr u32 prev_index_low_arr_size = 20;
	s32 prev_index_low_arr[prev_index_low_arr_size];
	
	//for ( s32 i=prev_index_low_arr_size-1; i>=0; --i )
	//{
	//	prev_index_low_arr[i] = 0;
	//}
	arr_memfill32( prev_index_low_arr, 0, prev_index_low_arr_size );
	
	
	s32 curr_node_index = temp_front_node_index;
	
	for ( s32 i=the_front_node_index;
		i!=-1;  )
		//i=get_node_at(i).next_node_index() )
	{
		u32 num_extra_low_indices = 0;
		
		
		s32 index_low = i;
		
		//sa_list_node<type>* node_at_j;
		sa_list_node_contents node_at_j;
		
		//type* the_data_at_index_low = &get_node_at(index_low).the_data;
		void* the_data_at_index_low = get_node_data_at(index_low);
		
		// Find the lowest value at or after i.
		for ( s32 j=index_low;
			j!=-1; 
			//j=node_at_j->next_node_index() )
			j=node_at_j.next_node_index() )
		{
			//node_at_j = &get_node_at(j);
			node_at_j = get_node_contents_at(j);
			
			//if ( node_at_j->the_data 
			//	< *the_data_at_index_low )
			if ( call_specific_type_less_func( node_at_j.ptr_to_the_data,
				the_data_at_index_low ) )
			{
				if ( num_extra_low_indices + 1 
					< prev_index_low_arr_size )
				{
					prev_index_low_arr[num_extra_low_indices++]
						= index_low;
				}
				
				index_low = j;
				
				//the_data_at_index_low = &get_node_at(index_low).the
				//the_data_at_index_low = &node_at_j->the_data;
				the_data_at_index_low = node_at_j.ptr_to_the_data;
			}
		}
		
		//sa_list_node<type>& node_at_index_low = get_node_at(index_low);
		////const type data_to_move = node_at_index_low.the_data;
		////type&& data_to_move = node_at_index_low.the_data
		sa_list_node_contents node_at_index_low = get_node_contents_at
			(index_low);
		
		if ( i == index_low )
		{
			i = node_at_index_low.next_node_index();
		}
		
		//////erase_at(index_low);
		////type&& data_to_move = unlink_at(index_low);
		//void* data_to_move = unlink_at(index_low);
		void* data_to_move = node_at_index_low.ptr_to_the_data;
		unlink_at(index_low);
		
		if ( temp_front_node_index < 0 )
		{
			//sorted_list.push_front(std::move(data_to_move));
			sorted_list.push_front( data_to_move, true );
			curr_node_index = temp_front_node_index;
		}
		else
		{
			//sorted_list.insert_after( curr_node_index, 
			//	std::move(data_to_move) );
			//curr_node_index = sorted_list.get_node_at(curr_node_index)
			//	.next_node_index();
			sorted_list.insert_after( curr_node_index,
				data_to_move, true );
			curr_node_index = sorted_list.get_next_node_index_at_node_index
				(curr_node_index);
		}
		
		
		for ( u32 j=0; j<num_extra_low_indices; ++j )
		{
			s32& curr_prev_index_low = prev_index_low_arr[j];
			
			//sa_list_node<type>& node_at_curr_prev_index_low 
			//	= get_node_at(curr_prev_index_low);
			sa_list_node_contents node_at_curr_prev_index_low
				= get_node_contents_at(curr_prev_index_low);
			
			if ( i == curr_prev_index_low )
			{
				i = node_at_curr_prev_index_low.next_node_index();
			}
			
			//type&& curr_data_to_move = unlink_at(curr_prev_index_low);
			void* curr_data_to_move = node_at_curr_prev_index_low
				.ptr_to_the_data;
			unlink_at(curr_prev_index_low);
			
			//if ( temp_front_node_index < 0 )
			//{
			//	//sorted_list.push_front(std::move(data_to_move));
			//	sorted_list.push_front( data_to_move, true );
			//	curr_node_index = temp_front_node_index;
			//}
			//else
			{
				//sorted_list.insert_after( curr_node_index, 
				//	std::move(curr_data_to_move) );
				//curr_node_index = sorted_list.get_node_at
				//	(curr_node_index).next_node_index();
				sorted_list.insert_after( curr_node_index,
					curr_data_to_move, true );
				curr_node_index = sorted_list
					.get_next_node_index_at_node_index(curr_node_index);
			}
		}
	}
	
	
	the_front_node_index = temp_front_node_index;
	
	temp_front_node_index = -1;
	
	
	return the_front_node_index;
}


s32 sa_list_backend::merge_sort()
{
	//u32 num_remaining_sublists = 0;
	//
	//sa_list_node_contents node_at_i;
	//
	//for ( s32 i=get_front_node_index();
	//	i>=0;
	//	i=node_at_i.next_node_index() )
	//{
	//	node_at_i = get_node_contents_at(i);
	//	
	//	
	//}
	
	return get_front_node_index();
	
}

