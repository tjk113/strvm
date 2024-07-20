add r0, 1
loop:
    add r0, 1
    ptn r0
    ptc 10 ; newline
    cmp r0, 3
    jlt loop