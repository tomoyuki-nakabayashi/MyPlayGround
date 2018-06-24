; nasmfunc.asm
; TAB=4

section .text
    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags
    GLOBAL  load_gdtr, load_idtr
    GLOBAL  asm_irq_handler21, asm_irq_handler2c
    GLOBAL  load_cr0, store_cr0
    EXTERN  irq_handler21, irq_handler2c

io_hlt:     ; void io_hlt(void)
    HLT
    RET

io_cli:     ; void io_cli(void)
    CLI
    RET

io_sti:     ; void io_sti(void)
    STI
    RET

io_stihlt:  ; void io_stihlt(void)
    STI
    HLT
    RET

io_in8:     ; int io_in8(int port)
    MOV     EDX, [ESP+4]
    MOV     EAX, 0
    IN      AL, DX
    RET

io_in16:    ; int io_in16(int port)
    MOV     EDX, [ESP+4]
    MOV     EAX, 0
    IN      AX, DX
    RET

io_in32:    ; int io_in32(int port)
    MOV     EDX, [ESP+4]
    IN      EAX, DX
    RET

io_out8     ; void io_out8(int port, int data)
    MOV     EDX, [ESP+4]
    MOV     EAX, [ESP+8]
    OUT     DX, AL
    RET

io_out16    ; void io_out16(int port, int data)
    MOV     EDX, [ESP+4]
    MOV     EAX, [ESP+8]
    OUT     DX, AX
    RET

io_out32    ; void io_out32(int port, int data)
    MOV     EDX, [ESP+4]
    MOV     EAX, [ESP+8]
    OUT     DX, EAX
    RET

io_load_eflags: ; int io_load_eflags(void)
    PUSHFD      ; push eflags
    POP     EAX
    RET

io_store_eflags:
    MOV     EAX, [ESP+4]
    PUSH    EAX
    POPFD       ; pop eflags
    RET

load_gdtr:		; void load_gdtr(int limit, int addr)
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

load_idtr:		; void load_idtr(int limit, int addr)
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

load_cr0:       ; int load_cr0(void)
        MOV     EAX, CR0
        RET

store_cr0:      ; void store_cr0(int cr0)
        MOV     EAX, [ESP+4]
        MOV     CR0, EAX
        RET

asm_irq_handler21:  ; void irq_handler21(int *esp)
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX, ESP
        PUSH    EAX
        MOV     AX, SS
        MOV     DS, AX
        MOV     ES, AX
        CALL    irq_handler21
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD


asm_irq_handler2c:  ; void irq_handler2c(int *esp)
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX, ESP
        PUSH    EAX
        MOV     AX, SS
        MOV     DS, AX
        MOV     ES, AX
        CALL    irq_handler2c
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD

