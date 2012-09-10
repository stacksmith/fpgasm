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

//#include <glib.h>
//#include <glib/gprintf.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
typedef  unsigned char U8;
typedef  unsigned short U16;
typedef  unsigned int U32;
typedef  unsigned long U64;
typedef  int S32;

#define DEBUG
#include "cDatum.h"
#include "cCollection.h"

void q(void*p);