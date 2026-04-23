# --- SETUP ---
addi $s0, $zero, 5      # ADDI: Set loop counter (5 iterations)
addi $t0, $zero, 0      # ADDI: Set base memory offset for loading (source)
addi $t1, $zero, 40     # ADDI: Set base memory offset for storing (destination)
addi $t2, $zero, 7      # ADDI: Constant for bitwise masking

# --- MAIN LOOP ---
loop:
lw   $t3, 0($t0)        # LW:   Load value from memory - pc 20 
mul  $t4, $t3, $s0      # MUL:  Multiply value by current counter
sub  $t5, $t4, $t2      # SUB:  Subtract mask constant
and  $t6, $t5, $t2      # AND:  Bitwise AND with 7
or   $t7, $t6, $s0      # OR:   Bitwise OR with counter
sll  $t8, $t7, 2        # SLL:  Shift Left Logical by 2 (multiply by 4)
srl  $t8, $t8, 1        # SRL:  Shift Right Logical by 1 (divide by 2)
add  $t9, $t8, $t2      # ADD:  Signed addition of results
sw   $t9, 0($t1)        # SW:   Store final result to memory

# --- CONTROL FLOW ---
addi $t0, $t0, 4        # ADDI: Increment read pointer (4 bytes per word)
addi $t1, $t1, 4        # ADDI: Increment write pointer
addi $s0, $s0, -1       # ADDI: Decrement loop counter
beq  $s0, $zero, end    # BEQ:  If counter == 0, jump to end
j    loop               # J:    Jump back to start of loop 
nop                     # NOP:  Branch delay slot

# --- EXIT ---
end:
j end                   # J:    Infinite loop to halt
nop                     # NOP:  Final padding