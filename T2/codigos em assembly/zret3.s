foo:
  if:
	cmpl $0, %edi
	jne  next
	movl $4, %eax
	leave
	ret
  next:
    movl $5, %eax
	leave
    ret