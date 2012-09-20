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
class cParseStream {
public:
  cParseStream();
  ~cParseStream();
  
protected:
   bool reload();
  //ws-related
  bool wsp();
  bool ws(bool required);
  bool enclosedComment();
  bool wsComment();
  void setFile(FILE*f);
//  bool parseComment();
  int cnt();
  int cnt(const char* seps);
  //these ws,cnt, parse, and return cnt()
  int requireWord(const char* str,int len);
  int requireSeparator(const char* caller,const char* seps,char*actual=0); 
  int optionalColon();
  
  int notEnd();
  //these check here for a token. If match, advance.
  //bool tokWire(int len);
//  bool tokHis(int len);
//  bool tokVcc(int len);
  bool tokTo(int len);
  bool tokAnything(const char* str,int parsecnt);

  
  //cDatum* parseLocation();
  cDatum* parseQuotedString();
  cDatum* parseBracedString();
  int     banglen(char*str); //length of !paramname!
  S32 parseLiteral();  //TODO: duplicated in dyn?
  void showtok(); //debugigng
  void validateName(int len);

  void errorIn(const char* from);
  void warningIn(const char* from);
  void error(int xerrno);
  void warning(int xerrno);

//and the data  
  FILE* file;
  int lineno;
  char* ptr;
  char* buf;
};