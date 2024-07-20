# super tiny register vm
does what it says on the tin
## building
the only dependencies are a C compiler, make, and [fiesta](https://github.com/tjk113/fiesta). make sure the fiesta directory is cloned into the same parent folder as this project, so they are siblings. then you can just `make` this project, and it will also build fiesta if needed.
## instruction set architecture
### registers
<table>
    <tr>
        <th>Register(s)</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>r0-r7</td>
        <td>General purpose registers</td>
    </tr>
    <tr>
        <td>rz</td>
        <td>Register always holding the value 0</td>
    </tr>
    <tr>
        <td>rst</td>
        <td>Status register</td>
    </tr>
    <tr>
        <td>rcmp</td>
        <td>Comparison result register</td>
    </tr>
    <tr>
        <td>pc</td>
        <td>Program counter</td>
    </tr>
    <tr>
        <td>ip</td>
        <td>Instruction pointer</td>
    </tr>
</table>

### instructions
<table>
    <tr>
        <th>Instruction</th>
        <th>Operands</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>nop</td>
        <td>
        <td>No operation</td>
    </tr>
    <tr>
        <td>mov</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Move <em>src</em> into <em>dst</em></td>
    </tr>
    <tr>
        <td>add</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Add <em>src</em> to <em>dst</em>, storing the result in <em>dst</em> (without carry)</td>
    </tr>
    <tr>
        <td>adc</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Add <em>src</em> to <em>dst</em>, storing the result in <em>dst</em> (with carry)</td>
    </tr>
    <tr>
        <td>sub</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Subtract <em>src</em> from <em>dst</em>, storing the result in <em>dst</em> (with carry)</td>
    </tr>
    <tr>
        <td>sbc</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Subtract <em>src</em> from <em>dst</em>, storing the result in <em>dst</em> (without carry)</td>
    </tr>
    <tr>
        <td>clc</td>
        <td></td>
        <td>Clear carry bit</td>
    </tr>
    <tr>
        <td>clv</td>
        <td></td>
        <td>Clear overflow bit</td>
    </tr>
    <tr>
        <td>mul</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Multiply <em>dst</em> by <em>src</em>, storing the result in <em>dst</em></td>
    </tr>
    <tr>
        <td>div</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Divide <em>dst</em> by <em>src</em>, storing the result in <em>dst</em></td>
    </tr>
    <tr>
        <td>neg</td>
        <td><em>dst</em>: reg</td>
        <td>Negate the value in <em>dst</em></td>
    </tr>
    <tr>
        <td>shl</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Shift <em>dst</em> left by <em>src</em> bits</td>
    </tr>
    <tr>
        <td>shr</td>
        <td><em>dst</em>: reg, <em>src</em>: reg/imm</td>
        <td>Shift <em>dst</em> right by <em>src</em> bits</td>
    </tr>
    <tr>
        <td>str</td>
        <td><em>dst_addr</em>: reg/imm, <em>src</em>: reg/imm</td>
        <td>Store value <em>src</em> at <em>dst_addr</em> in memory</td>
    </tr>
    <tr>
        <td>ld</td>
        <td><em>dst</em>: reg, <em>src_addr</em>: reg/imm</td>
        <td>Load value at <em>src_addr</em> in memory into <em>dst</em></td>
    </tr>
    <tr>
        <td>cmp</td>
        <td><em>a</em>: reg/imm, <em>b</em>: reg/imm</td>
        <td>Compare value <em>a</em> to value <em>b</em></td>
    </tr>
    <tr>
        <td>jmp</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em></td>
    </tr>
    <tr>
        <td>jne</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if <em>a</em> was not equal to <em>b</em> in the last comparison</td>
    </tr>
    <tr>
        <td>je</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if <em>a</em> was equal to <em>b</em> in the last comparison</td>
    </tr>
    <tr>
        <td>jgt</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if <em>a</em> was greater than <em>b</em> in the last comparison</td>
    </tr>
    <tr>
        <td>jlt</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if <em>a</em> was less than <em>b</em> in the last comparison</td>
    </tr>
    <tr>
        <td>jnz</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if the result of the last operation was not zero</td>
    </tr>
    <tr>
        <td>jz</td>
        <td><em>dst</em>: lbl</td>
        <td>Jump to the location of label <em>dst</em> if the result of the last operation was zero</td>
    </tr>
    <tr>
        <td>ptc</td>
        <td><em>src</em>: reg/imm</td>
        <td>Print value in <em>src</em> as an ASCII character</td>
    </tr>
    <tr>
        <td>ptn</td>
        <td><em>src</em>: reg/imm</td>
        <td>Print value in <em>src</em> as a signed integer</td>
    </tr>
    <tr>
        <td>ptu</td>
        <td><em>src</em>: reg/imm</td>
        <td>Print value in <em>src</em> as an unsigned integer</td>
    </tr>
</table>