foo:
  movl $3, %r10d
  if:
	cmpl $0, %r10d
	jne  next
	movl $4, %eax
	leave
	ret
  next:
    movl $5, %eax
	leave
    ret
//fim
//0000000000000000 <foo>:
//   0:   41 ba 03 00 00 00       mov    $0x3,%r10d
//
//0000000000000006 <if>:
//   6:   41 83 fa 00             cmp    $0x0,%r10d
//   a:   75 07                   jne    13 <next>
//   c:   b8 04 00 00 00          mov    $0x4,%eax
//  11:   c9                      leaveq
//  12:   c3                      retq
//
//0000000000000013 <next>:
//  13:   b8 05 00 00 00          mov    $0x5,%eax
//  18:   c9                      leaveq
//  19:   c3                      retq