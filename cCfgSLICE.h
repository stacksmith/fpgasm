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
/******************************************************************************

 cCfgSLICE is a cCollection, a special one containing cfgs of a SLICEL or
 a SLICEM.  cDynVerilog uses it to create the verilog BELs.  We are not creating
 this, just casting an existing collection in cDyn
******************************************************************************/
//class cDatum; //global.h always includes cDatumc & Collection.h
class cCfgSLICE:public cCollection{
private:
   cCfgSLICE(){};
  ~cCollection(){};
public:
};
