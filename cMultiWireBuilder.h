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
#include "cMultiWire.h"
#define CMULTIWIRE_CAP_INIT 4096 
class cMultiWireBuilder : public cMultiWire  {
public:
  cMultiWireBuilder();
  ~cMultiWireBuilder();
  void add(int inst,int port,int busid);
  void solidify();
  void stop();
  void end();
private:
  U32 max;
  U32 index;
};
