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
#pragma once
#include "cMultiWireWalker.h"
/******************************************************************************
 cDyn
 
 A dynamic instance created during expansion.
******************************************************************************/
class cProto;
class cSub;
class cWires;
class cDyn {
// data
public:
static FILE* fout;  
  
  cSub *hero;       //reference type of this dyn;
  cDyn *dad;
  cDyn** psubs;       //tree
  int psubcnt;        //number of children
  
  cDatum* loc;         //final location
  cCollection* pparams; //actual parameters...
private:
  static U32 seq;       //wire sequence number

public:
  cDyn(cSub* hero, cDyn* const dad);
  ~cDyn();
  void expand();
  void dump(FILE*f);
//  void hierName();
  void hierName(FILE*f);
  void place();
  //xdl-specific, see cDynXdl.cpp
  void xdlWires1();
  void xdlDefs();   //output definitions to xdl...
  void xdlHeader();
  //verilog-specific see cDynVlog.cpp
  void verilog();
protected:
  char* expandFile(const char* filename);
  void errorIn(const char* from);
  void error(int errnox);
  void errTopModuleNotConnected(const char* from, const char*pin);
  
  int banglen(char* str,char*fullstr);//length of !...! parameter, str is past !
  cDatum* getLocation();
  int childIndex(cDyn*p);
  //xdl-specific, see cDynXdl.cpp
  void xdlNetHeader(cCollection* pins,int pindex);
  void xdlNetInpin(cCollection* pins,int pindex);
  void xdlStartWires1(cDyn* prim);
  void xdlContinueWire1(cDyn* prim,int pindex,int busid);
  void xdlWireUpOrDown1(int pindex,int busid);
  void xdlWirePower1();
  void xdlWireInner1(int pinst,int pindex,int busid,cMultiWireWalker& walker);
  //verilog-specific, see cDynVerilog.cpp
  void verilogDefs(); 
  bool isPrimitive();
};
// deprecated old-style wiring
//  void xdlContinueWire(cDyn* prim,int pindex,int busid);
//  void xdlStartWires1(cDyn* prim);
//  void xdlWirePower1();
//  void xdlWireInner1(int pinst,int pindex,int busid,cMultiWireWalker& walker);

