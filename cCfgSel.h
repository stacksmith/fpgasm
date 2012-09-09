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
 This is a list of strings, possible cfgs stored for each element.
******************************************************************************/
#include "cProto.h"
class cPrim : public cProto{
public:
   cPrim(char*name);
  ~cPrim();
  void addCfg(char*key,char*p);
  virtual void xdl();
  void dumpCfgs();

private: 
  static void proc(gpointer,gpointer,gpointer);
  static void proc1(gpointer,gpointer,gpointer);
  GHashTable* cfg; //map element name to a list of possible values.
};
