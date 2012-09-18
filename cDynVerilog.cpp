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
Verilog output is a little different.  First of all, we need to infer
the BELs and the bel wiring for the primitives.
Wiring is different also - instead of trolling for primitive endpoints,
we start at the top module and trace the wiring from its input/outputs.
======================================================================*/
void CLASS::verilogLoc(){
  fputs("(*LOC=\"",fout);
  loc->outputLoc(fout);
  fputs("\" *)",fout);
}
void CLASS::verilogDefs(){
  if(hero->type->isPrimitive()){
    //A primitive.  Let's output it if we can.
    if(0==strncmp("SLICE",hero->type->name,5)){ //any kind of slice
      //get the cfg for this dyn
      int icfg = pparams->find("cfg");
      if(-1==icfg){
        errorIn("verilogDefs");
        fprintf(stderr,"No cfg parameters in %s\n", hero->name);
        error(-1);
      }
      cCollection* cfg = pparams->data[icfg]->valCfgs;
      // is there a G BEL?
      int i = cfg->find("G");
      if(-1!=i){
        verilogLoc();
        fputs("(* BEL=\"G\"*) //",fout);
        hierName(fout);
        fputs("\n",fout);
        //if the G value is 0x????, it's a lut4
      }


// fprintf(stderr,"cfg is at %p\n",cfg);
// cfg->dump(stderr);
      return;
    }
    errorIn("verilogDefs");
    fprintf(stderr,"Primitive type %s is not supported for verilog\n",
            hero->type->name);
    error(-1);
  } else {
    /*not a primitive, a regular module.  Do not output, continue
      decomposing it */
      int i;
      for(i=0;i<psubcnt;i++){
        (psubs[i])->verilogDefs();
    }
  }

  
}
