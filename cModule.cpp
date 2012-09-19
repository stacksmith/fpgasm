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
#include "cModule.h"
#include "cDevice.h"
#include "cDyn.h"
#include "cSub.h"
#define CLASS cModule

extern cDevice* pDevice;

CLASS::CLASS(char* name,int size) 
: cProto(name,size)
{
//printf("MODULE %s %d\n",name,size);
  psubs=new cCollection();
  pDevice->addProto(name,size,this); //will check for dups
}
CLASS::~CLASS(){
//  g_hash_table_destroy(subs);
}


cSub* CLASS::getSub(char*name){
  int i = psubs->find(name);
  cDatum* r = psubs->getDatum(i);
  if(TYPE_SUB!=r->type)
    return r->valSub;
  return 0;
}

/******************************************************************************
 * D U M P
*******************************************************************************/ 
void CLASS::dump(FILE*f){
  ((cProto*)this)->dump(f);
//  fprintf(f,"%d parameter names:\n",paramnames.size);
//  int i;
//  for(i=0;i<paramnames.size;i++){
//    fprintf(f," %d. '%s'\n",i,paramnames.getName(i));
//  }
  psubs->dump(f,"subs:\n");
}
/******************************************************************************
 * Verilog
 * Output this module as a verilog module.
*******************************************************************************/ 
void CLASS::verilog(FILE*fout){
  // first, check the module's instances and make sure their types are known.
  if(psubs){
    int i; for(i=0;i<psubs->size;i++){
      cSub* sub=psubs->data[i]->valSub;
      if(!sub->type->converted)
        sub->type->verilog(fout);
    }
  }
  // Now output the module verilog style
  fprintf(fout,"module %s(\n",name);
  // pins
  pins->vlogPinDefs(fout);
  fputs(");\n",fout);
  
  // output the module's instances
  if(psubs){
    int i; for(i=0;i<psubs->size;i++){
      cSub* sub=psubs->data[i]->valSub;
      //Create a wire for every pin of this sub, before the sub...
      sub->pins->vlogWireDefs(fout,sub->name);    
       //first, the loc
      //fprintf(fout,"(*RLOC="
      fprintf(fout,"  %s ",sub->type->name);
      fprintf(fout," %s(",sub->name);
      sub->pins->vlogPins(fout,sub->name); //output the pins in parens
      fputs(");\n",fout);
    }
  }
  // wire up our module
  vlogWiring(fout);
  fputs("\n",fout);
  fputs("endmodule\n",fout);
}
/******************************************************************************
 At this point we defined the module's instances, and declared all inst inputs
 and outputs as verilog wires.  Now the module wiring can connect them all!
******************************************************************************/
void CLASS::vlogWiring(FILE*fout){
  fprintf(fout,"WIRING GOES HERE\n");
  cWireList wl = pwires->seekFirst();
  while(wl.exists()){
    //more wires exist.
    //source
    int sInst; int sIndex;int sBusid;
    wl.getInc(sInst,sIndex,sBusid);
    fprintf(fout,"from %d %d %d\n",sInst,sIndex,sBusid);
    while(!wl.isLast()){
      int dInst; int dIndex;int dBusid;
      wl.getInc(dInst,dIndex,dBusid);
fprintf(fout,"to %d %d %d\n",dInst,dIndex,dBusid);
      //Now, verilog-style assigns. Destination cannot be a power net.
      fputs("  assign ",fout);
      if(0xFF==dInst){
        //destination is "MY" pin
        fprintf(fout,"%s_%s=",name,pins->name[dIndex]);
      }else{
        fprintf(fout,"%s_%s=",psubs->name[dInst],psubs->data[dInst]->valSub->pins->name[dIndex]);
      }
      //now source, RHS
      if(0xFF==sInst){
        //source is "MY" pin.  The pin can also be a power net
        switch(sIndex){
          case 0xFF: /*vcc*/ fputs("vcc",fout); break;
          case 0xFE: /*gnd*/ fputs("gnd",fout); break;
          default:
            fprintf(fout,"%s_%s",name,pins->name[sIndex]);
            break;
        }
      }else{
        fprintf(fout,"%s_%s",psubs->name[sInst],psubs->data[sInst]->valSub->pins->name[sIndex]);
      }
      fputs(";\n",fout);
    }
    wl.seekNext();  //next wire
  }

  
}