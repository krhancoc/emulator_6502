loop:
STA $c000,X
INX
TXA
CMP #$07
BNE loop
