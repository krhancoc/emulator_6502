LDA #$00
STA $30
LDA #$c0
STA $31
LDA #$05
STA $c000
LDA #$04
STA $c001
LDA #$05
STA $c002
LDA #$02
STA $c003
LDA #$01
STA $c004
LDA #$03
STA $c005
SORT8:
LDY #$00
STY $32
LDA ($30),Y
TAX
INY
NXTEL:
LDA ($30),Y
INY
CMP ($30),Y
BCC CHKEND
BEQ CHKEND
PHA
LDA ($30),Y
DEY
STA ($30),Y
PLA
INY
STA ($30),Y
LDA #$ff
STA $32
CHKEND:
DEX
BNE NXTEL
BIT $32
BMI SORT8
