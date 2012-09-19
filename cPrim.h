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
 Primitive Definition
  PrimDef is the low-level building block of the FPGA. It exports names of pins
******************************************************************************/
#include "cProto.h"
class cModule;
class cPrim : public cProto{
public:
   cPrim(char*name,int len);
  ~cPrim();
  virtual void verilog(FILE*f);

  cCollection* cfgval;    //for validation, cfg names and possible values
private: 
};
