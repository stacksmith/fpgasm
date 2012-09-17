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
 A device contains 
 - a database of named cPrimDefs
******************************************************************************/
class cProto;
class cDevice{
//DATA
private: 
  cCollection protos;
  int lineno;
  char* buf;
  FILE* f;
//CODE
public:
  cDevice();
  ~cDevice();
  void initialize();
  void parse_xdlrc(const char* fname);
  
  char* place(const char* key);
  
  void addProto(char* key,int len,cProto* proto); //add a prim or module
  cProto* findProto(char*str,int len); //find a prim or module
  int idxFindProto(char*str,int len); //find index or -1
  
  void listProtos();
  const char* tileFor( char* primsite); //like SLICE_XY or IOB
private:  
  bool readline();
  void parse_tile(char* buf);
  void parse_defs(char* buf);
  cCollection sites; //map sitename like SLICE_X?Y? to tilename R?C?
  //GHashTable *htPrims;  // map primnames like IOB to cPrim objects
  void errorIn(const char* function);
  void error(U32 xerrno);
  
};
