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


#ifndef sa_list_class_stuff_hpp
#define sa_list_class_stuff_hpp

#include "misc_types.hpp"
#include "../gba_specific_stuff/attribute_defines.hpp"
//#include "sa_dyn_array_class.hpp"
#include "sa_stack_classes.hpp"

#include "../gba_specific_stuff/asm_funcs.hpp"

#include "generic_funcptr_stuff.hpp"


#include <functional>


// A non-template class intended to be used by the sa_list_backend class.
class sa_list_node_contents
{
public:		// constants
	static constexpr size_t vec2_index_for_next_index = vec2_index_for_x,
		vec2_index_for_prev_index = vec2_index_for_y;
	
public:		// variables
	void* data_ptr;
	
	vec2_s16* index_pair_ptr;
	
public:		// functions
	// This is safe so long as data_ptr is the first member variable of the
	// sa_list_node class.
	inline void* get_sa_list_node_as_void_ptr()
	{
		return data_ptr;
	}
	
	template< typename type >
	inline type* cast_data_ptr()
	{
		return static_cast<type*>(data_ptr);
	}
	
	inline s16& next_index()
	{
		return (*index_pair_ptr)[vec2_index_for_next_index];
	}
	inline s16& prev_index()
	{
		return (*index_pair_ptr)[vec2_index_for_prev_index];
	}
	
} __attribute__((_align4));


template< typename type >
class sa_list_node
{
//public:		// constants
//	static constexpr size_t vec2_index_for_next_index 
//		= sa_list_node_contents::vec2_index_for_next_index,
//	vec2_index_for_prev_index 
//		= sa_list_node_contents::vec2_index_for_prev_index;
	
public:		// variables
	type data __attribute__((_align4)) = type();
	//vec2_s32 index_pair = { -1, -1 };
	vec2_s16 index_pair = { -1, -1 };
	
public:		// functions
	inline sa_list_node()
	{
	}
	
	//inline sa_list_node()
	//{
	//	data = type();
	//	next_index = -1;
	//	prev_index = -1;
	//}
	
	template< typename type_in_vec2 >
	inline sa_list_node( const type& data_to_copy, 
		vec2<type_in_vec2> s_index_pair ) : data(data_to_copy)
	{
		next_index() = get_next_index_from_vec2(s_index_pair);
		prev_index() = get_prev_index_from_vec2(s_index_pair);
	}
	
	inline s16& next_index()
	{
		return index_pair
			[sa_list_node_contents::vec2_index_for_next_index];
	}
	inline s16& prev_index()
	{
		return index_pair
			[sa_list_node_contents::vec2_index_for_prev_index];
	}
	
	
protected:		// functions
	template< typename type_in_vec2 >
	static inline s16 get_next_index_from_vec2
		( const vec2<type_in_vec2>& n_index_pair )
	{
		return static_cast<s16>(n_index_pair
			[sa_list_node_contents::vec2_index_for_next_index]);
	}
	template< typename type_in_vec2 >
	static inline s16 get_prev_index_from_vec2
		( const vec2<type_in_vec2>& n_index_pair )
	{
		return static_cast<s16>(n_index_pair
			[sa_list_node_contents::vec2_index_for_prev_index]);
	}
	
	
} __attribute__((_align4));




// A group of functions to use as function pointers in the sa_list_backend
// class.
template< typename type >
class sa_list_extras
{
protected:		// functions
	static void specific_type_copy( type* a, type* b )
		__attribute__((_text_hot_section))
	{
		*a = *b;
	}
	static void specific_type_move( type* a, type* b )
		__attribute__((_text_hot_section))
	{
		*a = std::move(*b);
	}
	static void specific_type_reset( type* to_reset )
		__attribute__((_text_hot_section))
	{
		*to_reset = type();
	}
	static u32 specific_type_less( type* a, type* b )
		__attribute__((_text_hot_section))
	{
		return ( (*a) < (*b) );
	}
	
	static void* get_sa_list_node_data( sa_list_node<type>* to_get_from )
		__attribute__((_text_hot_section))
	{
		return &to_get_from->data;
	}
	static vec2_s16* get_sa_list_index_pair
		( sa_list_node<type>* to_get_from )
		__attribute__((_text_hot_section))
	{
		return &to_get_from->index_pair;
	}
	static void conv_node_to_contents( sa_list_node_contents* ret,
		sa_list_node<type>* to_convert )
		__attribute__((_text_hot_section))
	{
		ret->data_ptr = &to_convert->data;
		ret->index_pair_ptr = &to_convert->index_pair;
	}
	
	// Instead of having the ENTIRE insertion_sort function be duplicated
	// for EVERY externally_allocated_sa_list instantiation, only duplicate
	// the inner loop.  This seems pretty good to me.
	static void insertion_sort_inner_loop
		( sa_list_node<type>* node_array, s32* ptr_to_index_low )
		__attribute__((_text_hot_section))
	{
		sa_list_node<type>* node_at_j;
		
		s32& index_low = *ptr_to_index_low;
		
		type* data_at_index_low = &(node_array[index_low].data);
		
		// Find the lowest value at or after i.
		for ( s32 j=index_low;
			j!=-1; 
			j=node_at_j->next_index() )
		{
			//node_at_j = &get_node_at(j);
			node_at_j = &(node_array[j]);
			
			//if ( node_at_j->data 
			//	< get_node_at(index_low).data )
			if ( node_at_j->data < *data_at_index_low )
			{
				index_low = j;
				
				data_at_index_low = &(node_at_j->data);
			}
		}
		
	}
	
	
public:		// functions
	// This is the first time I've used an "auto" return type much, but it
	// makes plenty of sense for this case.
	static inline auto get_specific_type_copy_fp()
	{
		return get_generic_void_2arg_fp(&specific_type_copy);
	}
	static inline auto get_specific_type_move_fp()
	{
		return get_generic_void_2arg_fp(&specific_type_move);
	}
	static inline auto get_specific_type_reset_fp()
	{
		return get_generic_void_1arg_fp(&specific_type_reset);
	}
	static inline auto get_specific_type_less_fp()
	{
		return get_generic_u32_2arg_fp(&specific_type_less);
	}
	
	static inline auto get_get_node_data_fp()
	{
		return get_generic_void_ptr_1arg_fp(&get_sa_list_node_data);
	}
	static inline auto get_get_index_pair_fp()
	{
		return get_generic_vec2_s16_ptr_1arg_fp
			(&get_sa_list_index_pair);
	}
	static inline auto get_conv_node_to_contents_fp()
	{
		return get_generic_void_2arg_fp(&conv_node_to_contents);
	}
	
	static inline auto get_insertion_sort_inner_loop_fp()
	{
		return get_generic_void_2arg_fp
			(&insertion_sort_inner_loop);
	}
	
} __attribute__((_align4));


// This class exists because C++ templates CAN generate too much code.  Oh,
// and I can put code in IWRAM this way!
class sa_list_backend
{
protected:		// types
	
protected:		// variables
	u32 size = 0;
	
	s32 front_index = -1;
	
	// node_array as a void pointer.
	void* node_array = NULL;
	
	sa_free_list_backend* the_free_list_backend_ptr = NULL;
	
	
	u32 total_num_nodes = 0;
	
	// sa_list_node stuff
	u32 specific_type_size = 0, 
		whole_node_size = 0;
	
	
	// Function pointers used by various functions
	
	// Pointers to the copying and moving functions (created using the
	// assignment operators of the specific type)
	generic_void_2arg_fp specific_type_copy_fp = NULL, 
		specific_type_move_fp = NULL;
	// A pointer to a function that does node = type()
	generic_void_1arg_fp specific_type_reset_fp = NULL;
	// A pointer to the less function (created by using the less operator
	// of the specific type)
	generic_u32_2arg_fp specific_type_less_fp = NULL;
	
	generic_void_ptr_1arg_fp get_node_data_fp = NULL;
	generic_vec2_s16_ptr_1arg_fp get_index_pair_fp = NULL;
	
	generic_void_2arg_fp conv_node_to_contents_fp = NULL,
		insertion_sort_inner_loop_fp = NULL;
	
	template< typename type > friend class externally_allocated_sa_list;
	
protected:		// functions
	inline sa_list_backend()
	{
	}
	// Dang, I had no idea it'd be this nice to get types automatically
	// this way.
	inline sa_list_backend( void* s_node_array, 
		sa_free_list_backend* s_the_free_list_backend_ptr,
		u32 s_total_num_nodes, u32 s_specific_type_size,
		u32 s_whole_node_size, 
		
		generic_void_2arg_fp s_specific_type_copy_fp, 
		generic_void_2arg_fp s_specific_type_move_fp,
		generic_void_1arg_fp s_specific_type_reset_fp, 
		generic_u32_2arg_fp s_specific_type_less_fp, 
		
		generic_void_ptr_1arg_fp s_get_node_data_fp, 
		generic_vec2_s16_ptr_1arg_fp s_get_index_pair_fp,
		generic_void_2arg_fp s_conv_node_to_contents_fp,
		generic_void_2arg_fp s_insertion_sort_inner_loop_fp )
	{
		init( s_node_array, s_the_free_list_backend_ptr,
			s_total_num_nodes, s_specific_type_size, s_whole_node_size,
			s_specific_type_copy_fp, 
			s_specific_type_move_fp,
			s_specific_type_reset_fp,
			s_specific_type_less_fp,
			
			s_get_node_data_fp, 
			s_get_index_pair_fp,
			s_conv_node_to_contents_fp,
			s_insertion_sort_inner_loop_fp );
	}
	inline sa_list_backend( sa_list_backend& to_copy )
	{
		init(to_copy);
	}
	
	void init( void* n_node_array, 
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
		generic_void_2arg_fp n_insertion_sort_inner_loop_fp );
	
	inline void init( sa_list_backend& to_copy )
	{
		init( to_copy.get_node_array(), 
			to_copy.get_the_free_list_backend_ptr(),
			to_copy.get_total_num_nodes(), 
			to_copy.get_specific_type_size(),
			to_copy.get_whole_node_size(), 
			
			to_copy.get_specific_type_copy_fp(), 
			to_copy.get_specific_type_move_fp(), 
			to_copy.get_specific_type_reset_fp(),
			to_copy.get_specific_type_less_fp(), 
			
			to_copy.get_get_node_data_fp(), 
			to_copy.get_get_index_pair_fp(),
			to_copy.get_conv_node_to_contents_fp(),
			to_copy.get_insertion_sort_inner_loop_fp() );
	}
	
	
	inline s32& get_front_index()
	{
		return front_index;
	}
	inline const s32 get_front_index() const
	{
		return front_index;
	}
	
	inline void* get_node_array()
	{
		return node_array;
	}
	inline const void* get_node_array() const
	{
		return node_array;
	}
	
	inline sa_free_list_backend* get_the_free_list_backend_ptr()
	{
		return the_free_list_backend_ptr;
	}
	inline const sa_free_list_backend* get_the_free_list_backend_ptr()
		const
	{
		return the_free_list_backend_ptr;
	}
	inline sa_free_list_backend& get_the_free_list_backend()
	{
		return *get_the_free_list_backend_ptr();
	}
	inline const sa_free_list_backend& get_the_free_list_backend() const
	{
		return *get_the_free_list_backend_ptr();
	}
	
	inline u32 get_total_num_nodes() const
	{
		return total_num_nodes;
	}
	inline u32 get_specific_type_size() const
	{
		return specific_type_size;
	}
	inline u32 get_whole_node_size() const
	{
		return whole_node_size;
	}
	
	inline generic_void_2arg_fp get_specific_type_copy_fp()
	{
		return specific_type_copy_fp;
	}
	inline generic_void_2arg_fp get_specific_type_move_fp()
	{
		return specific_type_move_fp;
	}
	inline generic_void_1arg_fp get_specific_type_reset_fp()
	{
		return specific_type_reset_fp;
	}
	inline generic_u32_2arg_fp get_specific_type_less_fp()
	{
		return specific_type_less_fp;
	}
	inline generic_void_ptr_1arg_fp get_get_node_data_fp()
	{
		return get_node_data_fp;
	}
	inline generic_vec2_s16_ptr_1arg_fp get_get_index_pair_fp()
	{
		return get_index_pair_fp;
	}
	inline generic_void_2arg_fp get_conv_node_to_contents_fp()
	{
		return conv_node_to_contents_fp;
	}
	inline generic_void_2arg_fp get_insertion_sort_inner_loop_fp()
	{
		return insertion_sort_inner_loop_fp;
	}
	
	
	inline uintptr_t get_node_addr_at( s32 index ) const
	{
		return reinterpret_cast<uintptr_t>(get_node_array())
			+ arr_byte_index_macro( whole_node_size, index );
	}
	
	
	
	// These three functions are intended to be used sparingly.  They make
	// most sense to use when their result is the ONLY part of a node that
	// is relevant, and in that case it is still best to ONLY CALL them
	// ONCE PER NODE.
	inline void* get_node_data_at_index( s32 index )
	{
		return get_node_data_at(get_node_addr_at(index));
	}
	inline vec2_s16* get_index_pair_at_index( s32 index )
	{
		return get_index_pair_at(get_node_addr_at(index));
	}
	inline s16& get_next_index_at_index( s32 index )
	{
		return get_next_index_at(get_node_addr_at(index));
	}
	inline s16& get_prev_index_at_index( s32 index )
	{
		return get_prev_index_at(get_node_addr_at(index));
	}
	
	
	inline void* get_node_data_at( uintptr_t addr )
	{
		return get_get_node_data_fp()(reinterpret_cast<void*>(addr));
	}
	inline vec2_s16* get_index_pair_at( uintptr_t addr )
	{
		return get_get_index_pair_fp()(reinterpret_cast<void*>(addr));
	}
	inline s16& get_next_index_at( uintptr_t addr )
	{
		return ((*get_index_pair_at(addr))
			[sa_list_node_contents::vec2_index_for_next_index]);
	}
	inline s16& get_prev_index_at( uintptr_t addr )
	{
		return ((*get_index_pair_at(addr))
			[sa_list_node_contents::vec2_index_for_prev_index]);
	}
	
	inline sa_list_node_contents get_node_contents_at( s32 index )
	{
		uintptr_t node_addr = get_node_addr_at(index);
		
		//sa_list_node_contents ret = { get_node_data_at(node_addr),
		//	get_index_pair_at(node_addr) };
		
		sa_list_node_contents ret;
		
		get_conv_node_to_contents_fp()( &ret, 
			reinterpret_cast<void*>(node_addr) );
		
		return ret;
	}
	
	
	// Make sure to call this with node DATA!
	inline void copy_node_data( void* dst_node_data, void* src_node_data )
	{
		get_specific_type_copy_fp()( dst_node_data,
			const_cast<void*>(src_node_data) );
	}
	inline void copy_node_data( void* dst_node_data, 
		const void* src_node_data )
	{
		get_specific_type_copy_fp()( dst_node_data, 
			const_cast<void*>(src_node_data) );
	}
	inline void move_node_data( void* dst_node_data, void* src_node_data )
	{
		get_specific_type_move_fp()( dst_node_data,
			const_cast<void*>(src_node_data) );
	}
	inline void move_node_data( void* dst_node_data, 
		const void* src_node_data )
	{
		get_specific_type_move_fp()( dst_node_data,
			const_cast<void*>(src_node_data) );
	}
	
	
	//inline void assign_to_whole_node_via_copy
	//	( sa_list_node_contents& dst_node_contents, const void* n_data,
	//	const vec2_s16& n_index_pair )
	//{
	//	copy_node_data( dst_node_contents.data_ptr, n_data );
	//	copy_vec2_s16_via_ptr( *dst_node_contents.index_pair_ptr,
	//		n_index_pair );
	//}
	//inline void assign_to_whole_node_via_move
	//	( sa_list_node_contents& dst_node_contents,
	//	const void* n_data, const vec2_s16& n_index_pair )
	//{
	//	move_node_data( dst_node_contents.data_ptr, n_data );
	//	copy_vec2_s16_via_ptr( *dst_node_contents.index_pair_ptr,
	//		n_index_pair );
	//}
	inline void assign_to_node_data
		( sa_list_node_contents& dst_node_contents, const void* n_data,
		u32 can_move_value )
	{
		if (!can_move_value)
		{
			copy_node_data( dst_node_contents.data_ptr, 
				n_data );
		}
		else //if (can_move_value)
		{
			move_node_data( dst_node_contents.data_ptr, 
				n_data );
		}
	}
	inline void assign_to_whole_node
		( sa_list_node_contents& dst_node_contents, const void* n_data,
		const vec2_s16& n_index_pair, u32 can_move_value )
	{
		assign_to_node_data( dst_node_contents, n_data, 
			can_move_value );
		copy_vec2_s16_via_ptr( *dst_node_contents.index_pair_ptr,
			n_index_pair );
	}
	
	
	// Make sure to call this with node DATA!
	inline u32 call_specific_type_less_func( void* node_a_data, 
		void* node_b_data )
	{
		return get_specific_type_less_fp()
			( const_cast<void*>(node_a_data), 
			const_cast<void*>(node_b_data) );
	}
	inline u32 call_specific_type_less_func( const void* node_a_data, 
		void* node_b_data )
	{
		return get_specific_type_less_fp()
			( const_cast<void*>(node_a_data), 
			const_cast<void*>(node_b_data) );
	}
	inline u32 call_specific_type_less_func( void* node_a_data, 
		const void* node_b_data )
	{
		return get_specific_type_less_fp()
			( const_cast<void*>(node_a_data), 
			const_cast<void*>(node_b_data) );
	}
	inline u32 call_specific_type_less_func( const void* node_a_data, 
		const void* node_b_data )
	{
		return get_specific_type_less_fp()
			( const_cast<void*>(node_a_data), 
			const_cast<void*>(node_b_data) );
	}
	
	void fully_deallocate() __attribute__((_iwram_code));
	void fully_deallocate_via_unlink() __attribute__((_iwram_code));
	
	
	// This is used by frontends to the move_node* functions
	void internal_func_allocate_and_assign_to_node
		( s32& index, sa_list_node_contents& node,
		const void* n_data, u32 can_move_value );
		//__attribute__((_iwram_code));
	
	inline s32 push_front( const void* to_push, u32 can_move_value=false )
	{
		s32 to_push_index;
		sa_list_node_contents node_to_push;
		
		internal_func_allocate_and_assign_to_node( to_push_index, 
			node_to_push, to_push, can_move_value );
		
		return move_node_to_front( to_push_index, node_to_push );
	}
	
	inline s32 insert_before( s32 index, const void* to_insert,
		u32 can_move_value=false )
	{
		s32 to_insert_index;
		sa_list_node_contents node_to_insert;
		
		internal_func_allocate_and_assign_to_node( to_insert_index,
			node_to_insert, to_insert, can_move_value );
		
		return move_node_before( index, to_insert_index, 
			node_to_insert );
	}
	inline s32 insert_after( s32 index, const void* to_insert,
		u32 can_move_value=false )
	{
		s32 to_insert_index;
		sa_list_node_contents node_to_insert;
		
		internal_func_allocate_and_assign_to_node( to_insert_index,
			node_to_insert, to_insert, can_move_value );
		
		return move_node_after( index, to_insert_index,
			node_to_insert );
	}
	
	//s32 push_front( const void* to_push, u32 can_move_value=false )
	//	__attribute__((_iwram_code,noinline));
	//s32 insert_before( s32 index, const void* to_insert,
	//	u32 can_move_value=false ) __attribute__((_iwram_code,noinline));
	//s32 insert_after( s32 index, const void* to_insert,
	//	u32 can_move_value=false ) __attribute__((_iwram_code,noinline));
	
	
	// Functions for internal use 
	s32 move_node_to_front( s32 to_move_index, 
		sa_list_node_contents& node_to_move );
		//__attribute__((noinline));
	s32 move_node_before( s32 to_move_before_index, 
		s32 to_move_index, sa_list_node_contents& node_to_move );
		//__attribute__((noinline));
	s32 move_node_after( s32 to_move_after_index, 
		s32 to_move_index, sa_list_node_contents& node_to_move );
		//__attribute__((noinline));
	
	
	// Give slightly more flexibility, at the expense of a small amount of
	// speed, to this function by allowing a pointer to the
	// node_at_index be passed to it.
	void* unlink_at_without_dealloc( s32 index, 
		sa_list_node_contents* node_at_index_ptr=NULL ) 
		__attribute__((noinline));
	
	
	// It is (slightly) faster to just unlink a node than it is to erase
	// it because erase_at ALSO resets the data of the node.  Use caution
	// when using this function!
	inline void* unlink_at( s32 index )
	{
		get_the_free_list_backend().push(index);
		
		return unlink_at_without_dealloc(index);
	}
	
	inline void erase_at( s32 index )
	{
		get_specific_type_reset_fp()(unlink_at(index));
	}
	
	
	//s32 insertion_sort() __attribute__((_iwram_code));
	//s32 insertion_sort() __attribute__((_text_hot_section));
	s32 insertion_sort();
	
	//s32 merge_sort() __attribute__((_iwram_code));
	s32 merge_sort();
	
	
} __attribute__((_align4));





template< typename type >
class externally_allocated_sa_list
{
protected:		// typedefs
	typedef sa_list_extras<type> extras_type;
	
//public:		// variables
protected:		// variables
	//s32* ptr_to_front_index;
	//s32 front_index;
	sa_list_node<type>* node_array = 0;
	sa_free_list_backend* the_free_list_backend_ptr = 0;
	u32 total_num_nodes = 0;
	
	sa_list_backend the_sa_list_backend;
	
public:		// functions
	inline externally_allocated_sa_list() : the_sa_list_backend()
	{
	}
	inline externally_allocated_sa_list
		( sa_list_node<type>* s_node_array, 
		sa_free_list_backend* s_the_free_list_backend_ptr,
		u32 s_total_num_nodes )
	{
		init( s_node_array, s_the_free_list_backend_ptr,
			s_total_num_nodes );
	}
	
	inline ~externally_allocated_sa_list()
	{
		fully_deallocate();
	}
	
	
	void init( sa_list_node<type>* n_node_array, 
		sa_free_list_backend* n_the_free_list_backend_ptr,
		u32 n_total_num_nodes ) __attribute__((noinline))
	{
		node_array = n_node_array;
		the_free_list_backend_ptr = n_the_free_list_backend_ptr;
		total_num_nodes = n_total_num_nodes;
		
		the_sa_list_backend.init( get_node_array(),
			the_free_list_backend_ptr, get_total_num_nodes(),
			sizeof(type), sizeof(sa_list_node<type>),
			extras_type::get_specific_type_copy_fp(),
			extras_type::get_specific_type_move_fp(),
			extras_type::get_specific_type_reset_fp(),
			extras_type::get_specific_type_less_fp(), 
			
			extras_type::get_get_node_data_fp(),
			extras_type::get_get_index_pair_fp(),
			extras_type::get_conv_node_to_contents_fp(),
			extras_type::get_insertion_sort_inner_loop_fp() );
		
		//static auto specific_type_copy = []( type* a, type* b ) -> void 
		//	{ *a = *b; };
		//static auto specific_type_move = []( type* a, type* b ) -> void
		//	{ *a = std::move(*b); };
		//static auto specific_type_reset = []( type* a ) -> void
		//	{ *a = type(); };
		//static auto specific_type_less = []( type* a, type* b ) -> u32
		//	{ return (*a) < (*b); };
		//
		//
		//asm_comment("the_sa_list_backend.init()");
		//the_sa_list_backend.init( get_node_array(),
		//	the_free_list_backend_ptr, get_total_num_nodes(),
		//	sizeof(type), sizeof(sa_list_node<type>),
		//	
		//	reinterpret_cast<generic_void_2arg_fp>(&specific_type_copy),
		//	reinterpret_cast<generic_void_2arg_fp>(&specific_type_move),
		//	reinterpret_cast<generic_void_1arg_fp>(&specific_type_reset),
		//	reinterpret_cast<generic_u32_2arg_fp>(&specific_type_less), 
		//	
		//	extras_type::get_get_node_data_fp(),
		//	extras_type::get_get_index_pair_fp(),
		//	extras_type::get_conv_node_to_contents_fp() );
	}
	
	inline s32& get_front_index()
	{
		return the_sa_list_backend.get_front_index();
	}
	inline const s32 get_front_index() const
	{
		return the_sa_list_backend.get_front_index();
	}
	
	inline sa_list_node<type>* get_node_array()
	{
		return node_array;
	}
	inline const sa_list_node<type>* get_node_array() const
	{
		return node_array;
	}
	
	inline sa_free_list_backend& get_the_free_list_backend()
	{
		return *the_free_list_backend_ptr;
	}
	inline const sa_free_list_backend& get_the_free_list_backend() const
	{
		return *the_free_list_backend_ptr;
	}
	
	inline u32 get_total_num_nodes() const
	{
		return total_num_nodes;
	}
	
	
	
	inline sa_list_node<type>& get_node_at( s32 index )
	{
		return get_node_array()[index];
	}
	inline sa_list_node<type>& front()
	{
		return get_node_at(get_front_index());
	}
	
	inline sa_list_node<type>& get_next_node_after_index
		( s32 index )
	{
		return get_node_at(get_node_at(index)
			.next_index());
	}
	inline sa_list_node<type>& get_prev_node_before_index
		( s32 index )
	{
		return get_node_at(get_node_at(index)
			.prev_index());
	}
	
	
	inline void fully_deallocate()
	{
		the_sa_list_backend.fully_deallocate();
	}
	inline void fully_deallocate_via_unlink()
	{
		the_sa_list_backend.fully_deallocate_via_unlink();
	}
	
	//s32 push_front_old( const type& to_push )
	//{
	//	// If there's nothing in the list
	//	if ( get_front_index() == -1 )
	//	{
	//		get_front_index() = get_the_free_list_backend()
	//			.peek_top();
	//		get_the_free_list_backend().pop();
	//		
	//		front().data = to_push;
	//		
	//		// These two operations are for good measure, and they might
	//		// not actually be necessary.
	//		front().next_index() = -1;
	//		front().prev_index() = -1;
	//	}
	//	// If there's at least one element in the list
	//	else
	//	{
	//		s32 old_front_index = get_front_index();
	//		get_front_index() = get_the_free_list_backend()
	//			.peek_top();
	//		get_the_free_list_backend().pop();
	//		
	//		get_node_at(old_front_index).prev_index() 
	//			= get_front_index();
	//		
	//		front().data = to_push;
	//		front().next_index() = old_front_index;
	//		
	//		// This operation is for good measure, and it might not
	//		// actually be necessary.
	//		front().prev_index() = -1;
	//	}
	//	
	//	return get_front_index();
	//}
	
	
	
	//s32 push_front_old_2( const type& to_push ) __attribute__((noinline))
	//{
	//	s32& the_front_index = get_front_index();
	//	s32 old_front_index = the_front_index;
	//	
	//	sa_free_list_backend& the_free_list_backend 
	//		= get_the_free_list_backend();
	//	
	//	the_front_index = the_free_list_backend.peek_top();
	//	the_free_list_backend.pop();
	//	
	//	
	//	//asm_comment("Before get_node_at(the_front_index)");
	//	
	//	sa_list_node<type>& the_front_node 
	//		= get_node_at(the_front_index);
	//	
	//	//asm_comment("After get_node_at(the_front_index)");
	//	
	//	
	//	s32 new_next_index = -1;
	//	
	//	// If there's nothing in the list
	//	//if ( the_front_index < 0 )
	//	//if ( old_front_index < 0 )
	//	//{
	//	//	
	//	//}
	//	//// If there's at least one element in the list
	//	//else
	//	if ( old_front_index >= 0 )
	//	{
	//		new_next_index = old_front_index;
	//		
	//		get_node_at(old_front_index).prev_index()
	//			= the_front_index;
	//	}
	//	
	//	
	//	the_front_node.data = to_push;
	//	the_front_node.next_index() = new_next_index;
	//	the_front_node.prev_index() = -1;
	//	
	//	return the_front_index;
	//}
	
	inline s32 push_front( const type& to_push )
	{
		return the_sa_list_backend.push_front(&to_push);
	}
	inline s32 push_front( type&& to_push )
	{
		return the_sa_list_backend.push_front( &to_push, true );
	}
	
	inline s32 pop_front_basic()
	{
		s32& the_front_index = get_front_index();
		
		erase_at(the_front_index);
		
		return the_front_index;
	}
	
	//s32 insert_before_old( s32 index, const type& to_insert )
	//{
	//	s32 old_prev_index = get_node_at(index)
	//		.prev_index();
	//	//s32 old_next_index = get_node_at(index)
	//	//	.next_index();
	//	
	//	// If index == front_index
	//	//if ( old_prev_index == -1 )
	//	if ( index == get_front_index() )
	//	{
	//		return push_front(to_insert);
	//	}
	//	else
	//	{
	//		//s32 new_index = get_the_free_list_backend().pop();
	//		s32 new_index = get_the_free_list_backend().peek_top();
	//		get_the_free_list_backend().pop();
	//		
	//		get_node_at(old_prev_index).next_index()
	//			= new_index;
	//		
	//		get_node_at(new_index).data = to_insert;
	//		get_node_at(new_index).prev_index() 
	//			= old_prev_index;
	//		get_node_at(new_index).next_index()
	//			= index;
	//		
	//		get_node_at(index).prev_index() 
	//			= new_index;
	//		
	//		return new_index;
	//	}
	//	
	//	//return index;
	//	//return new_index;
	//}
	
	//s32 insert_before_old_2( s32 index, const type& to_insert )
	//	__attribute__((noinline))
	//{
	//	//s32 old_prev_index = get_node_at(index)
	//	//	.prev_index();
	//	////s32 old_next_index = get_node_at(index)
	//	////	.next_index()
	//	
	//	// If index == front_index
	//	//if ( old_prev_index == -1 )
	//	if ( index == get_front_index() )
	//	{
	//		return push_front(to_insert);
	//	}
	//	else
	//	{
	//		sa_list_node<type>& node_at_index 
	//			= get_node_at(index);
	//		
	//		const s32 old_prev_index 
	//			= node_at_index.prev_index();
	//		
	//		const s32 new_index = get_the_free_list_backend()
	//			.peek_top();
	//		get_the_free_list_backend().pop();
	//		
	//		
	//		sa_list_node<type>& node_at_new_index
	//			= get_node_at(new_index);
	//		
	//		
	//		get_node_at(old_prev_index).next_index()
	//			= new_index;
	//		
	//		node_at_new_index.data = to_insert;
	//		node_at_new_index.prev_index() = old_prev_index;
	//		node_at_new_index.next_index() = index;
	//		
	//		node_at_index.prev_index() = new_index;
	//		
	//		return new_index;
	//	}
	//	
	//	//return index;
	//	//return new_index;
	//}
	inline s32 insert_before( s32 index, const type& to_insert )
	{
		return the_sa_list_backend.insert_before( index, &to_insert );
	}
	inline s32 insert_before( s32 index, type&& to_insert )
	{
		return the_sa_list_backend.insert_before( index, &to_insert,
			true );
	}
	
	
	//s32 insert_after_old( s32 index, const type& to_insert )
	//{
	//	//s32 old_prev_index = get_node_at(index)
	//	//	.prev_index();
	//	s32 old_next_index = get_node_at(index)
	//		.next_index();
	//	
	//	//s32 new_index = get_the_free_list_backend().pop();
	//	s32 new_index = get_the_free_list_backend().peek_top();
	//	get_the_free_list_backend().pop();
	//	
	//	// Special code is used for inserting an element at the end of the
	//	// list.
	//	if ( old_next_index == -1 )
	//	{
	//		get_node_at(index).next_index() = new_index;
	//		
	//		get_node_at(new_index).data = to_insert;
	//		get_node_at(new_index).next_index() = -1;
	//		get_node_at(new_index).prev_index() = index;
	//	}
	//	else
	//	{
	//		get_node_at(old_next_index).prev_index()
	//			= new_index;
	//		
	//		get_node_at(index).next_index() = new_index;
	//		
	//		get_node_at(new_index).data = to_insert;
	//		get_node_at(new_index).next_index() 
	//			= old_next_index;
	//		get_node_at(new_index).prev_index() = index;
	//	}
	//	
	//	//return index;
	//	return new_index;
	//}
	
	//s32 insert_after_old_2( s32 index, const type& to_insert )
	//	__attribute__((noinline))
	//{
	//	////s32 old_prev_index = get_node_at(index)
	//	////	.prev_index();
	//	//s32 old_next_index = get_node_at(index)
	//	//	.next_index();
	//	sa_list_node<type>& node_at_index = get_node_at(index);
	//	const s32 old_next_index = node_at_index
	//		.next_index();
	//	
	//	//s32 new_index = get_the_free_list_backend().pop();
	//	const s32 new_index = get_the_free_list_backend().peek_top();
	//	get_the_free_list_backend().pop();
	//	
	//	
	//	node_at_index.next_index() = new_index;
	//	
	//	sa_list_node<type>& node_at_new_index 
	//		= get_node_at(new_index);
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
	//		get_node_at(old_next_index).prev_index() 
	//			= new_index;
	//		new_next_index = old_next_index;
	//	}
	//	
	//	node_at_new_index.data = to_insert;
	//	node_at_new_index.next_index() = new_next_index;
	//	node_at_new_index.prev_index() = index;
	//	
	//	
	//	//return index;
	//	return new_index;
	//}
	inline s32 insert_after( s32 index, const type& to_insert )
	{
		return the_sa_list_backend.insert_after( index, &to_insert );
	}
	inline s32 insert_after( s32 index, type&& to_insert )
	{
		return the_sa_list_backend.insert_after( index, &to_insert,
			true );
	}
	
	//void erase_at_old( s32 index )
	//{
	//	s32 old_prev_index = get_node_at(index)
	//		.prev_index(),
	//		old_next_index = get_node_at(index)
	//		.next_index();
	//	
	//	// Special code is used for erasing an element at the beginning of
	//	// the list.
	//	//if ( old_prev_index == -1 )
	//	if ( index == get_front_index() )
	//	{
	//		//s32 old_front_index = get_front_index();
	//		
	//		get_node_at(get_front_index()).data = type();
	//		get_node_at(get_front_index()).next_index() = -1;
	//		get_node_at(get_front_index()).prev_index() = -1;
	//		
	//		get_the_free_list_backend().push(get_front_index());
	//		
	//		get_front_index() = old_next_index;
	//		
	//		if ( old_next_index != -1 )
	//		{
	//			front().prev_index() = -1;
	//		}
	//	}
	//	else 
	//	{
	//		get_node_at(index).data = type();
	//		get_node_at(index).next_index() = -1;
	//		get_node_at(index).prev_index() = -1;
	//		get_the_free_list_backend().push(index);
	//		
	//		if ( old_next_index != -1 )
	//		{
	//			get_node_at(old_prev_index).next_index()
	//				= old_next_index;
	//			get_node_at(old_next_index).prev_index()
	//				= old_prev_index;
	//		}
	//		else
	//		{
	//			get_node_at(old_prev_index).next_index()
	//				= -1;
	//		}
	//	}
	//	
	//}
	
	//void erase_at_old_2( s32 index ) __attribute__((noinline))
	//{
	//	//s32 old_prev_index = get_node_at(index)
	//	//	.prev_index(),
	//	//	old_next_index = get_node_at(index)
	//	//	.next_index();
	//	
	//	sa_list_node<type>& node_at_index = get_node_at(index);
	//	
	//	const s32 old_prev_index = node_at_index
	//		.prev_index(),
	//		old_next_index = node_at_index.next_index();
	//	
	//	s32& the_front_index = get_front_index();
	//	const s32 old_front_index = the_front_index;
	//	
	//	//node_at_index.data = type();
	//	//node_at_index.next_index() = -1;
	//	//node_at_index.prev_index() = -1;
	//	node_at_index = sa_list_node<type>();
	//	
	//	get_the_free_list_backend().push(index);
	//	
	//	if ( index == old_front_index )
	//	{
	//		the_front_index = old_next_index;
	//		
	//		if ( old_next_index >= 0 )
	//		{
	//			//front().prev_index() = -1;
	//			get_node_at(the_front_index).prev_index() = -1;
	//		}
	//	}
	//	else
	//	{
	//		sa_list_node<type>& old_prev_node 
	//			= get_node_at(old_prev_index);
	//		
	//		if ( old_next_index >= 0 )
	//		{
	//			old_prev_node.next_index() = old_next_index;
	//			get_node_at(old_next_index).prev_index()
	//				= old_prev_index;
	//		}
	//		else
	//		{
	//			old_prev_node.next_index() = -1;
	//		}
	//	}
	//}
	inline void erase_at( s32 index )
	{
		the_sa_list_backend.erase_at(index);
	}
	inline type&& unlink_at( s32 index )
	{
		return std::move(*reinterpret_cast<type*>
			(the_sa_list_backend.unlink_at(index)));
	}
	
	
	//s32 insertion_sort_old()
	//{
	//	// Don't do anything if this list has zero or one nodes.
	//	if ( get_front_index() == -1 )
	//	{
	//		return get_front_index();
	//	}
	//	if ( front().next_index() == -1 )
	//	{
	//		return get_front_index();
	//	}
	//	
	//	////externally_allocated_sa_list< type, total_num_nodes >
	//	////	sorted_list( ptr_to_node_array, ptr_to_the_free_list );
	//	//s32 temp_front_index = -1;
	//	//sa_list_backend<type> sorted_list( &temp_front_index, 
	//	//	node_array, the_free_list_backend_ptr, 
	//	//	total_num_nodes );
	//	externally_allocated_sa_list<type> sorted_list( node_array, 
	//		the_free_list_backend_ptr, get_total_num_nodes() );
	//	
	//	s32 curr_index = sorted_list.get_front_index();
	//	
	//	for ( s32 i=get_front_index();
	//		i!=-1;  )
	//		//i=get_node_at(i).next_index() )
	//	{
	//		
	//		//get_the_free_list_backend().debug_prs32();
	//		
	//		s32 index_of_node_with_lowest_value = i;
	//		
	//		// Find the lowest
	//		for ( s32 j=index_of_node_with_lowest_value;
	//			j!=-1;
	//			j=get_node_at(j).next_index() )
	//		{
	//			if ( get_node_at(j).data 
	//				< get_node_at(index_of_node_with_lowest_value)
	//				.data )
	//			{
	//				index_of_node_with_lowest_value = j;
	//			}
	//		}
	//		
	//		type data_to_push = get_node_at
	//			(index_of_node_with_lowest_value).data;
	//		
	//		if ( i == index_of_node_with_lowest_value )
	//		{
	//			i = get_node_at(i).next_index();
	//		}
	//		
	//		erase_at(index_of_node_with_lowest_value);
	//		
	//		if ( sorted_list.get_front_index() == -1 )
	//		{
	//			sorted_list.push_front(data_to_push);
	//			curr_index = sorted_list.get_front_index();
	//		}
	//		else
	//		{
	//			sorted_list.insert_after( curr_index, data_to_push );
	//			curr_index = sorted_list.get_node_at
	//				(curr_index).next_index();
	//		}
	//		
	//	}
	//	
	//	
	//	get_front_index() = sorted_list.get_front_index();
	//	
	//	sorted_list.get_front_index() = -1;
	//	
	//	return get_front_index();
	//}
	
	
	s32 insertion_sort_old_2() 
		__attribute__((_text_hot_section,noinline))
		//__attribute__((noinline))
	{
		s32& the_front_index = get_front_index();
		
		// Don't do anything if this list has zero or one nodes.
		if ( the_front_index < 0 )
		{
			return the_front_index;
		}
		if ( get_node_at(the_front_index).next_index() < 0 )
		{
			return the_front_index;
		}
		
		//s32 temp_front_index = -1;
		//sa_list_backend<type> sorted_list( &temp_front_index, 
		//	node_array, the_free_list_backend_ptr, 
		//	total_num_nodes );
		externally_allocated_sa_list<type> sorted_list( node_array, 
			the_free_list_backend_ptr, get_total_num_nodes() );
		
		s32& temp_front_index = sorted_list.get_front_index();
		
		s32 curr_index = temp_front_index;
		
		for ( s32 i=the_front_index;
			i!=-1;  )
			//i=get_node_at(i).next_index() )
		{
			
			//get_the_free_list_backend().debug_prs32();
			
			s32 index_low = i;
			
			sa_list_node<type>* node_at_j;
			
			// Find the lowest value at or after i.
			for ( s32 j=index_low;
				j!=-1; 
				j=node_at_j->next_index() )
			{
				node_at_j = &get_node_at(j);
				
				if ( node_at_j->data 
					< get_node_at(index_low).data )
				{
					index_low = j;
				}
			}
			
			sa_list_node<type>& node_at_index_low = get_node_at(index_low);
			//const type data_to_move = node_at_index_low.data;
			//type* data_to_move = &node_at_index_low.data;
			
			if ( i == index_low )
			{
				i = node_at_index_low.next_index();
			}
			
			//erase_at(index_low);
			type&& data_to_move = unlink_at(index_low);
			
			if ( temp_front_index < 0 )
			{
				sorted_list.push_front(std::move(data_to_move));
				curr_index = temp_front_index;
			}
			else
			{
				sorted_list.insert_after( curr_index, 
					std::move(data_to_move) );
				curr_index = sorted_list.get_node_at(curr_index)
					.next_index();
			}
			
		}
		
		
		the_front_index = temp_front_index;
		
		temp_front_index = -1;
		
		
		return the_front_index;
	}
	
	
	inline s32 insertion_sort()
	{
		return the_sa_list_backend.insertion_sort();
	}
	
	
	inline s32 merge_sort()
	{
		return the_sa_list_backend.merge_sort();
	}
	
	
} __attribute__((_align4));



// This is a template class for a statically allocated array of modifiable
// doubly-linked lists.  "sa" is short for "statically allocated".
template< typename type, u32 total_num_nodes, u32 num_lists >
class sa_array_of_lists
{
public:		// variables
	//array< externally_allocated_sa_list< type, total_num_nodes >, 
	//	num_lists > the_array;
	//array< sa_list_node<type>, total_num_nodes > node_array;
	
	externally_allocated_sa_list<type> easa_list_array[num_lists];
	sa_list_node<type> node_array[total_num_nodes];
	
	sa_free_list<total_num_nodes> the_free_list;
	
public:		// functions
	
	inline sa_array_of_lists()
	{
		//for ( auto& the_list : easa_list_array )
		//{
		//	the_list.set_node_array(node_array.data());
		//	the_list.set_ptr_to_the_free_list(&the_free_list);
		//	the_list.init_the_sa_list_backend();
		//}
		
		//for ( s32 i=(s32)get_node_array_size()-1; i>=0; --i )
		//{
		//	node_array[i] = sa_list_node<type>();
		//}
		
		for ( s32 i=(s32)get_the_array_size()-1; i>=0; --i )
		{
			//the_array[i].set_node_array(node_array);
			//the_array[i].set_ptr_to_the_free_list(&the_free_list);
			//the_array[i].init_the_sa_list_backend();
			easa_list_array[i].init( node_array,
				&the_free_list.the_sa_free_list_backend, 
				get_total_num_nodes() );
		}
		
		
	}
	
	constexpr inline u32 get_the_array_size() const
	{
		return get_num_lists();
	}
	constexpr inline u32 get_node_array_size() const
	{
		return get_total_num_nodes();
	}
	
	constexpr inline u32 get_total_num_nodes() const
	{
		return total_num_nodes;
	}
	
	constexpr inline u32 get_num_lists() const
	{
		return num_lists;
	}
	
} __attribute__((_align4));



#endif		// sa_list_class_stuff_hpp
