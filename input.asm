addi $s0, $zero, 3      
addi $t0, $zero, 0     
addi $t1, $zero, 40    
addi $t2, $zero, 7      

loop:
lw   $t3, 0($t0)        
mul  $t4, $t3, $s0      
sub  $t5, $t4, $t2      
and  $t6, $t5, $t2     
or   $t7, $t6, $s0     
sll  $t8, $t7, 2       
srl  $t8, $t8, 1        
add  $t9, $t8, $t2     
sw   $t9, 0($t1)        

addi $t0, $t0, 4        
addi $t1, $t1, 4        
addi $s0, $s0, -1      
beq  $s0, $zero, end   
j    loop               
nop                     

end:
j end                   
nop                    