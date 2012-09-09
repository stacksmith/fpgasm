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

 cCollection
 
 This class is roughly an array of named objects.
 -The array starts in 'builder' mode.  When the count of objects
  is nailed down, the array compacts itself.
  
  Index access is used to traverse wiring, so we have to use a
  fixed array.  If searching becomes too slow we could hashtable
  in addition instead of a stringchunk.
******************************************************************************/
#define TYPE_INT 0
#define TYPE_STR 1
#define TYPE_PIN 2 //direction, buswidth
#define TYPE_SUB 3 //instances declared in a module
#define TYPE_LOCXY 4
#define TYPE_LOCABS 5
#define TYPE_GOODCFGS 6
#define TYPE_PROTO 7
#define TYPE_TILE 8 //used by device to track tiles
#define TYPE_PARSUB 9 //parameter substitution in sub
#define TYPE_CFGS 10
#define TYPE_UNREALIZED 255 //string that must be handled at expansion.
class cSub;
class cProto;  
class cCollection;
class cDatum{
public:
  U8 type;
  union {
    unsigned int valInt;
    char* valStr;
    cSub* valSub;
    cProto* valProto;
    cCollection* valCfgs;
    struct {
      U16 valX;
      U16 valY;
    };
    struct {
      U8 pinDir;
      U8 pinBusWidth;
    } ;
  };
  // give factories access to data
 //friend cDatum* newStr(const char* str,int len);
  //friend cDatum* newInt(const char* str,int len);
public:
  cDatum(U8 type);
  ~cDatum();
  void dump(FILE*f);
  static cDatum* newStr(const char* str,int len);
  static cDatum* newInt(int val);
  static cDatum* newPin(int dir,int bwidth);
  static cDatum* newSub(cSub* val);
  static cDatum* newLocXY(int x,int y);
  static cDatum* newLocAbs(const char* str,int len);
  static cDatum* newGoodCfgs(const char* str,int len);
  static cDatum* newProto(cProto* val);
  static cDatum* newTile(const char* val,int len);
  static cDatum* newParSub(const char* val,int len);
  static cDatum* newUnrealized(const char* str,int len);
  
  char* outputLoc(); //proper SLICE_X..Y.. or abs
  void  realize();
  void  realize1();
private:
  static char*buf;   //a buffer for outputting location strings
  char*   parseLiteral(char*start,S32*presult);
  
};
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
