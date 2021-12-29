.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

INITSEG  = 0x9000
SYSSEG   = 0x1000
SETUPSEG = 0x9020			! setup starts here

entry _start
_start:
	mov ax, #SETUPSEG
	mov es, ax
	! Print some inane message
	!读入光标位置
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10

	mov	cx,#25
	mov	bp,#msg1
	call print_msg

	!读入光标位置
	mov ax, #INITSEG
	mov ds, ax
	mov ah, #0x03 !功能号0x03表示读光标
	xor bh, bh
	int 0x10
	mov [0], dx !将dx中保存的光标位置保存到ds:0中
	!打印光标位置
	mov cx, #14
	mov bp, #msg2
	call print_msg
	call print_hex
	call print_nl
	!读入内存大小
	mov ah, #0x88
	int 0x15
	mov [2], ax
    !打印扩展内存大小
	mov cx, #15
	mov bp, #msg3
	call print_msg
	mov dx, [2]
	call print_hex
	call print_nl

	!从0x41处拷贝16个字节（第一个磁盘参数表）
	mov ax, #0x000
	mov ds, ax
	lds si, [4*0x41] !第一个磁盘参数表的入口地址
	mov ax, #INITSEG
	mov es, ax
	mov di, #0x0004 !从es:0x0004处开始存储
	mov cx, #0x10
	rep
	movsb

	mov ax, #INITSEG
	mov ds, ax
	mov ax, #SETUPSEG
	mov es, ax

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	!打印柱面数
	mov cx, #8
	mov bp, #msg4
	call print_msg
	mov dx,[4]
	call print_hex
	call print_nl

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	!打印磁头数
	mov cx, #9
	mov bp, #msg5
	call print_msg
	mov dx, [6]
	call print_hex
	call print_nl

	mov ah, #0x03
	xor bh, bh
	int 0x10
	!打印每磁道扇区数
	mov cx, #11
	mov bp, #msg6
	call print_msg
	mov dx, [18]
	call print_hex
	call print_nl
!在这儿进入死循环，不再加载Linux内核
pause:
	jmp pause

!以16进制的方式打印dx中的16位数
print_hex:
	mov cx, #4
print_digit:
	rol dx, #4 !循环右移4位
	mov ax, #0xe0f
	and al, dl
	add al, #0x30 !得到整数的ASCI码
	cmp al, #0x3a
	jl  outp
	add al, #0x07
outp:
	int 0x10
	loop print_digit
	ret

print_nl:
	mov ax, #0xe0d
	int 0x10
	mov al, #0xa
	int 0x10
	ret

!打印bp中指定的message，cx中应该存放字符数
print_msg:
	mov bx, #0x0007
	mov ax, #0x1301
	int 0x10
	ret

msg1:
	.byte 13,10
	.ascii "Now we are in setup"
	.byte 13,10,13,10
msg2:
	.byte 13,10
	.ascii "Cursor Pos: "
msg3:
	.byte 13,10
	.ascii "Memory Size: "
msg4:
	.byte 13,10
	.ascii "Cyls: "
msg5:
    .byte 13,10
	.ascii "Heads: "
msg6:
	.byte 13,10
	.ascii "Sectors: "

.text
endtext:
.data
enddata:
.bss
endbss:
