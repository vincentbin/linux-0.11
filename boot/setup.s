.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

SETUPSEG = 0x9020			! setup starts here

entry _start
_start:
	mov ax, #SETUPSEG   ! 修改这里就行
	mov es, ax
	! Print some inane message
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#26
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10
	
msg1:
	.byte 13,10
	.ascii "Now we are in setup."
	.byte 13,10,13,10

.text
endtext:
.data
enddata:
.bss
endbss:
