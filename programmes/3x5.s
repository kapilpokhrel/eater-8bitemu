top:
	lda x
	sub one
	jc continue
	lda product
	out
	hlt
continue:
	sta x
	lda product
	add y
	sta product
	jmp top
	0
one:1
product:0
x:	3
y:	5
