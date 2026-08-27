.text
.globl main

main:
    #a = x10, b = x11, c = x12
    li x10, 0x100 #char arr
    li x11, 0x200 #short arr
    li x12, 0x300 #unsigned char arr
    #The loop will run 3 times
    # i = 0
    lbu x13, 0(x10)        # a[0]
    lh  x14, 0(x11)        # b[0]
    add x15, x13, x14
    sw  x15, 0(x12)        # c[0]
    # i = 1
    #1, 2, 4 is because of multiplication with bytes
    lbu x13, 1(x10)        # a[1]
    lh  x14, 2(x11)        # b[1]
    add x15, x13, x14
    sw  x15, 4(x12)        # c[1]
    # i = 2
    lbu x13, 2(x10)        # a[2]
    lh  x14, 4(x11)        # b[2]
    add x15, x13, x14
    sw  x15, 8(x12)        # c[2]

    # i = 3
    lbu x13, 3(x10)        # a[3]
    lh  x14, 6(x11)        # b[3]
    add x15, x13, x14
    sw  x15, 12(x12)        # c[3]

    end:
        j end