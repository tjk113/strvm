str 0, 72
str 1, 101
str 2, 108
str 3, 108
str 4, 111
str 5, 32
str 6, 87
str 7, 111
str 8, 114
str 9, 108
str 10, 100
str 11, 33
mov r0, 0
loop:
    ld r1, r0
    ptc r1
    add r0, 1
    cmp r0, 12
    jlt loop