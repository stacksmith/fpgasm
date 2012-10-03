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
#include "cParseStream.h"
#define CLASS cParseStream
#include <ctype.h>
CLASS::CLASS(){
  file=0;
  buf=(char*)malloc(1024);
  ptr=buf;
  *ptr=0;		//will force a reload.
  lineno=0;
}
CLASS::~CLASS(){
  if(file)
    fclose(file);
  free(buf);
}
void CLASS::setFile(FILE*f){
  file=f;
}

/******************************************************************************
  parseQuotedString.  
  Enter with ptr at "; return with ptr past ".
                                                                                 
******************************************************************************/ 
cDatum* CLASS::parseQuotedString(){
  ptr++; //ptr is past "
  int len=0;
  char c;
  char* buf=(char*)malloc(0x10000);
  char*dest=buf;
  // Now count the characters until "
  while('"' != (c=*ptr++)){
    len++;
    *dest++=c;
    switch(c){
      case 0x0A:
      case 0x0D: reload(); break;
    }
  }
  *dest++=0;len++;
  cDatum* ret = cDatum::newStr(buf,len);
  free(buf);
  return ret;
}
cDatum* CLASS::parseBracedString(){
  ptr++; //ptr is past {
  int len=0;
  char c;
  char* buf=(char*)malloc(0x10000);
  char*dest=buf;
  // Now count the characters until "
  while('}' != (c=*ptr++)){
    len++;
    *dest++=c;
    switch(c){
      case 0x0A:
      case 0x0D: reload(); break;
    }
  }
  *dest++=0;len++;
  cDatum* ret = cDatum::newStr(buf,len);
  free(buf);
  return ret;
}

/**************************************************************************
 SUBROUTINES 
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

bool CLASS::reload(){
//fprintf(stderr,"(reloaded)");
  ptr=buf;
  *ptr=0;
  lineno++;
  char*ret=fgets(buf,1024,file);
 return 0!=ret;
}
/******************************************************************************
     wsp is whitespace primitive.  It does not handle comments                                                                    
    called by include as well...                                                                      
******************************************************************************/ 
bool CLASS::wsp(){
  while(true){
    char c=*ptr;
    if(!c){
      if(!reload()) return false;
      c=*ptr;
    }
    if(!isspace(c)) return true;
    ptr++;
  }
}
/******************************************************************************
******************************************************************************/ 
bool CLASS::enclosedComment(){
  //for error reporting, keep track of opening position
  int errline=lineno;
  char* errsrc = strdup(buf);
  char* errptr=ptr;
  while(true){
    char* p = strstr(ptr,"*/");
    if(p) {
      ptr= p+2;
      return true;
    }
    if(!reload()){
      //even though the error occurs later, the source is here:
      memcpy(buf,errsrc,sizeof(buf));
      ptr=errptr;
      lineno=errline;
      errorIn("enclosedComment()");
      fprintf(stderr,"where is the closing */\n");
      error(1);
    }
  }
}

/******************************************************************************
    wsComment        
    process a comment, return isComment                                                         
******************************************************************************/ 

bool CLASS::wsComment(){
 if(0==strncmp(ptr,"/*",2)) {
    enclosedComment(); //it updates ptr to past */
    return true;
  }
  if(0==strncmp(ptr,"//",2)) {
    /* handle // comment to eol by reloading */
    if(!reload()){ 
      errorIn("wsComment()");
      fprintf(stderr,"Out of source in // comment");
      error(1);
    }
    //reload succeeded.
    return true;
  }
  // not a comment.
  return false;
}
/******************************************************************************
    ws and comments...
    return true if source ok, false if ran out.        
   
Note: calling ws(FALSE) and checking for false is a way to see if we ran out
of soruce in an include file, for instance. 
******************************************************************************/ 
bool CLASS::ws(bool required){
  do{
    if(!wsp()) {
      if(required){
        errorIn("ws()");
        fprintf(stderr,"Out of source");
        error(1);
      }else
        return false;          //end at ws
    }
    
    // After skipping space, check for comment. If comment, skip and
    // repeat.
  } while(wsComment());
  return true;
}

int CLASS::cnt(){
  int size=0;
  char* p = ptr;
  char c=*p;
  if(!c) return 0;
  switch(c){
    case 0:
    case '(':
    case ')':
    case ':':
    case ';':
    case ',':
    case '[':
    case ']':
      return 1;
  }
  
  while(true){
    char c=*p;
    if(isspace(c)) break;
    if(!c) break;
    if('('==c) break;
    if(')'==c) break;
    if(':'==c) break;
    if(';'==c) break;
    if(','==c) break;
    if('/'==c) break;
    if('\''==c) break;
    if('['==c) break;
    if(']'==c) break;
   size++;
    p++;
  }
  return size;
  
}
int CLASS::cnt(const char* seps){
  int ret=0;
  char*p = ptr;
  char c;
  while((c=*p++)){
    if(strchr(seps,c))
      return ret;
    ret++;
  }
  return ret;
  
}

void CLASS::showtok(){
    int size=cnt();
    char word[64];
    memcpy(word,ptr,size);
    word[size]=0;
    fprintf(stderr,"<%s>",word);
}

// require a word to occur at parseptr
int CLASS::requireWord(const char*  str,int len){
  ws(true);
  int i=cnt();
  if(!((i==len) && (0==strncmp(ptr,str,i)))){
    errorIn("requireWord()");
    fprintf(stderr,"'%.*s' is required\n",len,str);
    error(1);
  }
  ptr+=i;
  ws(true);
  return cnt();
}
/******************************************************************************
  returns cnt()                                                                             
******************************************************************************/ 
int CLASS::requireSeparator  (const char*caller,const char* separators,char*actual){
  ws(true);         //preceding whitespace
  //If the next cha acter is a separator in the list...
  char c = *ptr;
  if((strchr(separators,c))){ 
      ptr++;
      ws(true);
      if(actual)
        *actual=c;
      return cnt(); 
  }
  errorIn(caller);
  fprintf(stderr,"Expected a separator (one of '%s'), not '%c'\n",
    separators,c);
  error(1);
  return 0; //just to stop compiler barksf
}
/******************************************************************************
  returns cnt()                                                                             
******************************************************************************/ 
int CLASS::optionalColon(){
  ws(true);         //preceding whitespace
  if(':'==*ptr){
    ptr++;ws(true);
  }
  return cnt(); 
}
/******************************************************************************
                                                                               
******************************************************************************/ 
int CLASS::notEnd(){
  ws(true);
  int size=cnt();
  if((3==size)&&(0==strncasecmp(ptr,"end",3))){
    ptr+=3;
    return 0;
  }
  else
    return size;
}

/******************************************************************************
                                                                               
******************************************************************************/ 
//len is from cnt(), nothing to do with str!
bool CLASS::tokAnything(const char* str,int len){
  ws(true);
  int sl = strlen(str);
  if((len==sl)&&(0==strncasecmp(ptr,str,sl))){
    ptr+=sl;
    ws(false);
    return true;
  }
  return false;
}
/******************************************************************************
 validateName
                                                                                
******************************************************************************/ 
void CLASS::validateName(const char* func,int len){
  // first character must be alpha!
  char*p=ptr;
  char c=*p++;
  if(!isalpha(c)) {
    errorIn("cParseStream::validateName()");
    fprintf(stderr,"%s Identifier must begin with an alpha character, not %c\n",
            func,c);
    error(1);
  }
/*  int i;
  for(i=1;i<len;i++){
    c=*p++;
    if('_'==c) continue;
    if(!isalnum(c)){
      ptr=p-1;
      errorIn("validateName()");
      fprintf(stderr,"Identifier must contain with an alphanumeric character, not %c\n",
            c);
     error(1);
    }
  }
  */
  if((4==len)&&(0==strncmp(ptr,"wire",4))){
      errorIn("validateName()");
      fprintf(stderr,"Identifier must not be a reserved word. Check for last ;  \n");
     error(1);
  }    
}
/******************************************************************************
 parseLiteral
                                                                                
******************************************************************************/ 
S32 CLASS::parseLiteral(){
  int radix=10;
  long accum=0;
  bool neg=false;
  char c=*ptr;      //check the initial character...
  switch(c){
    case '$':
      radix=16;
      ptr++;
      break;
    case '%':
      radix=2;
      ptr++;
      break;
    case '-':
      neg=true;
      ptr++;
      break;
    default:
      radix=10;
      break;
  }
  while(true){
    c=*ptr++;
    if('_'==c) continue;
    c-='0';
    if(c<0)  break;
    if(c>9) {
      if(c<11)  //take care of : to @
        break;
      c-=7;
      if(c<0) {
        break;
      }
    }
    if(c>radix) {
      break;
//      ptr--;
//      errorIn("parseLiteral()");
//      fprintf(stderr,"invalid digit\n");
//      error(1);
    }
    accum = (accum*radix)+c;
  }
  ptr--;
  if(neg) accum = 0-accum;
  return accum;
}

/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::errorIn(const char* from){
  fprintf(stderr,"-------------------------------------------------------\n");
  fprintf(stderr,"Error in function cParse::%s\n",from);
   fprintf(stderr,"Error occured in line %d:\n",lineno);
 
}
void CLASS::warningIn(const char* from){
  fprintf(stderr,"-------------------------------------------------------\n");
  fprintf(stderr,"Warning in function cParse::%s\n",from);
   fprintf(stderr,"Warning occured in line %d:\n",lineno);
 
}
void CLASS::error(int xerrno){
   int spaces = ptr-buf;
  fprintf(stderr,"\n%s",buf);        //print line, complete with cr
   while(spaces--)
    fputc(' ',stderr);
  fputc('^',stderr);
  fputs("\n",stderr);
 fprintf(stderr,"----------------------------------------------------------------------\n");
  throw(xerrno);
}
void CLASS::warning(int xerrno){
   int spaces = ptr-buf;
  fprintf(stderr,"\n%s",buf);        //print line, complete with cr
   while(spaces--)
    fputc(' ',stderr);
  fputc('^',stderr);
  fputs("\n",stderr);
 fprintf(stderr,"----------------------------------------------------------------------\n");
}
