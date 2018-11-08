ADC #$04
CMP #$04
BEQ lab
LDA #$AA
JMP lab2
lab:
LDA #$BB
lab2:
ADC #$00
