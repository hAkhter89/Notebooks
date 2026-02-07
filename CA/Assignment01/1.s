# Hassan Akhter // ha10609
# CA - L3 // Husain Parvez


.text
.globl main

main:
 li x8, 0x100 # base address
# initialzing the array values
 li x20, 23
 li x21, 12
 li x22, 5
 li x23, 44
 li x24, 98
 li x25, 53
 li x26, 6
 li x27, 89
 li x28, 32
 li x29, 65

 sw x20, 0(x8)
 sw x21, 4(x8)
 sw x22, 8(x8)
 sw x23, 12(x8)
 sw x24, 16(x8)
 sw x25, 20(x8)
 sw x26, 24(x8)
 sw x27, 28(x8)
 sw x28, 32(x8)
 sw x29, 36(x8)

 li x9, 10 # size = 10

outer_loop:
 li x5, 0 # swapped = false
 li x6, 1 # index = 1

inner_loop:
 bge x6, x9, end_inner # if index > 10, exit loop

 slli x7, x6, 2 # shift left for arr offset
 add x10, x8, x7 # calculated address

 lw x11, 0(x10) # c[i]
 lw x12, -4(x10) # c[i-1]

 ble x12, x11, no_swap # if i-1 < i, dont swap
 #-else
 sw x12, 0(x10) #i = i-1 
 sw x11, -4(x10)#i-1 = i

 li x5, 1 # swapped = true

no_swap:
 addi x6, x6, 1 # i++
 j inner_loop #loopback

end_inner:
 bnez x5, outer_loop

exit:
    j exit
