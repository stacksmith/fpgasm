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
#include "cParse.h"
#include <ctype.h>
#include "cModule.h"
#include "cSub.h"
#define CLASS cParse
#include "cDevice.h"
extern cDevice* pDevice;
#include "cWires.h"
CLASS::CLASS(){
  file=0;
  buf=(char*)malloc(1024);
  ptr=buf;
  lineno=0;
}
CLASS::~CLASS(){
  free(buf);
}
/******************************************************************************
    handleInclude  
    
                                                                   
******************************************************************************/ 
bool CLASS::handleInclude(int len){
  static const char* funcname="handleInclude()";
  if(tokAnything("#include",len)){
    ws(true);
   if('"'==*ptr){
      cDatum* d = parseQuotedString();
       /* and the type of the file included is... */
      char* xdlrc = strstr(d->valStr,".xdlrc");
      if(xdlrc){
        if(pDevice){
          errorIn(funcname);
          fprintf(stderr,"Device is already included");
          error(1);
        }
        pDevice = new cDevice();
        pDevice->initialize();
        pDevice->parse_xdlrc(d->valStr);
   
      } else {
        FILE* f = fopen(d->valStr,"r");
        if(!f) {
          errorIn(funcname);
          fprintf(stderr,"Unable to open file '%s'\n",d->valStr);
          error(1);
        }
        /* to handle reentrant includes, preserve old state */
        FILE* fold = file;
        int linenoOld=lineno;
        lineno=0;
        parse(f);
        file=fold;
        lineno=linenoOld;
      }
    } else {
      errorIn(funcname);
      fprintf(stderr,"filename must be in quotes");
      error(1);
    }
    return true; //included.
  } else return false; //not an include...
}
/******************************************************************************
    parsePins
    
    parse a declaration of pins in the module.
    
    name        scalar pin
    name[3]     a bus.  Width must be a literal integer 1-255
    
                                                                   
******************************************************************************/ 
void CLASS::parsePins(cCollection* pins,int dir){
  ws(true);
  int len=requireWord("(",1);
  ws(true);
  if(')'==*ptr)  //in case it's a ()
    ptr++;
  else do {
    int i= pins->add(ptr,len,cDatum::newPin(dir,1)); //for now, scalar pin.
    ptr+=len;                         //skip pin name
//showtok();
    if('['==*ptr){ //defining a pin bus
      ptr++;
      ws(true);
      int width = parseLiteral();
      if((width<1)||(width>255)) {
        errorIn("parsePins");
        fprintf(stderr,"A bus must have at least 1 and at most 255 pins\n");
        error(1);
      }
      pins->data[i]->pinBusWidth=width;
      len=requireWord("]",1);
    }
    char sep;
    len=requireSeparator("parsePins",",; \t\n)",&sep); //ws with comma allowed here...
    if(')'==sep) break;
  } while(true);
}
/******************************************************************************
      parseParamNames
      
In the declaration of parameters ( NAME (ins)(outs)(params)) parameters names
are declared as a list.  In addition, default values may be declared as
name:default value.
                                                                               
******************************************************************************/ 
void CLASS::parseParamNames(cModule* module){
//fprintf(stderr,"PARAM [%s]\n",ptr);
  requireSeparator("parseParamNames","("); //we need a (
  bool doit=true;
  while(doit){
    ws(true);
    int len = cnt();
    if((1==len)&&(')'==*ptr)){
      ptr++;
      break;
    }
    //start by creating a named parameter with NULL data
    int index=module->paramnames->add(ptr,len,0);
    ptr+=len;
    ws(true);
    char c = *ptr;
    switch(c){
      case ':' :
        //Default value provided.
        ptr++;
        ws(true); len=cnt();
        module->paramnames->data[index]= cDatum::newStr(ptr,len);
        ptr+=len;
        // now either , or ).  Set c and fallthrough! WATCH OUT FOR ORDER HERE
        c=*ptr;
        switch(c){
          case ',': ptr++; break;
          case ')': ptr++; doit=false; break;
        }
//fprintf(stderr,"post-default parameter parser at [%s]\n",ptr);
//fprintf(stderr,"DEFAULT PARAMETER %s:%s\n",module->paramnames->name[index],module->paramnames->data[index]->valStr  );
        break;
      case ',':
        ptr++;
        break;
      case ')':
        ptr++;
        doit=false;
        break;
    }
  }
  module->paramnames->solidify();
}
/******************************************************************************
      parseWireEndpoint
      
Parse a unit of wire declaration.
                                                                               
******************************************************************************/ 
sWireEndpoint CLASS::parseWireEndpoint(cModule* module,int idxInst,cSub* pinst){
//fprintf(stderr,"STARTING [%s]\n",ptr);
  cCollection*ppins;
  sWireEndpoint ep;
  ep.busid1=ep.busid2=0; //for simple scalar wires.
//  cProto *proto;
  ws(true); int len=cnt();
  cProto* pinowner; //for error reporting mainly...
  if(tokAnything("my",len)){
    ep.inst=0xFF;
    ppins=module->pins;
    pinowner=module;
    /*proto=module;*/
  } else {
    if((tokAnything("his",len)) && (-1 != idxInst)){
      ep.inst=idxInst;
      ppins=pinst->pins;
    } else {
      /* find the inst by name
      */
      validateName(len);
      int inst =module->psubs->find(ptr,len);

//fprintf(stderr,"FOUND inst %s at %d [%s]\n",module->psubs->name[ep.inst],ep.inst,ptr);
      if(-1==inst){
        errorIn("parseWireEndpoint()");
        fprintf(stderr,"Instance '%.*s' has not been declared in module '%s'\n",
                len,ptr,module->name);
        error(1);
      }
      ep.inst=inst;
      ppins =module->psubs->data[ep.inst]->valSub->pins;
      ptr+=len;
    }
      pinowner = module->psubs->data[ep.inst]->valSub->type;
  }
  // handle 's neatly
  if(('\''==*ptr)&&('s'==*(ptr+1))) ptr+=2;

  ws(true); len=cnt();
  if(tokAnything("vcc",len)){
    ep.pindex=0xFF;
  } else if(tokAnything("gnd",len)){
    ep.pindex=0xFE;
  } else {
    int pindex = ppins->find(ptr,len);
//fprintf(stderr,"FOUND PIN %d %s at %d \n",len,ptr,pindex);
    if(-1==pindex){
      errorIn("parseWireEndpoint()");
      fprintf(stderr,"Pin '%.*s' has not been declared in module '%s' \n",
              len,ptr,
//              module->psubs->data[ep.inst]->valSub->name,  //wire's owner sub
              pinowner->name //and its type
             );
      error(-1);
    }
    ep.pindex= pindex; //don't forget it's a byte!
    ptr+=len;
    /* could be a [busid] or range [busid1:busid2] */
    if('['==*ptr){
//fprintf(stderr,"BUS %s\n",ptr);
       ptr++;
      ws(true); ep.busid2=ep.busid1=parseLiteral();
      if((ep.busid1<0)||(ep.busid1>=(ppins->data[ep.pindex]->pinBusWidth))) {
        errorIn("parseWireEndpoint");
        fprintf(stderr,"Pin %s[%d] has an invalid index; should be between %d and %d\n",
                ppins->name[ep.pindex],
                ep.busid1,
                ep.busid1+1,
                ppins->data[ep.pindex]->pinBusWidth-1);
        error(1);
      } 
      // is it a range?
//fprintf(stderr,"XXX ..%s..\n",ptr);
      if(':'==*ptr){
        ptr++;
        ws(true); ep.busid2=parseLiteral();
        if((ep.busid2<=ep.busid1)||(ep.busid2>=(ppins->data[ep.pindex]->pinBusWidth))) {
          errorIn("parseWireEndpoint");
          fprintf(stderr,"Pin %s[%d:%d] has an invalid index; should be between %d and %d\n",
                ppins->name[ep.pindex],ep.busid1,
                ep.busid2,
                ep.busid1+1,
                ppins->data[ep.pindex]->pinBusWidth-1);
          error(1);
        } 
      }
      ws(true);requireWord("]",1);
    }
  }
//  fprintf(stderr,"PARSED: %s inst %d index %d from %d to %d\n",
 //      ppins->name[ep.pindex], ep.inst,ep.pindex,ep.busid1,ep.busid2);
//fprintf(stderr,"AT {%s}\n",ptr);
  return ep;
}
/******************************************************************************
  wire
  
  Wires are stored in the module.  At wiring time, the names of
  relevant insts are in the inst array,and the names of pins are
  solidly in the proto's arrays.  We are free to use the bytecode
  encoding scheme as follows (each item is a byte index)
  <srcInst><pin> <dstInst><pin> ... 
  $FF inst means us, the module.
                                                                               
******************************************************************************/ 

void CLASS::parseWire(cModule* module,int idxInst,cSub* pinst){
  // start by parsing the source.  It may refer to a bus, in which case we shall
  // loop for every wire in the bus...
  // For starters, create a fixed array of 16 endpoints.
  #define MAX_ENDPOINTS 16
  sWireEndpoint ep[MAX_ENDPOINTS];
  //parse all the endpoints. ep[0] is the source endpoint.
  ep[0]=parseWireEndpoint(module,idxInst,pinst);
  int srcBusWidth= ep[0].busid2 - ep[0].busid1; //width-1; 0 means scalar... 
//fprintf(stderr,"module '%s'; width %d\n",module->name,buswidth);
  int i=1; //index of endpoint being processed
  int len;
  while(true){
    ws(true); len=cnt();
    if(tokAnything(",",len)) //just skip it
      len=cnt();
    if(tokAnything("to",len))
      len=cnt(); //just skip it
    ep[i]=parseWireEndpoint(module,idxInst,pinst);
    /* if source is a scalar, we can connect it to all destination bus widths.
       if it's a bus, it has to match the destination endpoint buswidths. */
    if(srcBusWidth) { //a scalar can go to a bus.. 
      if(srcBusWidth != (ep[i].busid2-ep[i].busid1)){
        errorIn("parseWire");
        fprintf(stderr,"bus width mismatch. Expected %d, found %d\n",srcBusWidth+1,ep[i].busid2-ep[i].busid1+1);
        error(1);
      }
    }
    i++;
    if(i>(MAX_ENDPOINTS-1)){
      errorIn("parseWire");
      fprintf(stderr,"Wire chain too long; maximum 16 exceeded\n");
      error(1);
    }
    ws(true); len=cnt();
    if(tokAnything(";",len)) break;
  }
  // Now add wires to the module.  The reason for all this is that buses
  // must be looped through wire by wire.  So now, loop on first one's
  // bus width.
//fprintf(stderr,"WILL ADD BUS OF %d WIRES\n",buswidth+1);
  if(srcBusWidth){
//fprintf(stderr,"bus wire in module '%s'; width %d\n",module->name,srcBusWidth);
    int j;   
    for(j=0;j<srcBusWidth+1;j++){ //for every wire in the bus
      int k;
      for(k=0;k<i;k++) {    //connect every endpoint
        module->pwires->add(
          ep[k].inst,
          ep[k].pindex,
          ep[k].busid1+j);
      }
      module->pwires->close(); //and end the wire
    }
  } else{ //source is a scalar; connect it to all destinations
    //source wire is a scalar...
   module->pwires->add( ep[0].inst,ep[0].pindex,ep[0].busid1);
    int ei; //endpoint index
    for(ei=1;ei<i;ei++){ //for every endpoint (after source endpoint)
      int j;
     for(j=ep[ei].busid1; j<=ep[ei].busid2; j++){ //for every bus wire
//  fprintf(stderr,"scalar wire in module '%s'; wiring to [%d]\n",module->name,j);
       module->pwires->add(ep[ei].inst, ep[ei].pindex,j);
      }
    }
    module->pwires->close(); //one wire from source, closed.
  }
}

/******************************************************************************
  parseQuotedString.  
  Enter with ptr at "; return with ptr past ".
                                                                                 
******************************************************************************/ 
cDatum* CLASS::parseQuotedString(){
  char*p=++ptr; //ptr is past "
  int len=0;
  char c;
  // Now count the characters until "
  while('"' != (c=*p++)){
    len++;
    //TODO: check for macro insertion of parameter names...
  }
  cDatum* ret = cDatum::newStr(ptr,len);
  ptr+=len+1;
//fprintf(stderr,"QUOTED STR [%s],%d\n",ret->valStr,strlen(ret->valStr));
  return ret;
}
/******************************************************************************
  parseParamData - TRICKY!  We will create 'unrealized' strings to be
  expanded during dynamic expansion phase.
  Right now it's only cfgs...                                                                              
******************************************************************************/ 
cDatum* CLASS::parseParamData(cModule*module,const char* prepend){
  cDatum*ret;
  int len;
  if('{'==*ptr){
    /* cfg-style list. Parse it as a long string, a pair at a time
     separated by spaces. It's a little messy as we need to do multi-line */
    ptr++;
    int total=0;
    int max = 8192; //TODO:bufsize
    char*bu=(char*)malloc(max);
    bu[0]=0;
    if(prepend){
      strcpy(bu,prepend);
    }
    while(true){
      ws(true); len=cnt(" }\0");
//fprintf(stderr,"parseParamData at: %d [%s]\n",len,ptr);
      if((len==0)&&('}'==*ptr)) { break;}
      total+=len;
      if(max<=total){
        errorIn("parseParamData()");  
        fprintf(stderr,"out of space parsing a cfg string\n");
        error(1);
      }
      strncat(bu,ptr,len);
      strcat(bu," ");
      ptr+=len;
      //If we bounced on ' )', the ) will stump us...
      ws(true); //after there may be a ws
    }
    //strcat(bu,";");
    ptr++;
    ret=cDatum::newUnrealized(bu,strlen(bu));
//fprintf(stderr,"unrealized buffer is %d\n",strlen(bu));
    free(bu);
    //done
  } else {
    len=cnt(" ;");
//fprintf(stderr,"QQQ %d, [%.*s]\n",len,len,ptr);
    ret = cDatum::newUnrealized(ptr,len);
    ptr+=len;
  }
  return ret;
}
/******************************************************************************
  parsePairs.  
  Our sub started exactly like its type paramnames, with defaults.
  scan the pairs and fix the data                                                                             
******************************************************************************/ 
void CLASS::parsePairs(cModule* module,cSub* sub){
  char sep;
  while(true){
    ws(true);
    if(';'==*ptr){
      ptr++;
//fprintf(stderr,"ending parsing of inst %s in module %s\n",sub->name,module->name); 
      break;
    }
//    cDatum *val;
    //-------------------------------------------------------------------------
    // parse a name:value pair.  Note: the entire cfg string is a single pair,
    // and is parsed out in cDyn during expansion.  The value may span multiple
    // lines, rendering pname invalid ...
//fprintf(stderr,"PARSING PAIR {%s}\n",ptr);
  char*pname;
  int  pnamelen;
    pnamelen=cnt();
    pname = (char*)malloc(pnamelen+1);
    strncpy(pname,ptr,pnamelen);
    pname[pnamelen]=0;
    // First of all, our parameter name must match the type's paramname.  Note:
    // the type's index is not important - our order may be different.
    //if module declares this parameter name, we are good.
    int i = sub->type->paramnames->find(pname,pnamelen);
    if(-1==i){
      errorIn("parsePairs()");
      fprintf(stderr,"Argument named '%.*s' is not a part of the parameter list of module '%s'\n",
              pnamelen,pname,sub->type->name);
      error(1);
    }
    ptr+=pnamelen;
    requireSeparator("parsePairs()",":",&sep);
    // To facilitate cfg merging, we should see if the name has occurred and
    // if it has, reuse the index.
//fprintf(stderr,"ParsePairs - value to be parsed from{%s}\n", ptr);
// fprintf(stderr,"ParsePairs - value parsed is{%s}\n", val->valStr);
   int q = sub->pparams->find(pname,pnamelen);
    if(-1==q) {
      // first time parameter is created
     cDatum* val = parseParamData(module,"cfg: "); //as unrealized string...
      q=sub->pparams->add(pname,pnamelen,val);
//fprintf(stderr,"in sub %s, added new parameter %.*s, [%s]\n",sub->name,
//        pnamelen,pname,val->valStr);
    } else {
      // an existing parameter must be merged
      cDatum* val = parseParamData(module,0);
      cDatum* oldparm = sub->pparams->data[q];
//fprintf(stderr,"Expanding old parameter [%s]\n", oldparm->valStr);
//fprintf(stderr,"Will append [%s]\n", val->valStr);
      int newlen =strlen(oldparm->valStr) + strlen(val->valStr) +1;
      oldparm->valStr = (char*)realloc(oldparm->valStr,newlen); 
      strcat(oldparm->valStr,val->valStr);
      delete val;
    }
    free(pname);
    // TODO: fix this later.  for parameters like xy(1,2  ), the space bounces
    // us out, and the ) is looming ahead...
    ws(true);
    if(')'==*ptr) ptr++;// for .. )
//fprintf(stderr,"ADDING PARAMETER %s %d %d\n",sub->pparams->name[i],i,q);    
  }
  //-----------------------------------------------------------------
  // Now walk through our sub's type's parameter names to make sure
  // all were declared here in the sub.  This only applies to modules
  // insts are allowed to carry cfg parameters that are just not used.
  if(sub->type->psubs){
//fprintf(stderr,"type is %s\n",sub->type->name);
//sub->type->paramnames->dump(stderr,"PARAMNAMES");
    int tsize = sub->type->paramnames->size;
    int i;
    for(i=0;i<tsize;i++){
      char* pname=sub->type->paramnames->name[i];   // pull a parameter name
      int idx=sub->pparams->find(pname);            //find by name in subs
      if(-1==idx){
        // Type declares a parameter, but we did not fill it.  Hopefully there
        // is a default value!
        if(sub->type->paramnames->data[i]){
          //Bingo! a default value.  So create a parameter
          sub->pparams->addClone(sub->type->paramnames->name[i],sub->type->paramnames->data[i]);
        } else {
          errorIn("parsePairs()");
          fprintf(stderr,"Module '%s' declares parameter '%s' that is never set in inst '%s'. Check the ;\n",
                sub->type->name,
                pname,
                sub->name);
          error(1);
        }
      }
    }
  }
  sub->pparams->solidify();
//fprintf(stderr,"After all pairs, the pparams is %d \n",sub->pparams->debugmax);
  
}
/******************************************************************************
      parseSub
      
      parse an instance declaration.
                                                                                
******************************************************************************/ 
cSub* CLASS::parseSub(cModule* module,int len){
  // NAME
  validateName(len,module);
  cSub* sub = new cSub(ptr,len);  
  module->psubs->add(ptr,len,cDatum::newSub(sub));    //add to the collection...
  ptr+=len;  
  // TYPE
  ws(true);
  int size=cnt();
  cProto* proto = pDevice->findProto(ptr,size); //find prototype by name
  if(!proto) {
    errorIn("parseSub");
    // A common error mode is a word taken for the name and : for type.
    if((1==size)&&(':'==*ptr)) {
      fprintf(stderr,"Run-on parameter.  The previous ; probably should not be there\n");
    } else {
      fprintf(stderr,"instance %s is of an invalid type '%.*s'\n",sub->name,size,ptr);
    }
    error(1);
  }
  sub->type=proto;
  sub->pins=proto->pins;

  ptr+=size;
  // LOCATION.  This should be (x,y) or name
  //sub->setLocation(parseLocation());
  //Start with an empty, expandable parameter collection
  sub->pparams = new cCollection();
  /* pparams will have name:value pairs.  A notable one is cfg:xxx
   because it contains a whole collection inside.  */
 
  // any variable pairs
  parsePairs(module,sub);
// fprintf(stderr,"------------------\n");  
//sub->pparams->dump(stderr,sub->name);
// fprintf(stderr,"------------------\n");  
  //DONE
  return sub;
}
/******************************************************************************
     parseMerge
     following an inst declaration,                                                                           
******************************************************************************/ 
/*void CLASS::parseMerge(cModule*module,cSub* inst){
fprintf(stderr,"merging inst %s\n",inst->name);
  static const char* funcname="parseMerge";
  if(!inst){
    errorIn("parseMerge()");
    fprintf(stderr,"No module declared to merge configurations into\n");
    error(1);
  }
  //merge can extend inst's pins.  Since inst started out with a reference
  //to type module's pins, let's make a copy. Now we will be appending and
  //never removing items, so we can reference individual pins in module.
  cCollection *pins = new cCollection();
  int i; for(i=0;i<inst->pins->size;i++){
    pins->name[i] = inst->pins->name[i]; //reference to type's pin's name
    pins->data[i] = inst->pins->data[i]; //reference to type's pin's data
  }
  pins->size=inst->pins->size;
  ws(true);int len=cnt();
  while(!tokAnything(";",len)){
    if(tokAnything("input",len)){
      requireSeparator(funcname,":");
      parsePins(pins,0); //inputs
    }else if(tokAnything("output",len)){
      requireSeparator(funcname,":");
      parsePins(pins,1); //outputs
    } else if(tokAnything("cfg",len)){
      requireSeparator(funcname,":");
      cDatum* mergeparm = parseParamData(module);
      //now rewrite the cfg by merging them
      int ioldcfg = inst->pparams->find("cfg");
      if(-1==ioldcfg){
        errorIn(funcname);
        fprintf(stderr,"Instance %s has no cfg parameter\n",inst->name);
        error(1);
      }
      cDatum* pOldCfg = inst->pparams->data[ioldcfg];
//fprintf(stderr,"old cfg [%s]\n",pOldCfg->valStr);
     //the length is both strings.  Since only one will start with cfg: -5.
      int newlen = strlen(mergeparm->valStr) + strlen(pOldCfg->valStr)-5;
      char* newCfgStr = (char*)malloc(newlen+1);
      strcpy(newCfgStr,pOldCfg->valStr); //start with old string
      strcat(newCfgStr,mergeparm->valStr+5); //append new
//fprintf(stderr,"merge parsed cfg[%s]\n",newCfgStr);
      // Now delete old data
      delete mergeparm;
      free(pOldCfg->valStr);
      //and replace it
      pOldCfg->valStr=newCfgStr;
//fprintf(stderr,"merged inst %s\n",inst->name);
//inst->pparams->dump(stderr,"XXX");

      
    }
    else {
      errorIn("parseMerge()");
      fprintf(stderr,"merge must contain only input:() output:() or ;\n");
      error(1);
    }
    ws(true );len=cnt();
  }
  //now replace our inst's pins with the new version.  Do not delete old-
  //it was just a pointer to type's
  pins->solidify();
  inst->pins=pins;
}
*/
  /******************************************************************************
     parseModule
                                                                                
******************************************************************************/ 
cModule* CLASS::parseModule(){
  static const char*funcname="parseModule()";
//  if(!ws(/*required*/false)) return 0; //done!
  ws(true);
  int size=cnt();
  validateName(size);
  //first check for duplications
  if(-1 != pDevice->idxFindProto(ptr,size)){
    errorIn(funcname);
    fprintf(stderr,"Module '%.*s' already exists\n",size,ptr);
    error(1);
  }
  cModule* module = new cModule(ptr,size); //it will check for dups
  ptr+=size;
  // parameter names
  parseParamNames(module); 
  //Now inputs and outputs
  ws(true); size=cnt();
  while(!tokAnything("{",size)){
    if(tokAnything("input",size)){
      requireSeparator(funcname,":");
      parsePins(module->pins,0); //inputs
    }else if(tokAnything("output",size)){
      requireSeparator(funcname,":");
      parsePins(module->pins,1); //outputs
    }
    else {
      errorIn("parseModule()");
      fprintf(stderr,"module header must contain only input:() output:() or ;\n");
      error(1);
    }
    ws(true );size=cnt();
  }
  //----------------------------------------------------------------
  // now parse insts and wiring...
  module->pwires=new cWires(module); //there may be many wires...
  
  int idxInst=-1; //keep track of most recent instance index for 'his'
  cSub* pinst=0;
  ws(true);size=cnt();
  while(!tokAnything("}",size)){
   if(tokAnything("wire",size)){
    //if(tokWire(size)){
      parseWire(module,idxInst,pinst);
    }/* else if(tokAnything("merge",size)){
      parseMerge(module,pinst);
    }*/ else {
     pinst=parseSub(module,size);
      idxInst++;
    }
    ws(true );size=cnt();
    
  }
  module->pins->solidify();
//  char*p=(char*)module->pwires->buf;
//q(module->pwires->buf);
// module->pwires->dump(stderr);
 //module->pwires->dump(stderr);
//printf("ptr[%s]\n",ptr);
  module->psubs->solidify();
  module->pwires->solidify();
//q(module->pwires->buf);
//  module->pwires->dump(stderr);

//  module->dump(stderr); 
//fprintf(stderr,"DONE module %s\n",module->name);
  return module;
}
/******************************************************************************
 In between modules, #includes may occur.
                                                                                
******************************************************************************/ 
void CLASS::parseModules(){
  bool iterate=true;
  int len; 
  do{
//fprintf(stderr,"OK [%s]\n",ptr);
    /* In between modules it's possible that we run off the EOF.  Normally this
     means that we are done with an included file, or everything. */
    if(!ws(false))/*EOF*/ return;
    len=cnt();
    /* In between modules, #INCLUDE is allowed */
    if(handleInclude(len))  continue;
    if(pDevice){
      /* If device is parsed, a module is expected. */
      iterate=parseModule();
    } else {
      errorIn("parseModules()");
      fprintf(stderr,"expecting an include of an .xdlrc file\n");
      error(1);
    }
  }  while(iterate);

}
/******************************************************************************
     parse                                                                      
    called by include as well...                                                                      
******************************************************************************/ 
void CLASS::parse(FILE*f){
  file=f;
  parseModules();
}
/**************************************************************************
 SUBROUTINES 
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

bool CLASS::reload(){
//fprintf(stderr,"(reloaded)");
  ptr=buf;
  *ptr=0;
  lineno++;
  char*ret=fgets(buf,1024,file);
 return 0!=ret;
}
/******************************************************************************
     wsp is whitespace primitive.  It does not handle comments                                                                    
    called by include as well...                                                                      
******************************************************************************/ 
bool CLASS::wsp(){
  while(true){
    char c=*ptr;
    if(!c){
      if(!reload()) return false;
      c=*ptr;
    }
    if(!isspace(c)) return true;
    ptr++;
  }
}
/******************************************************************************
******************************************************************************/ 
bool CLASS::enclosedComment(){
  //for error reporting, keep track of opening position
  int errline=lineno;
  char* errsrc = strdup(buf);
  char* errptr=ptr;
  while(true){
    char* p = strstr(ptr,"*/");
    if(p) {
      ptr= p+2;
      return true;
    }
    if(!reload()){
      //even though the error occurs later, the source is here:
      memcpy(buf,errsrc,sizeof(buf));
      ptr=errptr;
      lineno=errline;
      errorIn("parseComment()");
      fprintf(stderr,"where is the closing /*\n");
      error(1);
    }
  }
}

/******************************************************************************
    wsComment        
    process a comment, return isComment                                                         
******************************************************************************/ 

bool CLASS::wsComment(){
 if(0==strncmp(ptr,"/*",2)) {
    enclosedComment(); //it updates ptr to past */
    return true;
  }
  if(0==strncmp(ptr,"//",2)) {
    /* handle // comment to eol by reloading */
    if(!reload()){ 
      errorIn("wsComments()");
      fprintf(stderr,"Out of source in // comment");
      error(1);
    }
    //reload succeeded.
    return true;
  }
  // not a comment.
  return false;
}
/******************************************************************************
    ws and comments...
    return true if source ok, false if ran out.        
   
Note: calling ws(FALSE) and checking for false is a way to see if we ran out
of soruce in an include file, for instance. 
******************************************************************************/ 
bool CLASS::ws(bool required){
  do{
    if(!wsp()) {
      if(required){
        errorIn("ws()");
        fprintf(stderr,"Out of source");
        error(1);
      }else
        return false;          //end at ws
    }
    
    // After skipping space, check for comment. If comment, skip and
    // repeat.
  } while(wsComment());
  return true;
}

int CLASS::cnt(){
  int size=0;
  char* p = ptr;
  char c=*p;
  if(!c) return 0;
  switch(c){
    case 0:
    case '(':
    case ')':
    case ':':
    case ';':
    case ',':
    case '[':
    case ']':
      return 1;
  }
  
  while(true){
    char c=*p;
    if(isspace(c)) break;
    if(!c) break;
    if('('==c) break;
    if(')'==c) break;
    if(':'==c) break;
    if(';'==c) break;
    if(','==c) break;
    if('/'==c) break;
    if('\''==c) break;
    if('['==c) break;
    if(']'==c) break;
   size++;
    p++;
  }
  return size;
  
}
int CLASS::cnt(const char* seps){
  int ret=0;
  char*p = ptr;
  char c;
  while((c=*p++)){
    if(strchr(seps,c))
      return ret;
    ret++;
  }
  return ret;
  
}

void CLASS::showtok(){
    int size=cnt();
    char word[64];
    memcpy(word,ptr,size);
    word[size]=0;
    fprintf(stderr,"<%s>",word);
}

// require a word to occur at parseptr
int CLASS::requireWord(const char*  str,int len){
  ws(true);
  int i=cnt();
  if(!((i==len) && (0==strncmp(ptr,str,i)))){
    errorIn("requireWord()");
    fprintf(stderr,"'%.*s' is required\n",len,str);
    error(1);
  }
  ptr+=i;
  ws(true);
  return cnt();
}
/******************************************************************************
  returns cnt()                                                                             
******************************************************************************/ 
int CLASS::requireSeparator  (const char*caller,const char* separators,char*actual){
  ws(true);         //preceding whitespace
  //If the next cha acter is a separator in the list...
  char c = *ptr;
  if((strchr(separators,c))){ 
      ptr++;
      ws(true);
      if(actual)
        *actual=c;
      return cnt(); 
  }
  errorIn(caller);
  fprintf(stderr,"Expected a separator (one of '%s'), not '%c'\n",
    separators,c);
  error(1);
  return 0; //just to stop compiler barksf
}
/******************************************************************************
                                                                               
******************************************************************************/ 
int CLASS::notEnd(){
  ws(true);
  int size=cnt();
  if((3==size)&&(0==strncasecmp(ptr,"end",3))){
    ptr+=3;
    return 0;
  }
  else
    return size;
}

/******************************************************************************
                                                                               
******************************************************************************/ 
//len is from cnt(), nothing to do with str!
bool CLASS::tokAnything(const char* str,int len){
  ws(true);
  int sl = strlen(str);
  if((len==sl)&&(0==strncasecmp(ptr,str,sl))){
    ptr+=sl;
    ws(false);
    return true;
  }
  return false;
}
/******************************************************************************
 validateName
                                                                                
******************************************************************************/ 
void CLASS::validateName(int len){
  // first character must be alpha!
  char*p=ptr;
  char c=*p++;
  if(!isalpha(c)) {
    errorIn("validateName()");
    fprintf(stderr,"Identifier must begin with an alpha character, not %c\n",
            c);
    error(1);
  }
  int i;
  for(i=1;i<len;i++){
    c=*p++;
    if('_'==c) continue;
    if(!isalnum(c)){
      ptr=p-1;
      errorIn("validateName()");
      fprintf(stderr,"Identifier must contain with an alphanumeric character, not %c\n",
            c);
     error(1);
    }
  }
  if((4==len)&&(0==strncmp(ptr,"wire",4))){
      errorIn("validateName()");
      fprintf(stderr,"Identifier must not be a reserved word. Check for last ;  \n");
     error(1);
  }    
}
void CLASS::validateName(int len,cModule* module){
  validateName(len);
  //if another sub exists with this name, error...
   int i = module->psubs->find(ptr,len);
   if(-1 != i) {
     errorIn("validateName()");
     fprintf(stderr,"Module '%s' already has an inst named '%.*s'",
       module->name,len,ptr);
    error(1);
   }
}
/******************************************************************************
 parseLiteral
                                                                                
******************************************************************************/ 
S32 CLASS::parseLiteral(){
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
      if(c<11)  //take care of : to @
        break;
      c-=7;
      if(c<0) {
        break;
      }
    }
    if(c>radix) {
      break;
//      ptr--;
//      errorIn("parseLiteral()");
//      fprintf(stderr,"invalid digit\n");
//      error(1);
    }
    accum = (accum*radix)+c;
  }
  ptr--;
  if(neg) accum = 0-accum;
  return accum;
}

/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::errorIn(const char* from){
  fprintf(stderr,"-------------------------------------------------------\n");
  fprintf(stderr,"Error in function cParse::%s\n",from);
   fprintf(stderr,"Error occured in line %d:\n",lineno);
 
}
void CLASS::warningIn(const char* from){
  fprintf(stderr,"-------------------------------------------------------\n");
  fprintf(stderr,"Warning in function cParse::%s\n",from);
   fprintf(stderr,"Warning occured in line %d:\n",lineno);
 
}
void CLASS::error(int errno){
   int spaces = ptr-buf;
  fprintf(stderr,"\n%s",buf);        //print line, complete with cr
   while(spaces--)
    fputc(' ',stderr);
  fputc('^',stderr);
  fputs("\n",stderr);
 fprintf(stderr,"----------------------------------------------------------------------\n");
  throw(errno);
}
void CLASS::warning(int errno){
   int spaces = ptr-buf;
  fprintf(stderr,"\n%s",buf);        //print line, complete with cr
   while(spaces--)
    fputc(' ',stderr);
  fputc('^',stderr);
  fputs("\n",stderr);
 fprintf(stderr,"----------------------------------------------------------------------\n");
}
