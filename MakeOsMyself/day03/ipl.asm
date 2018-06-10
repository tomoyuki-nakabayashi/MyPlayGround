; hello-os
; TAB=4

CYLS    EQU     10              ; load up to where (CYLinderS)
        ORG     0x7c00          ; メモリ上の開始位置

; ディスクのための記述

        JMP     entry
        DB      0x90
        DB      "HARIBOTE"      ; ブートセレクタの名前を自由にかいていよい  (8Byte)
        DW      512             ; 1セクタの大きさ                           (512にしなければならない)
        DB      1               ; クラスタの大きさ                          (1セクタにしなければならない)
        DW      1               ; FATがどこから始まるか                     (普通は1セクタ目からにする)
        DB      2               ; FATの個数                                 (2にしなければならない)
        DW      224             ; ルートディレクトリ領域の大きさ            (普通は224エントリにする)
        DW      2880            ; このドライブの大きさ                      (2880セクタにしなければならない)
        DB      0xf0            ; メディアタイプ                            (0xf0にしなければならない)
        DW      9               ; FAT領域の長さ                             (9セクタにしなければならない)
        DW      18              ; 1トラックにいくつのセクタがあるか         (18にしなければならない)
        DW      2               ; ヘッドの数                                (2にしなければならない)
        DD      0               ; パーティションを使っていないのでここは必ず0
        DD      2880            ; このドライブの大きさをもう一度書く
        DB      0, 0, 0x29      ; よくわからないけどこの値にしておくといいらしい
        DD      0xffffffff      ; たぶんボリュームシリアル番号
        DB      "HARIBOTE-OS "   ; ディスクの名前                            (11Byte)
        DB      "FAT12   "      ; フォーマットの名前                        (8Byte)
        RESB    18              ; とりあえず18バイト開けておく

; Program Main Body
entry:
        MOV     AX, 0            ; レジスタの初期化
        MOV     SS, AX
        MOV     SP, 0x7c00
        MOV     DS, AX

; load disk
        MOV     AX, 0x0820
        MOV     ES, AX          ; extra segment
        MOV     CH, 0           ; cylinder 0
        MOV     DH, 0           ; head 0
        MOV     CL, 2           ; sector 2

readloop:
        MOV     SI, 0           ; register to count failure.

retry:
        MOV     AH, 0x02        ; read disk
        MOV     AL, 1           ; sector 1
        MOV     BX, 0           ; buffer address 0x0000
        MOV     DL, 0x00        ; A drive
        INT     0x13            ; call disk BIOS
        JNC     next            ; jump if not error

        ADD     SI, 1           ; increment SI
        CMP     SI, 5
        JAE     error           ; SI >= 5 then jump to error

        MOV     AH, 0x00        ; reset
        MOV     DL, 0x00        ; A drive
        INT     0x13            ; reset drive
        JMP     retry

next:
        MOV     AX, ES          ; add 0x200 (512) to address
        ADD     AX, 0x0020
        MOV     ES, AX
        ADD     CL, 1           ; increment cl
        CMP     CL, 18
        JBE     readloop        ; CL <= 18 then jump readloop

        ; side B of the floppy
        MOV     CL, 1           ; reset sector
        ADD     DH, 1           ; reverse HEAD
        CMP     DH, 2
        JB      readloop

        ; next Cylinder
        MOV     DH, 0           ; reset HEAD
        ADD     CH, 1           ; cylinder++
        CMP     CH, CYLS
        JB      readloop        ; CH < CYLS then jump readloop

        MOV     [0x0ff0], CH    ; up to where IPL loads the cylinders.
; jump to haribote.asm
        JMP     0xc200

fin:
        HLT
        JMP     fin

error:
        MOV     SI, msg

putloop:
        MOV     AL, [SI]        ; BYTE (accumulator low)
        ADD     SI, 1           ; increment
        CMP     AL, 0           ; compare (<end msg>)
        JE      fin             ; jump to fin if equal to 0
        MOV     AH, 0x0e        ; AH = 0x0e
        MOV     BX, 15          ; BH = 0, BL = <color code>
        INT     0x10            ; interrupt BIOS
        JMP     putloop

msg:
        DB      0x0a, 0x0a
        DB      "load error"
        DB      0x0a
        DB      0               ; end msg

        RESB    0x7dfe-0x7c00-($-$$)

        DB      0x55, 0xaa
