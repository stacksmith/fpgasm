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
#include "cMultiWire.h"

//sWireEnd dump
void sWireEnd::dump(FILE*f){
  switch(inst){
    case INST_STOP:
      fprintf(f,"-----\n");
      break;
    case INST_END:
      fprintf(f,"-fini-\n");
      break;
    default:
      fprintf(f,"-%d-%d-%d-\n",inst,pindex,busid);
      break;
  }
}


#define CLASS cMultiWire  
CLASS::CLASS(const cMultiWire& orig){
  buf=((cMultiWire&)orig).getBuf();
}

CLASS::CLASS(){
  buf=0;
}

CLASS::~CLASS(){
  free(buf);
}

#include "cMultiWireWalker.h"
void CLASS::dump(FILE*f){
  cMultiWireWalker* walker = new cMultiWireWalker(*this);
  while(!walker->isEnd()){
    while(!walker->isStop()){
fprintf(f," %p ",this);
      walker->getInc().dump(f);
    }
fprintf(f," %p ",this);
    walker->getInc().dump(f); //dump the stop
  }
fprintf(f," %p ",this);
  walker->getInc().dump(f); //dump the end, parse too far but who cares...
fprintf(f,"DONE\n");
  
}