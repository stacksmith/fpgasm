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
#define CLASS cProto
#include "stdio.h"
#include "cModule.h"
CLASS::CLASS(char* nam,int size)
{
  name=(char*)malloc(size+1);
  strncpy(name,nam,size);
  name[size]=0;
  paramnames = new cCollection();
  pins = new cCollection();
#ifdef DEBUG
  paramnames->setdebugname(nam,size);
#endif
  psubs=0;
  pwires=0;
  converted=false; //for the verilog converter
 // pins=new cCollection();
}
CLASS::~CLASS(){
  free(name);
}

void CLASS::dump(FILE*f){
  fprintf(f,"\nModule/prim %s:\n",name);

  //since pins are typed (see cCollection/cDatum)
  pins->dump(f,"pins:\n");
  paramnames->dump(f,"paramnames:\n");
 
}
