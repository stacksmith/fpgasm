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
#include "cSub.h"
#include "cProto.h"
#define CLASS cSub
/******************************************************************************
                                                                               
******************************************************************************/ 
CLASS::CLASS(char*nam,int len){
  name=(char*)malloc(len+1);
  strncpy(name,nam,len);
  name[len]=0;
  //cfg = g_hash_table_new(g_str_hash,g_str_equal);
}
CLASS::~CLASS(){
  free(name);
  //g_hash_table_destroy(cfg);
  //g_hash_table_destroy(var);
}
/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::init(cProto* theType){
  type=theType;
  //Now initialize our pins to be proto's
  pins=type->pins;
}
/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::setLocation(cDatum* l){
  pparams->add("loc",3,l);
}

cDatum* CLASS::getLocation(){
  if(pparams){
    int iloc = pparams->find("loc",3);
    if(-1==iloc) return 0;               //0 means no set location
    return pparams->data[iloc];
  } else {
    return 0; //no parameters at all
  }
}
/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::dump(FILE*f){
  fprintf(f,"sub '%s' ",name);
  if(pparams==0)
    fprintf(stderr,"NULL params\n");
  else
    pparams->dump(f,"params:\n");
}


