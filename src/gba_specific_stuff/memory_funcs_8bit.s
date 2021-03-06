@ This file is part of GBA Project Template.
@ 
@ Copyright 2015-2017 by Andrew Clark (FL4SHK).
@ 
@ GBA Project Template is free software: you can redistribute it and/or
@ modify it under the terms of the GNU General Public License as published
@ by the Free Software Foundation, either version 3 of the License, or (at
@ your option) any later version.
@ 
@ GBA Project Template is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
@ General Public License for more details.
@ 
@ You should have received a copy of the GNU General Public License along
@ with GBA Project Template.  If not, see <http://www.gnu.org/licenses/>.


.include "include/all_includes.s"

@ void memcpy8 ( void* dst, const void* src, uint bytecount ) 
@	__attribute__ ((_iwram_code));
@ 
@ r0, r1:  dst, src
@ r2:  bytecount
@ r3:  temporary storage


.section .iwram_code,"ax", %progbits
.align 4
.do_arm
.global memcpy8
.type memcpy8, %function

memcpy8:
	.fnstart
	
	ldrb r3, [r1], #1
	strb r3, [r0], #1
	subs r2, r2, #1
	bne memcpy8
	
	bx lr
	
	.fnend






@ void memfill8 ( void* dst, u32 src, uint bytecount ) 
@	__attribute__ ((_iwram_code));
@ 
@ r0, r1:  dst, src
@ r2:  bytecount

.section .iwram_code,"ax", %progbits
.align 4
.do_arm
.global memfill8
.type memfill8, %function

memfill8:
	.fnstart
	
	strb r1, [r0], #1
	subs r2, r2, #1
	bne memfill8
	
	bx lr
	.fnend




