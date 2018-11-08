LDY #$00
outerloop:
LDX #$00
loop:
STX $c000,Y
INX
INY
TXA
CMP #$07
BNE loop
LDX #$00
TYA
CMP #$FF
BNE outerloop


