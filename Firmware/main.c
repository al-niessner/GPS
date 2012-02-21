/**********************************************************************
 * Copyright (C) 2012 Al Niessner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 *====================================================================
 *
 * Module Description:
 *
 *********************************************************************/

#include "contexts.h"
#include "memory.h"

/** VECTOR REMAPPING *******************************************/
#define REMAPPED_RESET_VECTOR_ADDRESS 0x800
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS 0x808
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0x818

#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
extern void _startup( void );        // See c018i.c in your C18 compiler dir
void _reset( void )
{
    _asm goto _startup _endasm
}

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR( void )
{
    _asm goto ctxt_hpi _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR( void )
{
    _asm goto ctxt_lpi _endasm
}



#pragma code

void main( void )
{
  ctxt_initialize();
  while (true)
    {
      ctxt_nominal();
    }
}
