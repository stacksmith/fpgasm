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
// This file is included from cDyn.cpp.  It was separated purely for
// convenience as cDyn got too long, but it made no sense (as usual) to
// break it out into a class hierarchy (I really don't want to introduce
// templates here).

/*=====================================================================
======================================================================*/
/* Ok, here we go.  To output the definitions to XDL we need to walk the tree
*/
void CLASS::xdlDefs(){

  if(!hero->type->psubs){
//hierName(stderr);
//fprintf(stderr," xdlDefs %p \n",loc);
//loc->dump(stderr);
    char* primsite = loc->outputLoc();

    const char* tile = pDevice->tileFor(primsite);
    //leaf node (primitive).  Output it.
    fputs("inst \"",fout);
    hierName(fout);
    fprintf(fout,"\" \"%s\", placed %s %s,\n",
      hero->type->name,
      tile,
      primsite                    //name like SLICE_XY
    );
    // now output the cfgs. They are in a TYPE_CFG collection datatype.
//pparams->dump(stderr,"PARAMETERS:");  
    fprintf(fout," cfg \"" );
    if(pparams){
      int i;
      for(i=0;i<pparams->size;i++){
//        if(0==strncmp("loc",pparams->name[i],3)) continue; //do not output loc as cfg
        //else 
        if(0==strcmp("cfg",pparams->name[i])){
          cCollection* cfgs = pparams->data[i]->valCfgs;
          int j; for(j=0;j<cfgs->size;j++){
            fprintf(fout,"%s::%s ",cfgs->name[j],cfgs->data[j]->valStr);
          }
        } else {
          // loc is not interesting to us.
          if(0==strncmp("loc",pparams->name[i],3))
            continue; 
          
          // cfgfile is of interest here
          if(0==strncmp("cfgfile",pparams->name[i],7)){
            FILE*f=fopen(pparams->data[i]->valStr,"r");
            if(!f){
              errorIn("expandFile");
              fprintf(stderr,"Trying to open cfgfile \"%s\"\n",pparams->data[i]->valStr);
             xerror(-1);
            }
            char buf[256];
            while(fgets(buf,256,f)){
              fputs(buf,fout);
            }
            fclose(f);  
            continue;
          }
          errorIn("xdlDefs()");
          fprintf(stderr,"encountered parameter named '%s' in primitive '%s'\n",
           pparams->name[i], hero->name);
//fprintf(stderr,"dumping parameters:\n");
//pparams->dump(stderr,"xxx");
           throw(1);
        }
      }
    }
    fprintf(fout,"\";\n");
    // 
  } else {
      int i;
      for(i=0;i<psubcnt;i++){
        ((CLASS*)psubs[i])->xdlDefs();
    }
  }
}
/*=====================================================================
 Top-level entry to the wiring part of the process.. 
======================================================================*/
void CLASS::xdlWire(){
  //check for power wires originating here...
  if(psubcnt){
    xdlWirePower(); //vcc/gnd only for non-prims.
    int i; for(i=0;i<psubcnt;i++){
       psubs[i]->xdlWire();
    }
  } else {
    //Aha-a primitive...Ask the parent to send work on just us,
    //otherwise we will never know who did what.
    dad->xdlStartWires(this);
  }
}
/*=====================================================================
======================================================================*/
void CLASS::xdlStartWires(cDyn* prim){
  int refinst=childIndex(prim); //index of child primitive we love...
  cWires* wires = hero->type->pwires;
  cCollection* pins = prim->hero->pins;
  cWireList wl=wires->seekFirst();
  while(wl.exists()){
    int pinst; int pindex;int busid;
    wl.getInc(pinst,pindex,busid);
    if(refinst==pinst){ //sourced refinst?
      prim->xdlNetHeader(pins,pindex); //go in one
      xdlWireInner(pinst,pindex,busid,wl);
      fprintf(fout,";\n");
    };
    wl.seekNext();

  }
}
/*=====================================================================
======================================================================*/
//searches up and down!
void CLASS::xdlWireUpOrDown(int refindex,int busId){
  if(!psubs){
    //PRIMITIVE/leaf node... terminate net
    cCollection* pins = hero->pins;
    xdlNetInpin(pins,refindex);
  } else {
    //try to go back down
    // for wires starting with FF,pindex,busid
    cWires* wires = hero->type->pwires;
    cWireList wl=wires->seekFirst();
    while(wl.exists()){
      int pinst; int pindex;int busid;
      wl.getInc(pinst,pindex,busid);
      if((pinst==0xFF)&&(pindex==refindex)&&(busid==busId)){
         xdlWireInner(pinst,pindex,busid,wl);
      }
      wl.seekNext();
    } //after, try up as well.
    dad->xdlContinueWire(this,refindex,busId);
  }
}
//From just above, looks for wires reaching up to us.
/*=====================================================================
======================================================================*/
void CLASS::xdlContinueWire(cDyn* prim,int refindex,int busId){
//fprintf(stderr,"continueWire in %s %d\n",hero->name,refindex);
  //using our wires, find the wire from pindex of prim..
  int refinst=childIndex(prim); //index of child primitive we love...
  cWires* wires = hero->type->pwires;
  //we need to seek out one at refinst:pindex start...
  cWireList wl=wires->seekFirst();
  while(wl.exists()){
    int pinst; int pindex;int busid;
    wl.getInc(pinst,pindex,busid);
    if((pinst==refinst)&&(pindex==refindex)&&(busid==busId)){
      xdlWireInner(pinst,pindex,busid,wl);
    }
    wl.seekNext();
  }
}

/*=====================================================================
  Wire the vcc net.  VCCs originate from non-prims only.
======================================================================*/
void CLASS::xdlWirePower(){
  cWires* wires = hero->type->pwires;
  cWireList wl=wires->seekFirst();
  while(wl.exists()){
    int pinst;int pindex;int busid;
    wl.getInc(pinst,pindex,busid);
    if(pinst==0xFF){
      switch(pindex){
        case 0xFF: //vcc
        case 0xFE: //gnd
          xdlNetHeader(0,pindex);
          xdlWireInner(pinst,pindex,0,wl); //ground and vcc are scalars
          fprintf(fout,";\n");
      }
   }
   wl.seekNext();
  }
}
/*=====================================================================
======================================================================*/
void CLASS::xdlWireInner(int pinst,int pindex,int busid,cWireList wl){
  //we should follow all destinations...
  while(!wl.isLast()){
//  while(0xFE!=(pinst=*p++)){
//    pindex=*p++;
    wl.getInc(pinst,pindex,busid);
    cDyn* dyn;
    if(pinst==0xFF)
      dyn=this;
    else
      dyn=psubs[pinst];
    //end wire in wherever
    dyn->xdlWireUpOrDown(pindex,busid);
  }
}
/*=====================================================================
======================================================================*/
void CLASS::xdlHeader(){
  fprintf(fout,"# =======================================================\n");
  fprintf(fout,"# top FPGA Hammer tools 0.1.0\n");
  fprintf(fout,"# =======================================================\n");
  fprintf(fout,"design \"top\" xc3s200ft256-4 v3.2 ,\n");
  fprintf(fout,"  cfg \" \" ;\n") ; 
}

/*=====================================================================
Called to start a net output XDL-style.  
Note: primitives have no buses, all pins are scalars.
======================================================================*/
void CLASS::xdlNetHeader(cCollection*pins,int pindex){
   fputs(" net \"",fout);
   hierName(fout);
   switch(pindex){
     case 255:
       fprintf(fout,"/vcc/%.5d\" vcc,\n",seq++);//attach sequence number
       break;
     case 254:
       fprintf(fout,"/gnd/%.5d\" gnd,\n",seq++);//attach sequence number
       break;
     default:     
       fprintf(fout,"/%s",pins->name[pindex]);      //attach outpin name
       fprintf(fout,"/%.5d\" ,\n  outpin \"",seq++);//attach sequence number
       hierName(fout);
       fprintf(fout,"\" %s ,\n",pins->name[pindex]);
       break;
   }
}
/*=====================================================================
======================================================================*/
void CLASS::xdlNetInpin(cCollection*pins,int pindex){
  fputs("  inpin \"",fout);
  hierName(fout);
  fputs("\" ",fout);
  fputs(pins->name[pindex],fout);
  fputs(" ,\n",fout);
}
