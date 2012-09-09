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
#include "global.h"
#include "cPrim.h"
#include "cDevice.h"
#include "cDyn.h"
#define CLASS cPrim
class cModule;
extern cDevice* pDevice;
CLASS::CLASS(char*nam,int len) 
: cProto(nam,len)
{
//fprintf(stderr,"cPrim:cPrim %d %s\n",len,nam);
  //cfg     =g_hash_table_new(g_str_hash,g_str_equal);
  pDevice->addProto(name,len,this);
  cfgval=new cCollection();
}
CLASS::~CLASS()
{
  //g_hash_table_destroy(cfg);
}


