# 1 "src/supervisorTrap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/supervisorTrap.S"

.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN5Riscv16switchToSysStackEv
.extern _ZN5Riscv18switchFromSysStackEv
.extern _ZN5Riscv12checkRunningEv
.extern _ZN5Riscv17checkRunningStackEv
.extern _ZN5Riscv13returnContextEv

.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:

    addi t2,t2,-500
    beq t2,zero,skip1

    mv t1,ra
    call _ZN5Riscv12checkRunningEv
    mv ra,t1
    beq t0,zero,skip1

    mv t1,ra
    call _ZN5Riscv17checkRunningStackEv
    mv ra,t1
    beq t0,zero,skip1

    mv t1,ra
    call _ZN5Riscv13returnContextEv
    mv ra,t1

    sd sp,1*8(t0)
    ld sp,0*8(t0)

skip1:
    addi t2,t2,500


    # push all registers to stack
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr



    call _ZN5Riscv20handleSupervisorTrapEv


    sd a0, 10*8(sp)

    # pop all registers from stack
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    addi t2,t2,-500
    beq t2,zero,skip2

    mv t1,ra
    call _ZN5Riscv12checkRunningEv
    mv ra,t1
    beq t0,zero,skip2

    mv t1,ra
    call _ZN5Riscv17checkRunningStackEv
    mv ra,t1
    beq t0,zero,skip2

    mv t1,ra
    call _ZN5Riscv13returnContextEv
    mv ra,t1

    sd sp,0*8(t0)
    ld sp,1*8(t0)

skip2:
    addi t2,t2,500

    sret
