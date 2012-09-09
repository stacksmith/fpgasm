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


 U8* CLASS::ptr;
CLASS::CLASS(cModule*mod){
  buf=(U8*)malloc(4096); //room .
  ptr=buf;              //valid until solidify
#ifdef DEBUG
  bugModule=mod;
#endif
  
}

CLASS::~CLASS(){
  free(buf);
}

void CLASS::solidify(){
  *ptr++=0xFE;
  long int size=(ptr-buf);
  if(1==size){
    size+=1;
    *ptr=0xFE;
  }
//  printf("SOLIDIFYING to %ld bytes\n",size);
  buf=(U8*)realloc(buf,(ptr-buf));
  
}

void CLASS::add(int inst,int port,int busid){
  *ptr++=inst;
  *ptr++=port;
  *ptr++=busid;
}


void CLASS::close(){
  *ptr++=0xFE;
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
  