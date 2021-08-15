start:
	lda magic ;load the content of memroy address 15 into A
	add (magic + 1) ; A = A + [15]
	out
	hlt
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
	0 ;nop
magic:
	0xe
	0x2e