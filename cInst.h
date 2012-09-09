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
/******************************************************************************
 cInst
 
 An instantiation
 
  PrimDef is the low-level building block of the FPGA. It exports names of pins
******************************************************************************/
#include "cProto.h"


class cModule : public cProto{
public:
   cModule( char*name,int size);
  ~cModule();
  void addSub(char*name,int len,char* type);
  cProto* getSub(char*name);
  virtual void xdl();
  void dump();
  //cCollection pins;  //from cProto
  cCollection subs;   //enumerated submodules...
  cCollection paramnames;
};
