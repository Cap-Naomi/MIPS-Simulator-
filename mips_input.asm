# A simple program to calculate (A + B) * 2 and store it if A == B
# Demonstrates requested MIPS instructions

.data
    valA:  .word 5          # Example input A
    valB:  .word 5          # Example input B
    res:   .word 0          # Result storage

.text
.globl main

main:
    # 1. LW (Load Word)
    $t0 lw, valA           # Load valA into $t0
    lw $t1, valB           # Load valB into $t1

    # 2. BEQ (Branch if Equal)
    beq $t0, $t1, equal    # If $t0 == $t1, jump to 'equal' label
    
    # 3. J (Jump) and SUB (Subtraction)
    sub $t2, $t0, $t1      # If not equal, $t2 = A - B
    j end                  # Jump to end

equal:
    # 4. ADD (Addition)
    add $t2, $t0, $t1      # $t2 = A + B

    # 5. ADDI (Add Immediate)
    addi $t3, $zero, 2     # $t3 = 2 (using immediate)

    # 6. MUL (Multiplication)
    mul $t2, $t2, $t3      # $t2 = (A + B) * 2

    # 7. Bitwise operations: AND, OR
    and $t4, $t0, $t1      # $t4 = A & B
    or  $t5, $t0, $t1      # $t5 = A | B

    # 8. Shifts: SLL (Shift Left), SRL (Shift Right)
    sll $t2, $t2, 2        # Multiply $t2 by 4 via shift
    srl $t2, $t2, 2        # Shift back (divide by 4)

    # 9. SW (Store Word)
    sw $t2, res            # Store final result in memory

    # 10. NOP (No Operation)
    nop                    # Pipeline filler/delay slot

end:
    # Exit program
    li $v0, 10
    syscall


# ADD signed integer addition
# ADDI add immediate
# SUB signed integer subtraction
# MUL integer multiplication
# AND bitwise and operation
# OR bitwise or operaton
# SLL shift left logical
# SRL shift right logical
# LW load word
# SW store word
# BEQ branch if equal to
# J jump
# NOP no op


