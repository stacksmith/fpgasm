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
 A quark is a special part of a module.  Quarks correspond to Verilog primitive
 constructs that may be instantiated in Verilog, and is used in conversion to
 verilog.  Modules using quarks instead of instantiating primitives can be
 converted to verilog.
 A quark is instantiated inside a module, and merges its own cfg data and pins
 with those of the module.
******************************************************************************/
#pragma once
#include "cProto.h"
class cQuark: public cProto{
public:
   cQuark(char*name,int size);
  ~cQuark();
  virtual void verilog(FILE*f);
private:
};
