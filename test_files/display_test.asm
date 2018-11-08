loop:
STX $c000,X
INX
TXA
CMP #$07
BNE loop
CMP #$00
loop2:
STY $c000,Y
INY
TYA
CMP #$07
BNE loop2
