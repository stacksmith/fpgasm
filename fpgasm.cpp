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
#include "cDevice.h"
#include "cModule.h"
#include "cParse.h"
#include "cDyn.h"
#include "cSub.h"
#include "stdlib.h"

cDevice *pDevice;
#include "malloc.h"
int main(int argc,char** argv){
  FILE* fout = stdout;

//  ;GMainLoop*  gmloop;
//  gmloop = g_main_loop_new (NULL, TRUE);
//  
//  g_main_loop_run(gmloop);
  fprintf(stderr,"\nFPGAsm 0.1 (c) 2012 Victor Yurkovsky \n");
  if(argc<2){
    printf("Usage: fpgasm <filename.ham> \n");
    exit(1);
  }
  if(argc==3){
    fout = fopen(argv[2],"w");
    if(!fout){
      fprintf(stderr,"Unable to open %s for output\n",argv[2]);
      exit(1);
    }
  }
  pDevice = 0;
  
//  pDevice = new cDevice();
//  pDevice->initialize();
//  pDevice->parse_xdlrc("xc3s200ft256.xdlrc");
//  pDevice->listProtos();
  FILE* fin = (fopen(argv[1],"r"));
  if(!fin){
    fprintf(stderr,"Unable to open %s for input\n",argv[1]);
    exit(1);
  }
  cParse* parser = new cParse();
  try {
    parser->parse(fin);
  }
  catch(int i){
//    fprintf(stderr,"CAUGHT %d\n",i);
    exit(i);
  }
//fprintf(stderr,"parse() completed\n");
  cModule* root = (cModule*)pDevice->findProto((char*)"top",3); 
//dynamic tree...  First create a fake sub for the root
  cSub* subroot = new cSub(root->name,strlen(root->name));
  subroot->type=root;
  //subroot->loc=cDatum::newLocXY(0,0);
  subroot->pparams=0;
  //expand
  cDyn*dynroot=new cDyn(subroot,0);
  dynroot->fout = fout;

//fprintf(stderr,"expand started\n");
  dynroot->expand();
//fprintf(stderr,"expand completed\n");
  dynroot->place();
  dynroot->xdlHeader();

  dynroot->xdlDefs();
  dynroot->xdlWire();

  return 0;
}

 void q(void*ptr){
   U8*p=(U8*)ptr;
fprintf(stderr,"%p ",p);   
fprintf(stderr,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
          *p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6),*(p+7),*(p+8),*(p+9),*(p+10),*(p+11),*(p+12),*(p+13),*(p+14),*(p+15));

}
