.text
.globl main

main:
    li      s0, 0x100       # s0 = Base address of array (0x100)

    
    li s4, 12
    li s5, 11
    li s6, 99
    li s7, 56
    li s8, 2

    sw s4, 0(s0)
    sw s5, 4(s0)
    sw s6, 8(s0)
    sw s7, 12(s0)
    sw s8, 16(s0)

    li      s1, 10          # s1 = SIZE = 10

outer_loop:
    li      t0, 0           # t0 = swapped (0 = false)
    li      t1, 1           # t1 = i (1)

inner_loop:
    bge     t1, s1, end_inner   # if (i >= 10) break inner loop

    slli    t2, t1, 2       # t2 = i * 4
    add     t3, s0, t2      # t3 = (0x100 + offset)

    # c[i] and c[i-1]
    lw      t4, 0(t3)       # t4 = c[i]
    lw      t5, -4(t3)      # t5 = c[i-1]

    # - if (c[i-1] > c[i]) -
    ble     t5, t4, no_swap

    # --- Swap Logic ---
    sw      t5, 0(t3)       # c[i] = old c[i-1]
    sw      t4, -4(t3)      # c[i-1] = old c[i]

    li      t0, 1           # swapped = true

no_swap:
    addi    t1, t1, 1       # i++
    j       inner_loop      # Repeat inner loop

end_inner:
    # --- while (swapped); ---
    # If swapped (t0) is not zero, jump back to outer_loop
    bnez    t0, outer_loop

exit: