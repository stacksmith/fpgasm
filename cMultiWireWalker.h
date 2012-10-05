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
 This class starts out by referencing an existing cMultiWire.  It keeps an 
 index and walks the wire structure.
 ******************************************************************************/
#pragma once
#include "cMultiWire.h"
class cMultiWireWalker : public cMultiWire  {
public:
  cMultiWireWalker(cMultiWire& it);
  ~cMultiWireWalker();
  void getInc(int& inst,int& index,int& busid);
  sWireEnd& getInc();
  bool isStop();
  bool isEnd();
  bool seekNext();  //true=next  false=end.
  bool seekInst(int inst);

private:
  U32 index;
};
