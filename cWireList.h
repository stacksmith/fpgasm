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
 cWireList
 
 Originally I just used a byte array to represent a list of nets.  But I needed
 more flexibility, so I centralized access to the byte array into this class.
 Encoding: each net consists of two or more endpoints.  Each endpoint in turn
 consists of an INST index and a PIN index, each stored as a byte. Special cases:
 INST
 =====================
  FF   My wire
  FE   End of net
 =====================
  My pins also have special cases:
 =====================
  FF   My vcc
  FE   My gnd.
  
  Finally, the sequence FEFE indicates end of storage.
 
 ******************************************************************************/
// cWireList represents a list of connections, a subset of what is stored in
// cWires.  Originally it was a byte array, but I want more flexibility
struct cWireList{
  U8* wp; //wire pointer.
public:
  cWireList(U8*p);
//  U8 readInc(void){ return *wp++;}
  bool exists(void);
  void getInc(int& inst,int& index,int& busid);
  bool isLast();
  void seekNext();
  void seekInst(int inst);
};
