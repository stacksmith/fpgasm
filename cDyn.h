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
#include "cWires.h"
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
  void xdlDefs();   //output definitions to xdl...
  void xdlNets();
  void dump(FILE*f);
//  void hierName();
  void hierName(FILE*f);
  void place();
  void wire();
  void xdlHeader();
  void xdlNetHeader(cCollection* pins,int pindex);
  void xdlNetInpin(cCollection* pins,int pindex);
private:
  char* expandFile(const char* filename);
  void errorIn(const char* from);
  void xerror(int errnox);
  int banglen(char* str,char*fullstr);//length of !...! parameter, str is past !
  cDatum* getLocation();
  void startWires(cDyn* prim);
  void continueWire(cDyn* prim,int pindex,int busid);
  void wireUpOrDown(int pindex,int busid);
  void wirePower();
  void wireInner(int pinst,int pindex,int busid,cWireList wl);
  int childIndex(cDyn*p);
};
