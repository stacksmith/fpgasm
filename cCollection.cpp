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
#define CLASS1 cDatum

char *CLASS1::buf=(char*)malloc(32); //- SLICE_X...Y...\0

CLASS1::CLASS1(U8 typ){
  type=typ;
}
CLASS1::~CLASS1(){
    switch(type){
      case TYPE_UNREALIZED:
      case TYPE_STR: 
        free(valStr); break;
    }
    //TODO: comprehensive destructor
}
cDatum* CLASS1::newStr(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_STR);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS1::newInt(int val){
  cDatum* ret = new cDatum(TYPE_INT);
 
  ret->valInt=val;
  return ret;
}
cDatum* CLASS1::newPin(int dir,int bwidth){
  cDatum* ret = new cDatum(TYPE_PIN);
  ret->pinDir=dir&1;
  ret->pinBusWidth=bwidth&0xFF;
  return ret;
}
cDatum* CLASS1::newSub(cSub* val){
  cDatum* ret = new cDatum(TYPE_SUB);
 
  ret->valSub=val;
  return ret;
}
cDatum* CLASS1::newLocXY(int x,int y){
  cDatum* ret = new cDatum(TYPE_LOCXY);
 
  ret->valX=x;
  ret->valY=y;
  return ret;
}
cDatum* CLASS1::newLocAbs(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_LOCABS);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}  
cDatum* CLASS1::newGoodCfgs(const char* str,int len){
  cDatum* ret = new cDatum(TYPE_GOODCFGS);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
} 
cDatum* CLASS1::newProto(cProto* val){
  //proto reference
  cDatum* ret = new cDatum(TYPE_PROTO);
 
  ret->valProto=val;
  return ret;
}

cDatum* CLASS1::newTile(const char*str,int len){
  cDatum* ret = new cDatum(TYPE_TILE);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS1::newParSub(const char*str,int len){
  cDatum* ret = new cDatum(TYPE_PARSUB);
 
  ret->valStr = (char*)malloc(len+1);
  strncpy(ret->valStr,str,len);
  ret->valStr[len]=0;
  return ret;
}
cDatum* CLASS1::newUnrealized(const char* str,int len){
  cDatum* ret=CLASS1::newStr(str,len);
  ret->type=TYPE_UNREALIZED;
  return ret;
  
}
/******************************************************************************
 parseLiteral
                                                                                
******************************************************************************/ 
char* CLASS1::parseLiteral(char*start,S32*presult){
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
void CLASS1::realize(){
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


void CLASS1::realize1(){
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
char* CLASS1::outputLoc(){
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
void CLASS1::dump(FILE*f){
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






#define CLASS cCollection

CLASS::CLASS(int max){
  //  chunk = g_string_chunk_new(32);
    name = (char**)malloc(max*sizeof(char*));
    data = (cDatum**)malloc(max*sizeof(cDatum*));
    size=0;
#ifdef DEBUG
    debugmax=max;
#endif
}
CLASS::CLASS(){
    name = (char**)malloc(256*sizeof(char*));
    data = (cDatum**)malloc(256*sizeof(cDatum*));
    size=0;
#ifdef DEBUG
    debugmax=256;
#endif
}
CLASS::CLASS(const CLASS& src){
//fprintf(stderr,"COPY CONSTRUCTOR %d\n",src.size);
    name=src.name; //reuse name array
    //data array will be copied.  That way data can be replaced
    //without affecting the src.
    data=(cDatum**)malloc(src.size*sizeof(cDatum*));
    memcpy(data,src.data,src.size*sizeof(cDatum*));
    size=src.size;
}
CLASS::~CLASS(){
  free(name); //name array
  int i;
  for(i=0;i<size;i++){
    delete data[i];
  }
  free(data);
 
}

void CLASS::resize(int max){
  
}
int CLASS::add(const char* str,int len,cDatum* dat){
#ifdef DEBUG
  if(size>=debugmax){
    fprintf(stderr,"cCollection:%s add RANGE ERROR - overwriting RAM\n",debugname?debugname:"unnamed");
    fprintf(stderr,"max is %d, writing at %d\n",debugmax,size);
    fprintf(stderr,"len %d str %s\n",len,str);
this->dump(stderr,"DUMPING");
  
    throw(1);
}
#endif
//printf("ADDING %s \n",str);
  char*p = (char*)malloc(len+1);
  memcpy(p,str,len);
  p[len]=0;
  name[size]=p;
  data[size]=dat;
  return size++;
}
/******************************************************************************
  clone 
  Occasionally we need an item that is just a reference to an existing
  name:value pair.  Clone does just that.  
******************************************************************************/ 
int CLASS::addClone(const char* str,cDatum* dat){
  name[size]=(char*)str;
  data[size]=dat;
  return size++;
 
}

void CLASS::solidify(){
  //replace builder with properly sized copy of the array
  char** newname = (char**)malloc(size*sizeof(char*));
  memcpy(newname,name,(size*sizeof(char*)));
  name=newname; //release builder  
  //replace data with properly sized copy of the array as well
  cDatum** newdata=(cDatum**)malloc(size*sizeof(cDatum*));
  memcpy(newdata,data,(size*sizeof(cDatum*)));
  data=newdata; //release builder
#ifdef DEBUG
  debugmax=size;
//fprintf(stderr,"Solidified %s to %d elements",debugname?debugname:"unknown",size);
#endif
//fprintf(stderr,"PRE-SOLIDIFIED. \n");
//dump(stderr);
//fprintf(stderr,"SOLIDIFIED. \n");
//dump(stderr);
}

int CLASS::find(const char* str){
  return find(str,strlen(str));
}
int CLASS::find(const char* str,int len){
//fprintf(stderr,"cCollection:find(%s,%d)\n",str,len);
  int j;
  for(j=size-1;j>-1;j--){
    if(0==strncmp(str,name[j],len))
      if(0==name[j][len])
        break;
  }
//if(-1==j)
//  fprintf(stderr,"find %s %d failed\n",str,len);
//if(-1!=j)fprintf(stderr,"found:%d %s\n",j,name[j]);
  return j;
}
char* CLASS::getName(int i){
  if(i>=0)
    return name[i];
  else {
#ifdef DEBUG
    fprintf(stderr,"cCollection %s attempted to getName(%d)\n",debugname,i);
#else
    fprintf(stderr,"cCollection attempted to getName(%d)\n",i);
#endif
    throw(0);
  }
}
cDatum* CLASS::getDatum(int i){
  if(i>=0)
    return data[i];
  else {
#ifdef DEBUG
    fprintf(stderr,"cCollection %s attempted to getData(%d)\n",debugname,i);
#else
   fprintf(stderr,"cCollection attempted to getData(%d)\n",i);
#endif
   throw(0);
  }
}
int CLASS::indent=0;
void CLASS::dump(FILE*f,const char* title){
  if(!size) return;
  indent+=2;
  if(title)
    fprintf(f,"%s",title);
  else
    fprintf(f,"collection with %d items:\n",size);
  int i;
  for(i=0;i<size;i++){
    int j;
    for(j=0;j<indent;j++) fprintf(f," ");
    fprintf(f,"%d. '%s' ",i,name[i]);
    if(data[i])
      data[i]->dump(f);
    else {
      fprintf(f,"DATA IS NULL!!!\n"); 
    }
    fprintf(f,"\n");
  }
  indent-=2;
}
#ifdef DEBUG
void CLASS::setdebugname(char*name,int len){
  debugname = (char*)malloc(len+1);
  memcpy(debugname,name,len);
  debugname[len]=0;
}
#endif
  

