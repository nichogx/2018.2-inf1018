foo:
  if:
	cmpl $4385, -24(%rbp)
	jne  next
	movl $4, %eax
	leave
	ret
  next:
    movl $5, %eax
	leave
    ret