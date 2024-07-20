mov r0, rz
add r0, 5
sub r0, 5
jz yes
jnz no
yes:
    ptc 89 ; Y
    ptc 40 ; (
    ptn r0
    ptc 41 ; )
    hlt
no:
    ptc 78 ; N
    ptc 40 ; (
    ptn r0
    ptc 41 ; )
    hlt