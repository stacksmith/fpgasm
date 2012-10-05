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
  Cleaning up the wiring.
  Each endpoint consists of a 4-byte structure:
******************************************************************************/
struct sWireEnd {
  U16 inst;        //index of the instance
  U8 pindex;      //index of the pin
  U8 busid;      //start bus index

  void dump(FILE*f);
};
//special cases:
#define INST_MY 0xFFFF
#define INST_STOP 0xFFFE //end of wire
#define INST_END  0xFFFD //end of wire list

class cModule;
class cMultiWireWalker;
class cMultiWire {
public:
  cMultiWire(const cMultiWire& orig);
  ~cMultiWire();
  sWireEnd* getBuf(){return buf;} //for derived constructor use
  void dump(FILE*f);
protected:
  cMultiWire(); //only called by builder
//DATA
  sWireEnd* buf;      //buffer of sWireEnds

};
