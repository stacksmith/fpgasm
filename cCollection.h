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

 cCollection
 
 This class is roughly an array of named objects.
 -The array starts in 'builder' mode.  When the count of objects
  is nailed down, the array compacts itself.
  
  Index access is used to traverse wiring, so we have to use a
  fixed array.  If searching becomes too slow we could hashtable
  in addition instead of a stringchunk.
******************************************************************************/
//class cDatum; //global.h always includes cDatumc & Collection.h
class cCollection{
public:
   cCollection(); //for debugging only
   cCollection(int max);
   cCollection(const cCollection&src); //reuse names, point at same data
  ~cCollection();
   void resize(int max);
  int add(const char*str,int len,cDatum* dat);
  int addClone(const char*str,cDatum*dat);
  void solidify();
  int  find(const char*str);          //Neg means not found
  int  find(const char*str,int len);
  char* getName(int i);
  cDatum* getDatum(int i);
  void dump(FILE*f,const char*name);
  void vlogWireDefs(FILE*f,const char*prefix);
  void vlogPinDefs(FILE*f);  
//DATA:
  char** name;
  cDatum** data;
  int size;
#ifdef DEBUG
  int debugmax;
  char* debugname;
  void  setdebugname(char*name,int len);
#endif
private:
  static int indent;
};
