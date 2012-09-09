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
#include "cWireList.h"
#define CLASS cWireList

CLASS::CLASS(U8*p){
  wp=p;
}

bool CLASS::exists(void){
  return (wp!=0);
}

void CLASS::getInc(int& inst,int& index,int& busid){
  if(wp){
    inst=*wp++; index=*wp++; busid=*wp++;
    return;
  }
  fprintf(stderr,"seekNext NULL\n");
  throw(1);
}

bool CLASS::isLast(void){
 return(0xFE==*wp);
}

void CLASS::seekNext(){
  if(wp){
    while(0xFE != *wp++){
      wp++; //skip index
      wp++; //skip busid
    }
    if(0xFE==*wp){ 
      wp=0;
    }
    return;
  }
  fprintf(stderr,"seekNext NULL\n");
  throw(1);
}

void CLASS::seekInst(int inst){
   //starting at outgoing instance pointer...
  while(true){
    if(!wp){
      fprintf(stderr,"seekInst NULL\n");
      throw(1);
    }
    U8 c = *wp++;
    if(c==0xFE) {wp=0;return;}
    if(inst==c) return; //return pointing at pindex
    seekNext();
  }
}