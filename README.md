Usage:
Provide input file of op codes in hex. For 16 bit opcodes, set the most significant bit to 1. This is a Big Endian CPU.
Mneumonic 	Op Code 	Operation
NOP 		0000 0000  	No Operation
LDAC 		0000 0001 A 	AC<-M[A]
STAC 		0000 0010 A 	M[A]<-AC 
MVAC 		0000 0011  	R<-AC
MOVR 		0000 0100  	AC<-R
JUMP 		0000 0101 A  	GOTO A
JMPZ 		0000 0110 A  	If (Z=1) then GOTO A
JPNZ 		0000 0111 A  	If (Z=0) then GOTO A
JMPC 		0001 0000 A  	If (C=1) then GOTO A
JV 		  0001 0001 A  	If (V=1) then GOTO A
JN 		  0001 0111 A 	If (N=1) then GOTO A
ADD 		0000 1000  	AC<-AC + R, ZCVN set/reset
SUB 		0000 1001  	AC<-AC - R, ZCVN set/reset
INAC 		0000 1010  	AC<-AC + 1, ZCVN set/reset
CLAC 		0000 1011  	AC<-0, Z<-1 and CNV reset
AND 		0000 1100  	AC<-AC bitwise AND R, ZN set/reset
OR 		  0000 1101  	AC<-AC bitwise OR R, ZN set/reset
XOR 		0000 1110  	AC<-AC bitwise XOR R, ZN set/reset
NOT 		0000 1111  	AC<bitwise NOT (AC), ZN set/reset
RL 		  0001 0010  	AC<-AC rotated left one position ZCVN set/reset
RR 		  0001 0011  	AC<-AC rotated right one position ZCVN set/reset
LSL 		0001 0100  	AC<-AC shifted left one position ZCVN set/reset
LSR 		0001 0101  	AC<-AC shifted right one position ZCVN set/reset
MVI 		0001 0110 D  	AC<-D AC loaded with 8 bits following instruction 
HALT 		1111 1111  	Halt Execution


Registers:
AR 16-bit address register which supplies an address to memory
PC 16-bit Program Counter which contains the address of the next instruction to be executed or the address of the next operand of the current instruction.
DR 8-bit data register which receives instructions and data from memory and transfers data to memory.
IR 8-bit instruction register which contains the opcode fetched from memory to be executed.
TR 8-bit temporary register which holds data during instruction execution.

Flags:
Z set if 0
C set if carry
V set if overflow
N set if negative
