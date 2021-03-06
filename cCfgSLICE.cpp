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
#include "cCfgSLICE.h"
#include "cDyn.h"
#define CLASS cCfgSLICE
void CLASS::verilogLoc(FILE* fout,cDatum* loc){
  fputs("(*LOC=\"",fout);
  loc->outputLoc(fout);
  fputs("\" *)",fout);
}
void CLASS::verilog(FILE* fout,cDyn* dyn){
  // is there a G BEL?
  int i = find("G");
  if(-1!=i){
    verilogLoc(fout,dyn->loc);
    fputs("(* BEL=\"G\"*) //",fout);
    dyn->hierName(fout);
    fputs("\n",fout);
    char* gstring=data[i]->valStr;
fputs(gstring,stderr);
    if(0==strncmp(gstring,"#LUT:D=0x",9)) {
      //processing a lut. 0x????, means use all 4 inputs.
      fprintf(fout,"LUT4 #(.INIT(16'h%s)) ",gstring+9);
      dyn->hierName(fout);
    }
  } else {
    fprintf(stderr,"not yet finished\n");
  }
}
