/**
 * kernel/physmm.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 29-03-2014 - Created
 */

#include "kernel/physmm.h"
#include <assert.h>

uint32_t physmm_bitmap[PHYSMM_BITMAP_SIZE];

void physmm_set_bit(physaddr_t address) 
{
	address >>= 12;
	physmm_bitmap[address >> 5] |= 1 << (address & 0x1F);
}

void physmm_clear_bit(physaddr_t address) 
{
	address >>= 12;
	physmm_bitmap[address >> 5] &= ~(1 << (address & 0x1F));
}

void physmm_free_range(physaddr_t start, physaddr_t end) 
{
	physaddr_t counter;
	assert(start < end);
	for (counter = start; counter < end; counter += PHYSMM_PAGE_SIZE){
		physmm_free_frame(counter);
	}
}

void physmm_claim_range(physaddr_t start, physaddr_t end) 
{
	physaddr_t counter;
	assert(start < end);
	for (counter = start; counter < end; counter += PHYSMM_PAGE_SIZE){
		physmm_clear_bit(counter);
	}
}

physaddr_t physmm_alloc_frame() 
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter++) {
			if (physmm_bitmap[counter] & (1 << bit_counter)) {
				counter <<= 5;
				counter |= bit_counter;
				counter <<= 12;
				physmm_clear_bit(counter);
				//TODO: Release physmm_bitmap
				return counter;
			}
		}
	}
	//TODO: Release physmm_bitmap
	return PHYSMM_NO_FRAME;
}

physaddr_t physmm_alloc_quadframe() 
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter+=4) {
			if (physmm_bitmap[counter] & (0xF << bit_counter)) {
				counter <<= 5;
				counter |= bit_counter;
				counter <<= 12;
				physmm_clear_bit(counter+PHYSMM_PAGE_SIZE*0);
				physmm_clear_bit(counter+PHYSMM_PAGE_SIZE*1);
				physmm_clear_bit(counter+PHYSMM_PAGE_SIZE*2);
				physmm_clear_bit(counter+PHYSMM_PAGE_SIZE*3);
				//TODO: Release physmm_bitmap
				return counter;
			}
		}
	}
	//TODO: Release physmm_bitmap
	return PHYSMM_NO_FRAME;
}

physaddr_t physmm_count_free() 
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter, result;
	result = 0;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter++) {
			if (physmm_bitmap[counter] & (1 << bit_counter))
				result += PHYSMM_PAGE_SIZE;
		}
	}
	//TODO: Release physmm_bitmap
	return result;
}

void physmm_free_frame(physaddr_t address)
{
	physmm_set_bit(address);
}

void physmm_init(){
}
