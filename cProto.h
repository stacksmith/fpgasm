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
class cDyn;
class cModule;
class cWires;
class cProto{
public:
   cProto(char*name,int size);
  ~cProto();
   void dump(FILE*f);
   bool isPrimitive();
  
  // data
  char* name;       //name of this module/primitive...
  cCollection vars;
  //primitives and modules both have a list of valid parameters
  cCollection *paramnames;
  cCollection *psubs;   //names-> submodules...Not relevant to prim.
  cWires* pwires;
  cCollection *pins;    //pin name->buswidth mapping
private:

};
