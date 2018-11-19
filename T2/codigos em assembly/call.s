movl %edi,-24(%rbp)
movl $0, %edi
call func
movl -24(%rbp), %edi

0000000000000000 <.text>:
   0:   89 7d e8                mov    %edi,-0x18(%rbp)
   3:   bf 00 00 00 00          mov    $0x0,%edi
   8:   e8 00 00 00 00          callq  0xd
   d:   8b 7d e8                mov    -0x18(%rbp),%edi
