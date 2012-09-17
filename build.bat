REM ******************************************************************************
REM  Copyright 2012 Victor Yurkovsky
REM 
REM     This file is part of FPGAsm
REM 
REM     FPGAsm is free software: you can redistribute it and/or modify
REM     it under the terms of the GNU General Public License as published by
REM     the Free Software Foundation, either version 3 of the License, or
REM     (at your option) any later version.
REM 
REM     FPGAsm is distributed in the hope that it will be useful,
REM     but WITHOUT ANY WARRANTY; without even the implied warranty of
REM     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM     GNU General Public License for more details.
REM 
REM     You should have received a copy of the GNU General Public License
REM     along with FPGAsm.  If not, see <http://www.gnu.org/licenses/>.
REM ******************************************************************************/
 g++ fpgasm.cpp cDatum.cpp cCollection.cpp cProto.cpp cPrim.cpp cDevice.cpp ^
     cModule.cpp cSub.cpp cDyn.cpp cParseStream.cpp cParse.cpp cWires.cpp ^
     cWireList.cpp ^
     -Wall  -fpack-struct -o fpgasm  
REM CHANGE THIS LINE to your preferred PATH-visible directory
REM cp fpgasm ~/Portable/bin
