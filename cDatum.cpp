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
#include "cDatum.h"

#define CLASS cDatum

char *CLASS::buf=(char*)malloc(32); //- SLICE_X...Y...\0

CLASS::CLASS(eDatumType typ){
  type=typ;
}
CLASS::~CLASS(){
    switch(type){
      case TYPE_UNREALIZED:
      case TYPE_STR: 
        free(valStr); break;
      default:
      //TODO: comprehensive destructor
        break;
    }
}
cDatum* CLASS::newStr(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_STR);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS::newInt(int val){
  cDatum* ret = new cDatum(TYPE_INT);
 
  ret->valInt=val;
  return ret;
}
cDatum* CLASS::newPin(int dir,int bwidth){
  cDatum* ret = new cDatum(TYPE_PIN);
  ret->pinDir=dir&1;
  ret->pinBusWidth=bwidth&0xFF;
  return ret;
}
cDatum* CLASS::newSub(cSub* val){
  cDatum* ret = new cDatum(TYPE_SUB);
 
  ret->valSub=val;
  return ret;
}
cDatum* CLASS::newLocXY(int x,int y){
  cDatum* ret = new cDatum(TYPE_LOCXY);
 
  ret->valX=x;
  ret->valY=y;
  return ret;
}
cDatum* CLASS::newLocAbs(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_LOCABS);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}  
cDatum* CLASS::newGoodCfgs(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_GOODCFGS);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
} 
cDatum* CLASS::newProto(cProto* val){
  //proto reference
  cDatum* ret = new cDatum(TYPE_PROTO);
 
  ret->valProto=val;
  return ret;
}

cDatum* CLASS::newTile(const char*str,int len){
  cDatum* ret = new cDatum(TYPE_TILE);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS::newParSub(const char*str,int len){
  cDatum* ret = new cDatum(TYPE_PARSUB);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS::newUnrealized(const char* str,int len){
  cDatum* ret=CLASS::newStr(str,len);
  ret->type=TYPE_UNREALIZED;
  return ret;
  
}
/******************************************************************************
 parseLiteral
                                                                                
******************************************************************************/ 
char* CLASS::parseLiteral(char*start,S32*presult){
  char*ptr=start;
  int radix=10;
  long accum=0;
  bool neg=false;
  char c=*ptr;      //check the initial character...
  switch(c){
    case '$':
      radix=16;
      ptr++;
      break;
    case '%':
      radix=2;
      ptr++;
      break;
    case '-':
      neg=true;
      ptr++;
      break;
    default:
      radix=10;
      break;
  }
  while(true){
    c=*ptr++;
    if('_'==c) continue;
    c-='0';
    if(c<0)  break;
    if(c>9) {
      c-=7;
      if(c<0) break;
    }
    if(c>radix) {
      fprintf(stderr,"invalid digit in '%s'",start);
      throw(1);
    }
    accum = (accum*radix)+c;
  }
  ptr--;
  if(neg) accum = 0-accum;
  *presult=accum;
  return ptr;
}


/*=====================================================================
 Convert an unrealized datum to a realized type
======================================================================*/
void CLASS::realize(){
  if(type==TYPE_UNREALIZED){
    //let's check for xy()
    if(0==strncmp("xy(",valStr,3)){
      int x,y;
      char*p=parseLiteral(valStr+3,&x);
//TODO: check for , and ) properly.
//fprintf(stderr,"cDatum::realize() after x,'%s'\n",p);
      p=parseLiteral(p+1,&y);
//fprintf(stderr,"cDatum::realize() after y,'%s'\n",p);
      free(valStr);
      type=TYPE_LOCXY;
      valX=x;
      valY=y;
      return;
    } 
    if(0==strncmp("cfg:",valStr,4)){
      //cfg contains many name:val pairs...
      char* p = strtok(valStr," \n");
      int maxcfgs=256;
      cCollection* cfgs = new cCollection(maxcfgs);
int counter=0;
      while(true){
        p=strtok(NULL," :\n");
        if(!p) break;
counter++;
if(counter>=maxcfgs){
  fprintf(stderr,"Error: Maximum of %d cfgs allowed; exceeded\n",maxcfgs);
  throw(-1);
}
        char* name=p;
        //An empty cfg has a " " here...  strtok(NULL," " will just eat the 
        //next name, so check explicitly.  TODO: this sucks, and should 
        //be replaced by a stream parsing class...
        char* val=(char*)""; //for empty cfg
        if(' '!=*(name+strlen(name)+1)) {
//fprintf(stderr,"xxx[%s]\n",name+strlen(name)+1);
          p=strtok(NULL," \n");
          if(!p) {
            fprintf(stderr,"Error realizing %d-long cfg string [%s] %d\n",(int)strlen(name),name,*name);
            throw(1);
          }
          val=p;
        }
//fprintf(stderr,"REALIZED #%d [%s:%s]\n",counter,name,val);
        cfgs->add(name,strlen(name),cDatum::newStr(val,strlen(val)));
//fprintf(stderr,"[[%s:%s]]\n",name,val);
      }
      cfgs->solidify();      
      delete valStr;
      valCfgs=cfgs;
      type=TYPE_CFGS;
    }
    else
      type=TYPE_STR;
  }
}


void CLASS::realize1(){
  if(type==TYPE_UNREALIZED){
    //let's check for xy()
    if(0==strncmp("xy(",valStr,3)){
      int x,y;
      char*p=parseLiteral(valStr+3,&x);
//TODO: check for , and ) properly.
//fprintf(stderr,"cDatum::realize() after x,'%s'\n",p);
      p=parseLiteral(p+1,&y);
//fprintf(stderr,"cDatum::realize() after y,'%s'\n",p);
      free(valStr);
      type=TYPE_LOCXY;
      valX=x;
      valY=y;
      return;
    } 
    if(0==strncmp("cfg:",valStr,4)){
      //cfg contains many name:val pairs...
      char* p = strtok(valStr," ");
      int maxcfgs=256;
      cCollection* cfgs = new cCollection(maxcfgs);
int counter=0;
      while(true){
        p=strtok(NULL," :");
        if(!p) break;
counter++;
if(counter>=maxcfgs){
  fprintf(stderr,"Error: Maximum of %d cfgs allowed; exceeded\n",maxcfgs);
  throw(-1);
}
        char* name=p;
        //An empty cfg has a " " here...  strtok(NULL," " will just eat the 
        //next name, so check explicitly.  TODO: this sucks, and should 
        //be replaced by a stream parsing class...
        char* val=(char*)""; //for empty cfg
        if(' '!=*(name+strlen(name)+1)) {
//fprintf(stderr,"xxx[%s]\n",name+strlen(name)+1);
          p=strtok(NULL," ");
          if(!p) {
            fprintf(stderr,"Error realizing cfg string\n");
            throw(1);
          }
          val=p;
        }
//fprintf(stderr,"REALIZED #%d [%s:%s]\n",counter,name,val);
        cfgs->add(name,strlen(name),cDatum::newStr(val,strlen(val)));
//fprintf(stderr,"[[%s:%s]]\n",name,val);
      }
      cfgs->solidify();      
      delete valStr;
      valCfgs=cfgs;
      type=TYPE_CFGS;
    }
    else
      type=TYPE_STR;
  }
}
/*=====================================================================
 outputLoc - used by xdl output
======================================================================*/
char* CLASS::outputLoc(){
  switch(type){
    case TYPE_STR:
      strcpy(buf,valStr);
      break;
    case TYPE_LOCXY:
      sprintf(buf,"SLICE_X%dY%d",valX,valY);
      break;
    default:
fprintf(stderr,"cCollection:outputLoc - Not a location!\n");
throw(1);
  }
  return buf;
}
void CLASS::dump(FILE*f){
  switch(type){
    case TYPE_INT: fprintf(f,"int:%d ",valInt);break;
    case TYPE_STR: fprintf(f,"str:\"%s\" ",valStr);break;
    case TYPE_PIN: fprintf(f,"pin:%s ",valInt?"output":"input");break;
    case TYPE_SUB: valSub->dump(f); break;
    case TYPE_LOCXY:
      fprintf(f,"(%d,%d) ",valX,valY); break;
    case TYPE_LOCABS:
      fprintf(f,"abs:%s ",valStr); break;
    case TYPE_GOODCFGS:
      fprintf(f,"good cfgs:%s ",valStr); break;
    case TYPE_PROTO: valProto->dump(f); break;
    case TYPE_TILE: fprintf(f,"tile:\"%s\" ",valStr);break;
    case TYPE_PARSUB: fprintf(f,"parsub:<%s> ",valStr);break;
    case TYPE_UNREALIZED: fprintf(f,"unr:\"%s\" ",valStr);break;
    case TYPE_CFGS:
      valCfgs->dump(f,"cfgs:"); break;
      
    default: fprintf(f,"cCollection::dump - unknown type %d\n",type);
 }
}




