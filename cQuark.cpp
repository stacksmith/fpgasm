/******************************************************************************
 Copyright 2012 Victor Yurkovsky

    This file is part of FPGAsm

    FPGAsm is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FPGAsm is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FPGAsm.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "global.h"
#include "cQuark.h"
#define CLASS cQuark

CLASS::CLASS(char* name,int size) 
: cProto(name,size)
{
//printf("MODULE %s %d\n",name,size);
//  pDevice->addProto(name,size,this); //will check for dups
}
CLASS::~CLASS(){
//  g_hash_table_destroy(subs);
}



/******************************************************************************
 * Verilog
 * Output this module as a verilog module.
*******************************************************************************/ 
void CLASS::verilog(FILE*fout){
  fputs("QUARK verilog not implemented\n",fout);
}