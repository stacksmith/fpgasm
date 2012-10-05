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
#include "cWires.h"
#include "cModule.h"
#include "cSub.h"
#define CLASS cWires


CLASS::CLASS(cModule*mod){
  index=0;                 
  max = 16384;// CWIRES_CAP_INIT;   //starting point
  buf=(U8*)malloc(max);  //room .
#ifdef DEBUG
  bugModule=mod;
#endif
  
}

CLASS::~CLASS(){
  free(buf);
}

void CLASS::solidify(){
  add(0xFE,0xFE,0xFE);
//  printf("SOLIDIFYING to %ld bytes\n",size);
  buf=(U8*)realloc(buf,(index));
  max=index;
fprintf(stderr,"cWires: final wire space is %d bytes, %d wires\n",index,index/3);
  
}

void CLASS::add(int inst,int port,int busid){
  //is there room for 3 more?
  if((index+3)>=max){
    // reallocate, double size
fprintf(stderr,"cWires: reallocating from %d to %d\n",max,max*2);
    max = max*2;
    buf = (U8*)realloc(buf,max);
  }
  buf[index++]=inst;
  buf[index++]=port;
  buf[index++]=busid;
}


void CLASS::close(){
  buf[index++]  =0xFE;
}
  
/*------------------------------------------------------------------
  Encapsulate at least a little bit.
*------------------------------------------------------------------*/
cWireList CLASS::seekFirst(){
  if(0xFE==*buf)
    return cWireList(0);     //return a null wirelist
  return buf;
}


void CLASS::dumpEndpoint(FILE*f,int inst,int port){
  cProto* proto;
   if(inst==0xFF){
    fprintf(f,"my ");
    proto=bugModule;
  } else {
    proto=bugModule->psubs->data[inst]->valSub->type;
    fprintf(f,"%s's ",bugModule->psubs->name[inst]);
  }
  if(port==0xFF){
    fprintf(f,"vcc ");
  }else {
    fprintf(f,"%s ",proto->pins->name[port]);
  }
}
void CLASS::dump(FILE*f){
  fputs("wire ",f);
  U8*p=buf;
  U8 inst,port;
  inst=*p++;
  port=*p++;
  dumpEndpoint(f,inst,port); //first one may be ffff
  while(true) {
    inst=*p++;
    if(inst==0xFE) break;
    port=*p++;
    fputs("to ",f);
    dumpEndpoint(f,inst,port);
  }
  fputs(";\n",f);
 }
  