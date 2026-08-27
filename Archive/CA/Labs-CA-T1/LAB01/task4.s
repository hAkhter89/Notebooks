.text
.globl main

main:
    li x10, 0x78786464   # x10 = 0x78786464
    li x11, 0xA8A81919   # x11 = 0xA8A81919

    sw x10, 0x100(x0)   # store word in memory at address 0x100
    sw x11, 0x1F0(x0)

    lhu x12, 0x100(x0)
    lh x13, 0x1F0(x0)   # load signed halfword from 0x1F0 into x13

    lb x14, 0x1F0(x0)   # load signed byte from 0x1F0 into x14
end:
    j end
