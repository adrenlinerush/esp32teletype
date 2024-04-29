PORTB = $7C00
PORTA = $7C01
DDRB = $7C02
DDRA = $7C03

PCR = $700C
IFR = $700D
IER = $700E

KBD = $0200
KBD_WPTR = $0000
KBD_RPTR = $0001
KBD_FLAGS = $0002

RELEASE = %00000001
SHIFT   = %00000010

CURSOR_X = $0003
CURSOR_Y = $0005
FG_COLOR = $0007
BG_COLOR = $0009
TEXT_FLAGS = $000A

BOLD   = %00000001
DIM    = %00000010
ITALIC = %00001000
ULINE  = %00010000
BLINK  = %00100000
INVERT = %01000000

  .org $8000

RESET:
    LDA #$00
    STA TEXT_FLAGS

    LDA #$00 ; set PTR and FLAGS
    STA KBD_FLAGS
    STA KBD_WPTR
    STA KBD_RPTR
    
    LDA #$ff ; Set VIA Port B to output
    STA DDRB

    LDA #$00 ; Set VIA Port A to input
    STA DDRA

    JSR SET_TEXT
    JSR CLS 
    JSR PROMPT

    CLI


LOOP:
    SEI ; Set Interrupt
    LDA KBD_RPTR
    CMP KBD_WPTR
    CLI ; Clear Interrupt
    BNE KEY_PRESSED
    JMP LOOP

KEY_PRESSED:
    LDX KBD_RPTR
    INC KBD_RPTR
    LDA KBD, X
    CMP #$0A ; enter
    BEQ KEY_ENTER
    JMP CONTINUE

KEY_ENTER:
    JSR LFCR
    JSR PROMPT
    ; INC TEXT_FLAGS
    ; JSR SET_TEXT
    JMP LOOP

CONTINUE:  
    JSR ECHO
    JMP LOOP

TEST_BIOS:
    LDA #$34
    STA BG_COLOR
    LDA #$33
    STA FG_COLOR
    STA CURSOR_X
    STA CURSOR_Y
    LDX #$01
    LDA #$32
    STA BG_COLOR, X
    LDA #$31
    STA FG_COLOR, X
    STA CURSOR_X, X
    STA CURSOR_Y, X

    JSR SET_FG_COLOR
    JSR SET_BG_COLOR
    JSR MV_CURSOR

    RTS

IRQ:
    JSR KBD_IRQ
    RTI

KBD_IRQ:
    ; push reigisters onto stack off the registers
    PHA
    TXA
    PHA
    TYA
    PHA
   

    LDX #$03; try delaying so I get correct scan code
    JSR DELAY_REG_X_CYCLES
    
    LDX PORTA
    CPX #$F0 ; Release Code
    BEQ KBD_RELEASE

    CPX #$12 
    BEQ KBD_SHIFT
    CPX #$59
    BEQ KBD_SHIFT

    LDA KBD_FLAGS
    AND #SHIFT
    BNE KBD_SHIFTED_KEY

    LDY KEYMAP, X
    JMP KBD_VALID

KBD_SHIFTED_KEY:
    LDY KEYMAP_SHIFTED, X
    
KBD_VALID:
    ; exit if invalid scancode
    CPY #$00 ; Unknown
    BEQ KBD_EXIT

    LDA KBD_FLAGS
    AND #RELEASE
    BEQ KBD_PUSHKEY ; if not releasing Push

KBD_CLR_RELEASE:
    LDA KBD_FLAGS
    EOR #RELEASE
    STA KBD_FLAGS

    JMP KBD_EXIT

KBD_PUSHKEY:
    LDX KBD_WPTR
    TYA
    STA KBD, X
    INC KBD_WPTR
    JMP KBD_EXIT

KBD_SHIFT:
    LDA KBD_FLAGS
    AND #SHIFT
    BNE KBD_SHIFT_UP

KBD_SHIFT_DOWN:
    LDA KBD_FLAGS
    ORA #SHIFT
    STA KBD_FLAGS
   
    JMP KBD_EXIT

KBD_SHIFT_UP:
    LDA KBD_FLAGS
    EOR #SHIFT
    STA KBD_FLAGS

    JMP KBD_CLR_RELEASE

    
KBD_RELEASE:
    LDA KBD_FLAGS
    ORA #RELEASE
    STA KBD_FLAGS

KBD_EXIT:
    ; restore regiters before returning
    PLA
    TAY
    PLA
    TAX
    PLA
    RTS

NMI:
    RTI

MV_CURSOR:
    JSR SEND_ESC_SEQ
    LDA CURSOR_X 
    JSR ECHO
    LDX #$01
    LDA CURSOR_X, X 
    JSR ECHO
    LDA #$3B ; ;
    JSR ECHO
    LDA CURSOR_Y 
    JSR ECHO
    LDX #$01
    LDA CURSOR_Y, X 
    JSR ECHO
    LDA #$48 ; H
    JSR ECHO

    RTS


SET_FG_COLOR:
    JSR SEND_ESC_SEQ

    LDA FG_COLOR 
    JSR ECHO
    LDX #$01
    LDA FG_COLOR, X 
    JSR ECHO

    LDA #$6D
    JSR ECHO
  
    RTS

SET_BG_COLOR:
    JSR SEND_ESC_SEQ

    LDA BG_COLOR 
    JSR ECHO
    LDX #$01
    LDA BG_COLOR, X 
    JSR ECHO

    LDA #$6D ; m
    JSR ECHO
  
    RTS

SET_TEXT:
    LDA TEXT_FLAGS
    AND #BOLD
    BNE SET_BOLD
   
    LDX #$32
    JSR UNSET_TEXT_OPTION
        
    JMP ST_DIM

SET_BOLD:
    LDX #$31
    JSR SET_TEXT_OPTION


ST_DIM:
    LDX #$32
    LDA TEXT_FLAGS
    AND #DIM
    BNE SET_DIM
   
    JSR UNSET_TEXT_OPTION
        
    JMP ST_ITALIC

SET_DIM:
    JSR SET_TEXT_OPTION

ST_ITALIC:
    LDX #$33
    LDA TEXT_FLAGS
    AND #ITALIC
    BNE SET_ITALIC
   
    JSR UNSET_TEXT_OPTION
        
    JMP ST_ULINE

SET_ITALIC:
    JSR SET_TEXT_OPTION

ST_ULINE:
    LDX #$34
    LDA TEXT_FLAGS
    AND #ULINE
    BNE SET_ULINE
   
    JSR UNSET_TEXT_OPTION
        
    JMP ST_BLINK

SET_ULINE:
    JSR SET_TEXT_OPTION

ST_BLINK:
    LDX #$35
    LDA TEXT_FLAGS
    AND #BLINK
    BNE SET_BLINK
   
    JSR UNSET_TEXT_OPTION
        
    JMP ST_INVERT

SET_BLINK:
    JSR SET_TEXT_OPTION

ST_INVERT:
    LDX #$37
    LDA TEXT_FLAGS
    AND #INVERT
    BNE SET_INVERT
   
    JSR UNSET_TEXT_OPTION
        
    JMP ST_RETURN

SET_INVERT:
    JSR SET_TEXT_OPTION
    
ST_RETURN:
    RTS

SET_TEXT_OPTION:
    JSR SEND_ESC_SEQ
    TXA
    JSR ECHO
    LDA #$6D ; m
    JSR ECHO
  
    RTS

UNSET_TEXT_OPTION:
    JSR SEND_ESC_SEQ
    LDA #$32 ; 2
    JSR ECHO
    TXA
    JSR ECHO
    LDA #$6D ; m
    JSR ECHO

    RTS

SEND_ESC_SEQ:
    LDA #$1B ; Escape
    JSR ECHO
    LDA #$5B ; [
    JSR ECHO
  

ECHO:
    STA PORTB
    LDA #$00
    STA PORTB
    RTS

LFCR:
    LDA #$0A ; New Line
    JSR ECHO
    LDA #$0D ; Cairrage Return
    JSR ECHO
   
    RTS

CLS:
    JSR SEND_ESC_SEQ
    LDA #$32 ; 2
    JSR ECHO
    LDA #$4A ; J
    JSR ECHO

    JSR SEND_ESC_SEQ
    LDA #$48 ; H
    JSR ECHO

    RTS

PROMPT:
    LDA #$3E ; >
    JSR ECHO
 
    LDA #$20
    JSR ECHO

    RTS

DELAY_REG_X_CYCLES:
    DEX
    BNE DELAY_REG_X_CYCLES
    RTS

  .org $fd00
KEYMAP:
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$09,"`",$00
  .byte $00,$00,$00,$00,$00,"q1",$00,$00,$00,"zsaw2",$00
  .byte $00,"cxde43",$00,$00," vftr5",$00
  .byte $00,"nbhgy6",$00,$00,$00,"mju78",$00
  .byte $00,",kio09",$00,$00,"./l;p-",$00
  .byte $00,$00,"'",$00,"[=",$00,$00,$00,$00,$0A,"]",$00,$5C,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$08,$00,$00,"1",$00,"47",$00,$00,$00 ; Number Pad + BSP Fix Special Keys
  .byte "0.2568",$1b,$00,$00,"+3-*9",$00,$00                         ; Number Pad
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
KEYMAP_SHIFTED:
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$09,"~",$00
  .byte $00,$00,$00,$00,$00,"Q!",$00,$00,$00,"ZSAW@",$00
  .byte $00,"CXDE$#",$00,$00," VFTR%",$00
  .byte $00,"NBHGY^",$00,$00,$00,"MJU&*",$00
  .byte $00,"<KIO)(",$00,$00,">?L:P_",$00
  .byte $00,$00,'"',$00,"{+",$00,$00,$00,$00,$0A,"}",$00,"?",$00,$00
  .byte $00,$00,$00,$00,$00,$00,$08,$00,$00,"1",$00,"47",$00,$00,$00 ; Nubmer Pad + BSP Fix Special Keys
  .byte "0.2568",$1b,$00,$00,"+3-*9",$00,$00                         ; Number Pad
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
  .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00

  .org $fffa
  .word NMI
  .word RESET
  .word IRQ
