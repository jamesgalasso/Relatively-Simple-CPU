| Mnemonic | Op Code       | Operation                                      |
| -------- | ------------- | ---------------------------------------------- |
| NOP      | `0000 0000`   | No operation                                   |
| LDAC     | `0000 0001 A` | `AC ← M[A]`                                    |
| STAC     | `0000 0010 A` | `M[A] ← AC`                                    |
| MVAC     | `0000 0011`   | `R ← AC`                                       |
| MOVR     | `0000 0100`   | `AC ← R`                                       |
| JUMP     | `0000 0101 A` | GOTO `A`                                       |
| JMPZ     | `0000 0110 A` | If `(Z = 1)` then GOTO `A`                     |
| JPNZ     | `0000 0111 A` | If `(Z = 0)` then GOTO `A`                     |
| JMPC     | `0001 0000 A` | If `(C = 1)` then GOTO `A`                     |
| JV       | `0001 0001 A` | If `(V = 1)` then GOTO `A`                     |
| JN       | `0001 0111 A` | If `(N = 1)` then GOTO `A`                     |
| ADD      | `0000 1000`   | `AC ← AC + R`, ZCVN set/reset                  |
| SUB      | `0000 1001`   | `AC ← AC - R`, ZCVN set/reset                  |
| INAC     | `0000 1010`   | `AC ← AC + 1`, ZCVN set/reset                  |
| CLAC     | `0000 1011`   | `AC ← 0`, `Z ← 1`, `C N V` reset               |
| AND      | `0000 1100`   | `AC ← AC AND R`, ZN set/reset                  |
| OR       | `0000 1101`   | `AC ← AC OR R`, ZN set/reset                   |
| XOR      | `0000 1110`   | `AC ← AC XOR R`, ZN set/reset                  |
| NOT      | `0000 1111`   | `AC ← NOT(AC)`, ZN set/reset                   |
| RL       | `0001 0010`   | Rotate `AC` left one bit, ZCVN set/reset       |
| RR       | `0001 0011`   | Rotate `AC` right one bit, ZCVN set/reset      |
| LSL      | `0001 0100`   | Shift `AC` left one bit, ZCVN set/reset        |
| LSR      | `0001 0101`   | Shift `AC` right one bit, ZCVN set/reset       |
| MVI      | `0001 0110 D` | `AC ← D` (8-bit immediate follows instruction) |
| HALT     | `1111 1111`   | Halt execution                                 |

| Register | Size   | Description                                              |
| -------- | ------ | -------------------------------------------------------- |
| AR       | 16-bit | Address Register – supplies an address to memory         |
| PC       | 16-bit | Program Counter – address of next instruction or operand |
| DR       | 8-bit  | Data Register – transfers data to/from memory            |
| IR       | 8-bit  | Instruction Register – holds fetched opcode              |
| TR       | 8-bit  | Temporary Register – holds data during execution         |

| Flag | Meaning                   |
| ---- | ------------------------- |
| Z    | Set if result is zero     |
| C    | Set if carry occurs       |
| V    | Set if overflow occurs    |
| N    | Set if result is negative |
