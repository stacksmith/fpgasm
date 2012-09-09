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
 cSub
 
 An embedded instance description.  These are used as data for
 subs collection.
 Describes how a module invokes another module.
 pins start out same as the type module, but can be extended with 'merge'
******************************************************************************/
class cProto;
class cSub {
public:
  cSub(char*nam,int len);
  ~cSub(); 
  void  setLocation(cDatum* loc);
  cDatum* getLocation();
  void dump(FILE*f);
  void init(cProto* type);
// DATA
  char*name;
  cCollection* pparams;
  cProto* type;
  cCollection* pins;
};
