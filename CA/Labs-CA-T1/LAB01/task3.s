.text
.globl main
main:
    li x20, 5 # a
    li x21, 0 # b
    addi x20, x21, 32 # a = b + 32
    add x5, x20, x21 # a + b
    li x6, 5
    sub x22, x5, x6 # d = x5 - x6
    sub x23, x20, x22 # a - d
    sub x24, x21, x20 # b - a
    add x25, x23, x24 # a-d + b-a
    add x19, x25, x22 # x19 = e
    add x26, x22, x19 # d + e

    add x19, x5, x26  # e
    

end:
    j end