/*
James Galasso
Based on: https://coed.asee.org/wp-content/uploads/2020/11/6-The-Relatively-Simple-Computer-System-Simulator-A-Visualization-Tool-for-Computer-System-Organization-and-Architecture.pdf
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
JV 		0001 0001 A  	If (V=1) then GOTO A
JN 		0001 0111 A 	If (N=1) then GOTO A
ADD 		0000 1000  	AC<-AC + R, ZCVN set/reset
SUB 		0000 1001  	AC<-AC - R, ZCVN set/reset
INAC 		0000 1010  	AC<-AC + 1, ZCVN set/reset
CLAC 		0000 1011  	AC<-0, Z<-1 and CNV reset
AND 		0000 1100  	AC<-AC bitwise AND R, ZN set/reset
OR 		0000 1101  	AC<-AC bitwise OR R, ZN set/reset
XOR 		0000 1110  	AC<-AC bitwise XOR R, ZN set/reset
NOT 		0000 1111  	AC<bitwise NOT (AC), ZN set/reset
RL 		0001 0010  	AC<-AC rotated left one position ZCVN set/reset
RR 		0001 0011  	AC<-AC rotated right one position ZCVN set/reset
LSL 		0001 0100  	AC<-AC shifted left one position ZCVN set/reset
LSR 		0001 0101  	AC<-AC shifted right one position ZCVN set/reset
MVI 		0001 0110 D  	AC<-D AC loaded with 8 bits following instruction 
HALT 		1111 1111  	Halt Execution
*/
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>

#define Z 1 //least significant bit
#define C 2
#define V 4
#define N 8 // most significant bit
#define MEMORY 65536

using namespace std;

/*global declarations for registers & memory
AR 16-bit address register which supplies an address to memory
PC 16-bit Program Counter which contains the address of the next instruction to be executed or the address of the next operand of the current instruction.
DR 8-bit data register which receives instructions and data from memory and transfers data to memory.
IR 8-bit instruction register which contains the opcode fetched from memory to be executed.
TR 8-bit temporary register which holds data during instruction execution.
*/
unsigned char M[MEMORY];
short int PC = 0, AR = 0, EXECUTE = 1, hits = 0, misses = 0;
unsigned char DR = 0, TR = 0, IR = 0, FLAG = 0;
unsigned short AC = 0, R = 0;

struct line{
  unsigned char tag;
  unsigned char line[4];
};

line cache[256];

void cacheprinta(unsigned char block, unsigned char tag, unsigned char word, unsigned char cacheline){
  cout << "AR = " << int(AR) << endl;
  cout << "Block = " << int(block) << endl;
  cout << "Before hit/miss cache line = " << int(cacheline) << endl;
  cout << "Tag = " << int(tag) << endl;
  cout << "Misses = " << int(misses) << endl;
  cout << "Hits = " << int(hits) << endl;
  cout << "Cache tag = " << int(cache[cacheline].tag) << endl;
  cout << "Cache data = " << int(cache[cacheline].line[0]) << " "
       << int(cache[cacheline].line[1]) << " "
       << int(cache[cacheline].line[2]) << " "
       << int(cache[cacheline].line[3]) << " " << endl;
}

void cacheprintb(unsigned char tag, unsigned char cacheline){
  cout << "Cache line = " << int(cacheline) << endl;
  cout << "Tag = " << int(tag) << endl;
  cout << "Cache data = " << int(cache[cacheline].line[0]) << " "
       << int(cache[cacheline].line[1]) << " "
       << int(cache[cacheline].line[2]) << " "
       << int(cache[cacheline].line[3]) << " " << endl;
}

unsigned char readcache(){
  unsigned char block = AR & 0xFFFC;
  unsigned char tag = block >> 10;
  unsigned char word = AR & 3;
  unsigned char cacheline = AR & 0x03FC;
  cacheline >>= 2;

  cacheprinta(block, tag, word, cacheline);

  if(cache[cacheline].tag == tag){
    hits++;
    cout << "Hit detected\n";
    cout << "Cache line = " << int(cacheline) << endl;
    cout << "Tag = " << int(tag) << endl;
    //grab the data and return it
  } else {
    misses++;
    cout << "Miss detected\n";
    cache[cacheline].tag = tag;
    for(int i = 0; i < 4; i++){
      cache[cacheline].line[i] = M[block];
      block++;
    }
  }
  
  cacheprintb(tag, cacheline);
  
  return cache[cacheline].line[word];
}

void writecache(){
  unsigned char block = AR & 0xFFFC;
  unsigned char tag = block >> 10;
  unsigned char word = AR & 3;
  unsigned char cacheline = AR & 0x03FC;
  cacheline >>= 2;

  cacheprinta(block, tag, word, cacheline);
  
  if(cache[cacheline].tag == tag){
    cout << "Hit detected\n";
  } else {
    cout << "Miss detected\n";
    cache[cacheline].tag = tag;
  }
  
  for(int i = 0; i < 4; i++){
    cache[cacheline].line[i] = M[block];
    block++;
  }
  cache[cacheline].line[word] = DR;
  M[AR] = DR;
  
  cacheprintb(tag, cacheline);
}

//function for fetch cycle
void fetch(){
  //fetch1
  AR = PC;
  cout << "Fetch1\nAR = " << int(AR) << "\nPC = " << int(PC) << endl;

  //fetch2
  DR = readcache();
  PC++;
  cout << "Fetch2\nDR = " << int(DR) << "\nPC = " << int(PC) << endl;

  //fetch3
  IR = DR;
  AR = PC;
  cout << "Fetch3\nIR = " << int(IR) << "\nAR = " << int(AR) << endl << endl;
}

//function for universal prints during execute cycle
void eprint(){
  cout << "Registers after instruction:\n";
  cout << "ACE = " << AC << endl;
  cout << "RE = " << R << endl;
  cout << "FLAG = " << int(FLAG) << endl;
  cout << "AR = " << int(AR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "Total hits = " << int(hits) << endl;
  cout << "Total misses = " << int(misses) << endl;
  cout << "---------------\n";
}

//no operation
void nop(){
  cout << "NOP\n";
  eprint();
}

//load AC
void ldac(){
  cout << "LDAC\n";

  //LDAC1
  //high order of address into DR
  DR = readcache();
  PC++;
  AR++;
  cout << "LDAC1\n";
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //LDAC2
  //low order of address into DR
  TR = DR;
  DR = readcache();
  PC++;
  cout << "LDAC2\n";
  cout << "TR = " << int(TR) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //LDAC3
  //AR = TR,DR
  AR = 0;
  AR = ((short(TR) << 8) | AR)| DR;
  cout << "LDAC3\nAR = " << int(AR) << endl << endl;

  //LDAC4
  //DR <- M[AR]
  DR = readcache();
  cout << "LDAC4\nDR = " << int(DR) << endl << endl;

  //used to just be AC = DR
  unsigned char tempac = DR;

  AC &= 0xFF00;
  AC |= tempac;
  cout << "LDAC5\nACE = " << AC << endl;
  cout << "AC = " << int(tempac) << endl << endl;
  
  eprint();
}

//store AC
void stac(){
  cout << "STAC\n";
  
  //STAC1
  DR = readcache();
  PC++;
  AR++;
  cout << "STAC1\n";
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //STAC2
  //save high order into DR
  TR = DR;
  DR = readcache();
  PC++;
  cout << "STAC2\n";
  cout << "TR = " << int(TR) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl << endl;

  //STAC3
  //TR <- DR, save low order into DR
  AR = 0;
  AR = ((short(TR) << 8) | AR) | DR;
  cout << "STAC3\n";
  cout << "AR = " << int(AR) << endl;

  unsigned char tempac = AC & 255;
  cout << "AC = " << int(tempac) << endl;
  cout << "ACE = " << AC << endl << endl;

  DR = tempac;
  cout << "STAC4\nDR = " << int(DR) << endl << endl;
  cout << "Before storing data\nAddress = " << int(TR) << int(DR) << endl;
  cout << "M[AR] = " << int(M[AR]) << endl;
  writecache();
  cout << "STAC5\nAfter storing data\nM[AR] = " << int(M[AR]) << " DR = " << int(DR) << endl << endl;
  
  eprint();
}
//move into AC
void mvac(){
  cout << "MVAC\n";

  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;
  
  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;
  tempr = tempac;

  R &= 0xFF00;
  R |= tempr;
  eprint();
}
//move into R
void movr(){
  cout << "MOVR\n";

  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;

  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;

  tempac = tempr;

  AC &= 0xFF00;
  AC |= tempac;
  //AC = R;
  eprint();
}

void jump(){
  cout << "JUMP\n";

  //JUMP1
  //high order of address into DR
  DR = readcache();
  AR++;
  cout << "JUMP1\n";
  cout << "DR = " << int(DR) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //JUMP2
  //low order of address into DR
  TR = DR;
  DR = readcache();
  cout << "JUMP2\n";
  cout << "TR = " << int(TR) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //JUMP3
  //PC = TR,DR
  PC = (short(TR) << 8) | DR;
  cout << "JUMP3\nPC = " << int(PC) << endl << endl;
  
  eprint();
}

void jmpz(){
  cout << "JMPZ\n";

  if(FLAG&Z){
    //JMPZ1
    //high order of address into DR
    DR = readcache();
    AR++;
    cout << "JMPZ1\n";
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JMPZ2
    //low order of address into DR
    TR = DR;
    DR = readcache();
    cout << "JMPZ2\n";
    cout << "TR = " << int(TR) << endl;
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JMPZ3
    //PC = TR,DR
    PC = (short(TR) << 8) | DR;
    cout << "JMPZ3\nPC = " << int(PC) << endl << endl;
  } else {

    PC++;
    cout << "JMPZ1\nPC = " << int(PC) << endl << endl;

    PC++;
    cout << "JMPZ2\nPC = " << int(PC) << endl << endl;
    
  }

  eprint();
}

void jpnz(){
  cout << "JPNZ\n";

  if(!(FLAG&Z)){
    //JNPZ1
    //high order of address into DR
    DR = readcache();
    AR++;
    cout << "JNPZ1\n";
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JNPZ2
    //low order of address into DR
    TR = DR;
    DR = readcache();
    cout << "JNPZ2\n";
    cout << "TR = " << int(TR) << endl;
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JNPZ3
    //PC = TR,DR
    PC = (short(TR) << 8) | DR;
    cout << "JNPZ3\nPC = " << int(PC) << endl << endl;
  } else {

    PC++;
    cout << "JNPZ1\nPC = " << int(PC) << endl << endl;

    PC++;
    cout << "JNPZ2\nPC = " << int(PC) << endl << endl;

  }
  
  eprint();
}

void jmpc(){
  cout << "JMPC\n";
  if(FLAG&C){
    //JMPC1
    //high order of address into DR
    DR = readcache();
    AR++;
    cout << "JMPC1\n";
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JMPC2
    //low order of address into DR
    TR = DR;
    DR = readcache();
    cout << "JMPC2\n";
    cout << "TR = " << int(TR) << endl;
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JMPC3
    //PC = TR,DR
    PC = (short(TR) << 8) | DR;
    cout << "JMPC3\nPC = " << int(PC) << endl << endl;
  } else {

    PC++;
    cout << "JMPC1\nPC = " << int(PC) << endl << endl;

    PC++;
    cout << "JMPC2\nPC = " << int(PC) << endl << endl;

  }

  eprint();
}

void jv(){
  cout << "JV\n";

  if(FLAG&V){
    //JV1
    //high order of address into DR
    DR = readcache();
    AR++;
    cout << "JV1\n";
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JV2
    //low order of address into DR
    TR = DR;
    DR = readcache();
    cout << "JV2\n";
    cout << "TR = " << int(TR) << endl;
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JV3
    //AR = TR,DR
    PC = (short(TR) << 8) | DR;
    cout << "JV3\nPC = " << int(PC) << endl << endl;
  } else {

    PC++;
    cout << "JV1\nPC = " << int(PC) << endl << endl;

    PC++;
    cout << "JV2\nPC = " << int(PC) << endl << endl;

  }

  eprint();
}

void jn(){
  cout << "JN\n";

  if(FLAG&N){
    //JN1
    //high order of address into DR
    DR = readcache();
    AR++;
    cout << "JN1\n";
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JN2
    //low order of address into DR
    TR = DR;
    DR = readcache();
    cout << "JN2\n";
    cout << "TR = " << int(TR) << endl;
    cout << "DR = " << int(DR) << endl;
    cout << "AR = " << int(AR) << endl << endl;

    //JN3
    //AR = TR,DR
    PC = (short(TR) << 8) | DR;
    cout << "JN3\nPC = " << int(PC) << endl << endl;
  } else {

    PC++;
    cout << "JN1\nPC = " << int(PC) << endl << endl;

    PC++;
    cout << "JN2\nPC = " << int(PC) << endl << endl;

  }

  eprint();
}

//add R to AC
void add(){

  cout << "ADD\n";

  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;
  unsigned char tempac1 = tempac;
  unsigned char tempr1 = tempr;
  int result = tempac1+tempr1;
  tempac += tempr;
  /*int result = AC + R;
  AC+=R;*/

  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl;
  cout << "result = " << int(result) << endl;

  //zero flag
  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  //overflow flag
  if(tempac1 <= 127 && tempr1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempr1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(result < 0 || result > 255){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

//subtract R from AC
void sub(){
  cout << "SUB\n";

  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;// used to be ~R+1
  unsigned char tempac1 = tempac;
  unsigned char tempr1 = (~tempr+1);
  int result = tempac1 + tempr1;
  
  //AC+=(~R+1);
  tempac += (~tempr+1);
  

  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;
  //zero flag
  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  //overflow flag
  if(tempac1 <= 127 && tempr1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempr1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(result < 0 || result > 255){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}
//increment AC
void inac(){
  cout << "INAC\n";

  unsigned char tempac = AC & 255;
  cout << "AC = " << int(tempac) << endl;
  
  if(tempac == 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }
  
  //AC becomes 0, set Z
  if(tempac == 255){
    FLAG |= Z;
  } else {
    //reset Z bit to 0
    FLAG &= ~Z;
  }

  //set/reset negative flag
  //shift by 7 to see if MSB will be negative, if so OR with N
  if(tempac+1 >> 7 == 1){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }
  
  //if value would result in a 1 or 0 being carried out of MSB, set accordingly
  //set this to value of MSB
  //If AC == 255, roll it over to 0, set C flag accordingly
  if(tempac == 255){
    FLAG |= C;
    tempac = 0;
  } else {
    FLAG &= ~C;
    tempac++;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

//clear AC
void clac(){
  cout << "CLAC\n";
  unsigned char tempac = 0;
  FLAG = 1; 
  //set Z, reset CNV

  AC &= 0xFF00;
  AC |= tempac;
  cout << "AC = " << int(tempac) << endl;
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rscpu_and(){
  cout << "AND\n";
  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;
  tempac &= tempr;
  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }
  
  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rscpu_or(){
  cout << "OR\n";
  //AC |= R;

  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;
  tempac |= tempr;
  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rscpu_xor(){
  cout << "XOR\n";
  //AC ^= R;
  unsigned char tempac = AC & 255;
  unsigned char tempr = R & 255;
  tempac ^= tempr;
  cout << "AC = " << int(tempac) << endl;
  cout << "R = " << int(tempr) << endl << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rscpu_not(){
  cout << "NOT\n";
  //AC = ~AC;

  unsigned char tempac = AC & 255;
  tempac = ~tempac;
  cout << "AC = " << int(tempac) << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rl(){
  cout << "RL\n";
  unsigned char tempac = AC & 255;
  unsigned char tempac1 = tempac;
  
  //if 1 is in msb, append to the end otherwise behaves as regular shift
  if(tempac & 128){
    tempac = (tempac << 1) | 1;
  } else {
    tempac <<=1;
  }
  cout << "AC = " << int(tempac) << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac1 & 128){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rr(){
  cout << "RR\n";
  unsigned char tempac = AC & 255;
  unsigned char tempac1 = tempac;
  //if 1 is in lsb, append to the end otherwise behaves as regular shift
  if(tempac & 1){
    tempac = (tempac >> 1) | 128;
  } else {
    tempac >>=1;
  }
  cout << "AC = " << int(tempac) << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac1 & 1){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void lsl(){
  cout << "LSL\n";
  unsigned char tempac = AC & 255;
  unsigned char tempac1 = tempac;
  tempac <<= 1;
  
  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac1 & 128){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  cout << "AC = " << int(tempac) << endl;
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void lsr(){
  cout << "LSR\n";
  unsigned char tempac = AC & 255;
  unsigned char tempac1 = tempac;
  tempac >>= 1;
  cout << "AC = " << int(tempac) << endl;

  if(!tempac){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac1 & 1){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac1 <= 127 && tempac > 127){
    FLAG |= V;
  } else if(tempac1 > 127 && tempac <= 127){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(tempac > 127){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  AC &= 0xFF00;
  AC |= tempac;
  
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

//move immediate
void mvi(){
  cout << "MVI\n";

  //MVI1 place contents of M[AR] into DR, increment PC and AR
  DR = readcache();
  PC++;
  AR++;
  cout << "MVI1\n";
  cout << "PC = " << int(PC) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //MVI2 load said 8 bits into AC
  //AC = DR;
  unsigned char tempac = DR;
  cout << "MVI2\nAC = " << int(tempac) << endl;

  AC &= 0xFF00;
  AC |= tempac;
  cout << "AC = " << int(AC) << endl << endl;

  eprint();
}

//HALT execution
void halt(){
  cout << "HALT\n";
  eprint();
  EXECUTE = 0;
}

void ldace(){
  cout << "LDACE\n";

  //LDAC1
  //high order of address into DR
  DR = readcache();
  PC++;
  AR++;
  cout << "LDACE1\n";
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  TR = DR;
  DR = readcache();
  PC++;
  cout << "LDACE2\n";
  cout << "TR = " << int(TR) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //LDAC3
  //AR = TR,DR
  AR = 0;
  AR = ((short(TR) << 8) | AR)| DR;
  cout << "LDACE3\nAR = " << int(AR) << endl << endl;

  //LDAC4
  //DR <- M[AR]
  DR = readcache();
  cout << "LDACE4\nDR = " << int(DR) << endl << endl;

  //LDAC5
  unsigned char tempac = DR;
  AC = tempac;
  AC <<= 8;
  cout << "LDACE5\nACE = " << int(AC) << endl;
  cout << "AC = " << int(tempac) << endl << endl;

  AR++;
  cout << "LDACE6\nAR = " << int(AR) << endl << endl;

  DR = readcache();
  cout << "LDACE7\nDR = " << int(DR) << endl << endl;

  tempac = DR;
  AC |= tempac;
  cout << "LDACE8\nAC = " << int(tempac) << endl;
  eprint();
}

void stace(){
  cout << "STACE\n";

  //STAC1
  DR = readcache();
  PC++;
  AR++;
  cout << "STACE1\n";
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //STAC2
  //save high order into DR
  TR = DR;
  DR = readcache();
  PC++;
  cout << "STACE2\n";
  cout << "TR = " << int(TR) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "PC = " << int(PC) << endl << endl;

  //STAC3
  //TR <- DR, save low order into DR
  AR = 0;
  AR = ((short(TR) << 8) | AR) | DR;
  cout << "STACE3\n";
  cout << "AR = " << int(AR) << endl << endl;

  //STAC4
  //move contents of AC into memory
  unsigned char tempac = AC >> 8;
  DR = tempac;
  cout << "STACE4\n";
  cout << "AC = " << int(tempac) << endl;
  cout << "DR = " << int(DR) << endl << endl;

  //STAC5
  cout << "STACE5\n";
  cout << "High order bits: " << int(tempac) << endl;
  cout << "Before storing data\nAddress = " << int(TR) << int(DR) << endl;
  cout << "M[AR] = " << int(M[AR]) << endl;
  writecache();
  cout << "After storing data\nM[AR] = " << int(M[AR]) << endl << endl;

  AR++;
  cout << "STACE6\nAR = " << int(AR) << endl << endl;

  tempac = (AC << 8) >> 8;
  DR = tempac;
  cout << "STACE7\nAC = " << int(tempac) << endl;
  cout << "DR = " << int(DR) << endl << endl;
  cout << "low order bits: " << int(tempac) << endl;
  cout << "Before storing data\nAddress = " << int(TR) << int(DR) << endl;
  cout << "M[AR] = " << int(M[AR]) << endl;
  cout << "After storing data\nM[AR] = " << int(M[AR]) << endl << endl;

  cout << "STACE8\n";
  writecache();
  
  eprint();
}

void mvace(){
  cout << "MVACE\n";
  R = AC;
  eprint();
}

void movre(){
  cout << "MOVRE\n";
  AC = R;
  eprint();
}

void adde(){
  cout << "ADDE\n";
  
  unsigned short tempac = AC;
  unsigned short tempr = R;
  int result = AC+R;
  AC+=R;

  cout << "tempac = " << tempac << endl;
  cout << "tempr = " << tempr << endl;
  cout << "result = " << result << endl;

  //zero flag
  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  //overflow flag
  if(tempac <= 32767 && tempr <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && tempr > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(result < 0 || result > 0xFFFF){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void sube(){
  cout << "SUBE\n";

  unsigned short tempac = AC;
  unsigned short tempr = ~R+1;
  int result = tempac + tempr;

  AC+=(~R+1);

  //zero flag
  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  //overflow flag
  if(tempac <= 32767 && tempr <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && tempr > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(result < 0 || result > 0xFFFF){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void inace(){
  cout << "INACE\n";
  
  if(AC == 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }
  
  if(AC == 0xFFFF){
    FLAG |= Z;
  } else {
    //reset Z bit to 0
    FLAG &= ~Z;
  }

  if(AC+1 >> 15 == 1){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  if(AC == 0xFFFF){
    FLAG |= C;
    AC = 0;
  } else {
    FLAG &= ~C;
    AC++;
  }
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void clace(){
  cout << "CLACE\n";
  AC = 0;
  FLAG = 1;
  cout << "FLAG = " << int(FLAG) << endl << endl;
  eprint();
}

void ande(){
  cout << "ANDE\n";
  AC &= R;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void ore(){
  cout << "ORE\n";
  AC |= R;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }
  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void xore(){
  cout << "XORE\n";
  AC ^= R;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void note(){
  cout << "NOTE\n";
  AC = ~AC;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rle(){
  cout << "RLE\n";
  unsigned short tempac = AC;
  
  if(AC & 0x8000){
    AC = (AC << 1) | 1;
  } else {
    AC <<=1;
  }

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac & 0x8000){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void rre(){
  cout << "RRE\n";
  unsigned short tempac = AC;
  //if 1 is in lsb, append to the end otherwise behaves as regular shift
  if(AC & 1){
    AC = (AC >> 1) | 0x8000;
  } else {
    AC >>=1;
  }

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac & 1){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void lsle(){
  cout << "LSLE\n";
  unsigned short tempac = AC;
  AC <<= 1;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac & 0x8000){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void lsre(){
  cout << "LSRE\n";
  unsigned short tempac = AC;
  AC >>= 1;

  if(!AC){
    FLAG |= Z;
  } else {
    FLAG &= ~Z;
  }

  if(tempac & 1){
    FLAG |= C;
  } else {
    FLAG &= ~C;
  }

  if(tempac <= 32767 && AC > 32767){
    FLAG |= V;
  } else if(tempac > 32767 && AC <= 32767){
    FLAG |= V;
  } else {
    FLAG &= ~V;
  }

  if(AC > 32767){
    FLAG |= N;
  } else {
    FLAG &= ~N;
  }

  cout << "FLAG = " << int(FLAG) << "\n\n";
  eprint();
}

void mvie(){
  cout << "MVIE\n";
  DR = readcache();
  PC++;
  AR++;
  cout << "MVIE1\n";
  cout << "PC = " << int(PC) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "AR = " << int(AR) << endl << endl;

  //MVI2 load said 8 bits into AC
  unsigned char tempac = DR;
  AC = tempac;
  AC <<= 8;
  cout << "MVIE2\n";
  cout << "ACE = " << AC << endl;
  cout << "AC = " << tempac << endl << endl;

  AR++;
  PC++;
  cout << "MVIE3\nAR = " << int(AR) << endl;
  cout << "PC = " << int(PC) << endl << endl;

  DR = readcache();
  tempac = DR;
  AC &= 0xFF00;
  AC |= tempac;
  cout << "MVIE4\n";
  cout << "DR = " << int(DR) << endl;
  cout << "ACE = " << AC << endl;
  cout << "AC = " << int(tempac) << endl << endl;
  eprint();
}

//function for switching on opcode(decode & execute cycle)
void decode(){
  switch(IR){
  case 0:
    nop();
    break;
  case 1:
    ldac();
    break;
  case 2:
    stac();
    break;
  case 3:
    mvac();
    break;
  case 4:
    movr();
    break;
  case 5:
    jump();
    break;
  case 6:
    jmpz();
    break;
  case 7:
    jpnz();
    break;
  case 16:
    jmpc();
    break;
  case 17:
    jv();
    break;
  case 23:
    jn();
    break;
  case 8:
    add();
    break;
  case 9:
    sub();
    break;
  case 10:
    inac();
    break;
  case 11:
    clac();
    break;
  case 12:
    rscpu_and();
    break;
  case 13:
    rscpu_or();
    break;
  case 14:
    rscpu_xor();
    break;
  case 15:
    rscpu_not();
    break;
  case 18:
    rl();
    break;
  case 19:
    rr();
    break;
  case 20:
    lsl();
    break;
  case 21:
    lsr();
    break;
  case 22:
    mvi();
    break;
  case 129:
    ldace();
    break;
  case 130:
    stace();
    break;
  case 131:
    mvace();
    break;
  case 132:
    movre();
    break;
  case 136:
    adde();
    break;
  case 137:
    sube();
    break;
  case 138:
    inace();
    break;
  case 139:
    clace();
    break;
  case 140:
    ande();
    break;
  case 141:
    ore();
    break;
  case 142:
    xore();
    break;
  case 143:
    note();
    break;
  case 146:
    rle();
    break;
  case 147:
    rre();
    break;
  case 148:
    lsle();
    break;
  case 149:
    lsre();
    break;
  case 150:
    mvie();
    break;
  case 255:
    halt();
    break;
  default:
    cout << "Opcode: " << int(IR) << " invalid\n";
  }
}

int main(){
  cout << "Demonstration of John Carpinelli's RSCPU from COMPUTER SYSTEMS ORGANIZATION & ARCHITECUTRE\n";

  ifstream fin;
  string fn;
  unsigned char nibbles[2];
  unsigned char opcode;
  int i = 0;

  cout << "Enter filename: ";
  cin >> fn;
  cout << "Opening " << fn << endl;
  fin.open(fn.c_str());
  assert(fin.is_open());
  while(1){
    fin >> nibbles;
    if(fin.eof())
      break;
    cout << "Read " << nibbles << endl;
    
    //convert hex to dec on nibbles
    switch(nibbles[0]){
    case '0':
      nibbles[0] = 0;
      break;
    case '1':
      nibbles[0] = 1;
      break;
    case '2':
      nibbles[0] = 2;
      break;
    case '3':
      nibbles[0] = 3;
      break;
    case '4':
      nibbles[0] = 4;
      break;
    case '5':
      nibbles[0] = 5;
      break;
    case '6':
      nibbles[0] = 6;
      break;
    case '7':
      nibbles[0] = 7;
      break;
    case '8':
      nibbles[0] = 8;
      break;
    case '9':
      nibbles[0] = 9;
      break;
    case 'a':
      nibbles[0] = 10;
      break;
    case 'b':
      nibbles[0] = 11;
      break;
    case 'c':
      nibbles[0] = 12;
      break;
    case 'd':
      nibbles[0] = 13;
      break;
    case 'e':
      nibbles[0] = 14;
      break;
    case 'f':
      nibbles[0] = 15;
      break;
    }
    
    switch(nibbles[1]){
    case '0':
      nibbles[1] = 0;
      break;
    case '1':
      nibbles[1] = 1;
      break;
    case '2':
      nibbles[1] = 2;
      break;
    case '3':
      nibbles[1] = 3;
      break;
    case '4':
      nibbles[1] = 4;
      break;
    case '5':
      nibbles[1] = 5;
      break;
    case '6':
      nibbles[1] = 6;
      break;
    case '7':
      nibbles[1] = 7;
      break;
    case '8':
      nibbles[1] = 8;
      break;
    case '9':
      nibbles[1] = 9;
      break;
    case 'a':
      nibbles[1] = 10;
      break;
    case 'b':
      nibbles[1] = 11;
      break;
    case 'c':
      nibbles[1] = 12;
      break;
    case 'd':
      nibbles[1] = 13;
      break;
    case 'e':
      nibbles[1] = 14;
      break;
    case 'f':
      nibbles[1] = 15;
      break;
    }

    //create opcode
    M[i] = nibbles[0]*16;
    M[i++] += nibbles[1];
  }//end file reading loop
  cout << "\n\n";
  fin.close();

  //print initial values of registers
  cout << "FLAG = " << int(FLAG) << endl;
  cout << "AR = " << int(AR) << endl;
  cout << "PC = " << int(PC) << endl;
  cout << "DR = " << int(DR) << endl;
  cout << "IR = " << int(IR) << endl;
  cout << "TR = " << int(TR) << endl;
  cout << "AC = " << int(AC) << endl;
  cout << "R = " << int(R) << endl << endl;

  //initialize cache
  for(int i = 0; i < 256; i++){
    cache[i].tag = 255;
    cache[i].line[0] = cache[i].line[1] = cache[i].line[2] = cache[3].line[0] = 0;
  }
  //begin rscpu
  while(EXECUTE){
    fetch();
    decode();
  }
}
