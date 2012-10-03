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
#include "cWireList.h"
/******************************************************************************
 cWires
 
 A datastructure containing a list of wires...
 3 bytes encode an endpoint... see WireList
 ******************************************************************************/
class cModule;

#define CWIRES_CAP_INIT 4096 
class cWires {
public:
  cWires(cModule* mod);
  ~cWires();
  void solidify();
  void add(int inst,int port,int busid);
  void close();
  cWireList seekFirst();
//  void seekNext(cWireList start);
//  void seekInst(cWireList start,int inst); //inst is an index
//-----------------------------------
private:    
  U8* buf;            //malloc'd buffer containing data
  U32 index;          //pointer used during construction
  U32 max;
#ifdef DEBUG
public:
  cModule* bugModule;
  void dump(FILE*f);
  void dumpEndpoint(FILE*f,int inst,int port);
  
#endif
};
