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
#include "cDyn.h"
#include "cModule.h"
#include "cSub.h"
#include "cPrim.h"
#include "cDevice.h"
extern cDevice* pDevice;
#define CLASS cDyn
//char* CLASS::namebuf=(char*)malloc(256);
FILE* CLASS::fout; //static
/*
 Note: net names are hier followed by seq number.  This allows duplication
 in net sources.  Otherwise, we would need to build a wire sorter.
=====================================================================
======================================================================*/
CLASS::CLASS(cSub* ero, cDyn* const pop){
  hero=ero;
  dad=pop;
}
CLASS::~CLASS(){};


/*=====================================================================
======================================================================*/
int CLASS::banglen(char* str,char*fullstr){ //just past the bang
  char* nextbang=strchr(str,'!');
  if(!nextbang) {
    errorIn("banglen()");
   fprintf(stderr,"Argument value '%s' is missing the closing !",
        fullstr); 
    xerror(1);
  }
  return nextbang-str;
}
/*=====================================================================
  expandFile
  called by expand for cfgfile parameters
======================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
char* CLASS::expandFile(const char* filename){
  int f = open(filename,O_RDONLY);
  if(-1==f){
    errorIn("expandFile");
    fprintf(stderr,"Trying to open cfgfile \"%s\"\n",filename);
    xerror(-1);
  }
  char* data = (char*)malloc(0x10000);
  strcpy(data,"cfg: ");
  read(f,data+5,0x10000-5);
  //TODO: clean up
//fprintf(stderr,"EXPANDED\n----------------\n%s\n-------------------\n",data);
//fprintf(stderr,"EXPANDED length %d\n",strlen(data));
  return data;
    
}
/*------------------------------------------------------------------
  expand                                                            
  All data is in unrealized form; now is the time to realize it.    
  !..! substitution takes place here.                               
*-----------------------------------------------------------------*/
/*=====================================================================
======================================================================*/
void CLASS::expand(){
  char expandbuf[8192];   //TODO: BUFSIZEwill be expanding to here
  char* dest;
  char* src;
  char c;
//hierName(stderr);fprintf(stderr,"\n");
//fprintf(stderr," being expanded  %p\n",this);
  //--------------------------
  // expand parameters.  Start by copying hero's
  if(hero->pparams){
    pparams = new cCollection(*hero->pparams);
//pparams->dump(stderr,"expanding");
// hero->pparams->dump(stderr,"expanding");
  // now step through and fix the parsub values
    int i;
    for(i=0;i<pparams->size;i++){
      // If cfgfile parameter is encountered, substitute 
      // the value of the file for val.
//      if(0==strcmp("cfgfile",pparams->name[i])){
//       //(pparams->name[i])=(char*)"cfg"; //convert cfgfile into cfg
//        char*filedata=expandFile(pparams->data[i]->valStr);
//        //now replace the cDatum's string with expanded
//        free(pparams->data[i]->valStr); //don't need the filename
//        pparams->data[i]->valStr=filedata; //replace with data
//      } 
      cDatum* val=pparams->data[i];
      src=val->valStr;
      dest=expandbuf; *dest=0;
      //copy characters unless a special one is encountered...
      bool done=false;
      while(!done){
        c=*src++;
        switch(c){
          case '!':
            // Substitution!
            {
              int blen = banglen(src,val->valStr);
//fprintf(stderr,"substitution of %.*s\n",blen,src);
              //Dad should have a named parameter like this
              int idx=dad->pparams->find(src,blen);
              if(-1==idx) {
//      char pname[64];
//      strncpy(pname,src,blen);
      errorIn("expand()");
      fprintf(stderr,"While expanding argument '%s',",val->valStr);
      fprintf(stderr,"noticed that:\n containing module %s has no parameter ",dad->hero->type->name);
      fprintf(stderr,"named '%.*s'",blen,src);
      //Argument value !%.*s! requires module %s to have such a parameter named %s"
      //              ,blen,val->valStr,hero->type->name,pname);
      xerror(1);
              }
              src+=blen;  //In source string !xxx!, skip it
              src++; //skip the ! too
            //now copy dad's value
//fprintf(stderr,"will copy %s\n",dad->pparams->data[idx]->valStr);
//fprintf(stderr,"dest %s\n",dest);
              strcpy(dest,dad->pparams->data[idx]->valStr);
              dest+=strlen(dad->pparams->data[idx]->valStr);
//fprintf(stderr,"final answer: %s\n",expandbuf);
            }
            break;
 //         case ' ':
          case 0:
            done=true;
          default: 
            *dest++=c;
            break;           
        }
     }
     // Now replace parameter data with our string.
// fprintf(stderr,"REPLACING %s's parameter '%s' was {%s} now {%s}\n",//
//          hero->name,pparams->name[i],pparams->data[i]->valStr,expandbuf);
      //Do not delete - it is a shallow copy...
      pparams->data[i]=cDatum::newUnrealized(expandbuf,strlen(expandbuf));
     //Now realize...
//fprintf(stderr,"Realizing parameter %d [%s]\n",(int)strlen(pparams->data[i]->valStr),pparams->data[i]->valStr);
      pparams->data[i]->realize();
//fprintf(stderr,"Done realizing \n");
    }
  } else {
    pparams = new cCollection();
    pparams->solidify(); 
  }
//pparams->dump(stderr,"params");
//if(hero->pparams)
//  hero->pparams->dump(stderr,"hero");

  //--------------------------
  // children.
  psubs=0;
  psubcnt=0;
  if(hero->type->psubs){
    psubcnt=hero->type->psubs->size;
//fprintf(stderr,"SUB COUNT at %p %d\n",&psubcnt,psubcnt);
    if(psubcnt){
    // Now go through the children.  Inside dyn, create an array
    //pointing to each of the children, as we create them.
      psubs = (cDyn**)malloc(psubcnt*sizeof(cDyn*));
      int i;
      for(i=0;i<psubcnt;i++){
        cDatum* dat = hero->type->psubs->data[i];
        if(TYPE_SUB != dat->type) {
errorIn("expand()");
fprintf(stderr,"Module '%s' stores an invalid inst (seq %d)\n",
        hero->type->name,i);
xerror(1);
                  }
        cSub* sub = dat->valSub;
    //do whatever expansion here
        psubs[i]=new cDyn(sub,this);  
        psubs[i]->expand();
      }
    }
  }
 
}
/*=====================================================================
======================================================================*/
void CLASS::place(){
//fprintf(stderr,"1. Start placing %s %p\n",hero->name,this);
//hierName(stderr);
//fprintf(stderr,"\n");
//fprintf(stderr," subcnt at %p %d \n",&psubcnt,psubcnt);
  //first place ourselves...Simple if we are absolut
  cDatum* hloc = getLocation();
//fprintf(stderr,"2. Got location %p\n",hloc);
  if(!hloc) {
  /* No location specified in the hero.  This means use dad's loc,
  unless we are top.  In this case, just set 0,0. */
    if(dad)   
      loc = dad->loc; 
    else
      loc=cDatum::newLocXY(0,0);
  } else {
//fprintf(stderr," loc:%p \n",hloc);
//fprintf(stderr," loc:%s \n",hloc->valStr);

//fprintf(stderr,"2.location found: %d\n",hloc->type);
    switch (hloc->type){
    case TYPE_STR: 
//fprintf(stderr,"5. hloc->type is STR %s\n",hloc->valStr);
      loc = hloc; //just use our hero's location!
      break;
    case TYPE_LOCXY: //our hero is xy..
//fprintf(stderr,"6. hloc->type is XY (%d,%d)\n",hloc->valX,hloc->valY);
      loc = cDatum::newLocXY(hloc->valX,hloc->valY);//copy constructor...
      if(dad){ //TODO: what if no dad
        switch(dad->loc->type){
          case TYPE_LOCABS:
            errorIn("place()");
            fprintf(stderr,"cDyn::place(): %s has an absolute location; %s cannot be relative\n",dad->hero->name,hero->name);
            xerror(1);
            break;
          case TYPE_LOCXY: //dad is xy, so adjust...
            loc->valX += dad->loc->valX;
            loc->valY += dad->loc->valY;
            break;
          default:
            errorIn("place()");
            fprintf(stderr,"cDyn:place %s invalid loc type %d\n",hero->name,dad->loc->type);
            xerror(1);
        }
      } else {
        errorIn("place()");
        fprintf(stderr,"cDyn:place %s has no parent and cannot be placed\n",hero->name);
        xerror(1);
      }
      break;
    default:
      errorIn("place()");
      fprintf(stderr,"loc type of %s not ABS or XY, it is %d\n",hero->name,hloc->type);
      xerror(1);
      break;
    }
  }
  // now place children
    int i;
  for(i=0;i<psubcnt;i++){
    psubs[i]->place();
  }
//fprintf(stderr,"99.done placing %s\n",hero->name);
//fprintf(stderr,"loc is %p\n",loc);
} 

U32 CLASS::seq=0;

/*=====================================================================
======================================================================*/
int CLASS::childIndex(cDyn*p){
  int i;
  for(i=0;i<psubcnt;i++){
    if(p==psubs[i]) return i;
  }
  return -1;
}
/*void CLASS::hierName(){
  if(dad) {
    dad->hierName();
    strcat(namebuf,"/");
    strcat(namebuf,hero->name);
  } else {
    strcpy(namebuf,hero->name);
  }
}*/
/*=====================================================================
======================================================================*/
void CLASS::hierName(FILE*f){
  if(dad){
    dad->hierName(f);
    fprintf(f,"/%s",hero->name);
  }
  else
    fprintf(f,"%s",hero->name);
}

/*=====================================================================
======================================================================*/
cDatum* CLASS::getLocation(){
  if(pparams){
    int iloc = pparams->find("loc",3);
    if(-1==iloc) return 0;               //0 means no set location
    return pparams->data[iloc];
  } else {
    return 0; //no parameters at all
  }
}

/*=====================================================================
 error
 First, fprintf an error message to stderr.
 Then, call error - it outputs the position.
======================================================================*/
void CLASS::errorIn(const char* from){
  fprintf(stderr,"----------------------------------------------------------------------\n");
  fprintf(stderr,"Error in function cDyn::%s\n",from);
}
void CLASS::xerror(int errnox){
  fprintf(stderr,"\nError occured in definition of module '%s' instance '%s'\n",
    dad->hero->type->name, hero->name);
  fputs("while processing '",stderr);
  hierName(stderr);
  fputs("' \n",stderr);
  fprintf(stderr,"----------------------------------------------------------------------\n");
  throw(errnox);
}


#include "cDynXdl.cpp"