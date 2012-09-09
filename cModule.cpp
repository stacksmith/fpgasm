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
#include "cModule.h"
#include "cDevice.h"
#include "cDyn.h"
#define CLASS cModule

extern cDevice* pDevice;

CLASS::CLASS(char* name,int size) 
: cProto(name,size)
{
//printf("MODULE %s %d\n",name,size);
  psubs=new cCollection();
  pDevice->addProto(name,size,this); //will check for dups
}
CLASS::~CLASS(){
//  g_hash_table_destroy(subs);
}


cSub* CLASS::getSub(char*name){
  int i = psubs->find(name);
  cDatum* r = psubs->getDatum(i);
  if(TYPE_SUB!=r->type)
    return r->valSub;
  return 0;
}

/******************************************************************************
 * D U M P
*******************************************************************************/ 
void CLASS::dump(FILE*f){
  ((cProto*)this)->dump(f);
//  fprintf(f,"%d parameter names:\n",paramnames.size);
//  int i;
//  for(i=0;i<paramnames.size;i++){
//    fprintf(f," %d. '%s'\n",i,paramnames.getName(i));
//  }
  psubs->dump(f,"subs:\n");
}
