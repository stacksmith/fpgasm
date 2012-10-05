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
#include "cMultiWireWalker.h"
#define CLASS cMultiWireWalker

CLASS::CLASS(cMultiWire& it){
  buf=it.getBuf();
  index=0;
}
CLASS::~CLASS(){
  //do not free - we don't own the wire, just walk it
  
}


void CLASS::getInc(int& inst,int& pindex,int& busid){
  inst =   buf[index].inst;
  pindex = buf[index].pindex;
  busid =  buf[index].busid;
  index++;
}
sWireEnd& CLASS::getInc(){
  return buf[index];
}
bool CLASS::isStop(void){
  return(INST_STOP==buf[index].inst);
}
bool CLASS::isEnd(void){
  return(INST_END==buf[index].inst);
}
// Seek next wire.  If end, leave at end.
bool CLASS::seekNext(){
  while(true){
    switch(buf[index].inst){
      case INST_STOP:
        index++;
        return true;
      case INST_END:
        return false;
      default:
        index++;
        break;
    }
  }
}
//seek a particular instance
bool CLASS::seekInst(int inst){
  if(isEnd())
    return false;   //at end!
  //starting at outgoing instance pointer...
  while(seekNext()){
    if(inst==buf[index].inst)
      return true;
  }
  return false;
}