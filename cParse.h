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
#include "cParseStream.h"
class cModule;
class cSub;
/* While parsing wiring, we generate endpoints.  */
struct sWireEndpoint {
  U8 inst;        //index of the instance
  U8 pindex;      //index of the pin
  U8 busid1;      //start bus index
  U8 busid2;      //end   bus index
};


class cParse: public cParseStream {
public:
  cParse();
  ~cParse();
   void parse(FILE*f);
//data
   cModule* topModule; //keep track of the last module - it is top.
 
protected:
  void parseModules();
  bool handleInclude(int len);
  void validateName(int len,cModule* module);
 
  
  
  void parsePins(cCollection*pins,int dir);
  void parseParamNames(cModule* module);
  cDatum* parseParamData(cModule*module,const char* prepend);
  sWireEndpoint parseWireEndpoint(cModule* module,int idxInst,cSub*pinst);
  void parseWire(cModule* module,int idxInst,cSub*pinst);
  void  parseMergeQuark(cModule* module);
  
  cSub* parseSub(cModule* module,int len);
  void  parseMerge(cModule* module,cSub* pinst);
  void parsePairs(cModule* module,cSub* sub);
  cModule* parseModule();
  bool parseQuark();
//data  
};