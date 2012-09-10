fpgasm
======

FPGA assembler!  Create bare-metal FPGA designs without Verilog or VHDL

Coding conventions
C++ is used in a very limited way, mainly as an organizational aid.

Compiling
A simple 'build' script is used to compile and link the entire project (for now).

Invoking
Generally, place the file 'fpgasm' into your working directory.  Make sure that you include the '.xdlrc' file for your architecture.  Invoke with  

 fpgasm sourcefile outfile.xdl

Sometimes it's convenient to omit the output file to see the output on stdout, or redirect it from the command line.

The resultant .xdl file contains a placed but not routed design.   For now, I use Xilinx tools to route, generate the bitstream, and configure the fpga.  For simple test circuits FPGAsm runs in milliseconds; Xilinx tools add about 9 seconds to the turnaround time. 

See the test directory for one way to set up your projects.