.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

Fib:
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	li $t1, 0
	addi $sp, $sp, -4
	sw $t1, -4($fp)
	lw $t0, 8($fp)
	lw $t1, -4($fp)
	beq $t0, $t1, label1
	sw $t0, 8($fp)
	sw $t1, -4($fp)
	j label3
label3:
	li $t1, 1
	sw $t1, -4($fp)
	lw $t0, 8($fp)
	lw $t1, -4($fp)
	beq $t0, $t1, label1
	sw $t0, 8($fp)
	sw $t1, -4($fp)
	j label2
label1:
	li $t1, 1
	sw $t1, -4($fp)
	lw $v0, -4($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
label2:
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -8($fp)
	lw $t0, 8($fp)
	lw $t1, -8($fp)
	sub $t2, $t0, $t1
	addi $sp, $sp, -4
	sw $t2, -12($fp)
	lw $t0, -12($fp)
	addi $sp, $fp, -16
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal Fib
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -20($fp)
	move $t0, $v0
	sw $t0, -20($fp)
	li $t1, 2
	addi $sp, $sp, -4
	sw $t1, -24($fp)
	lw $t0, 8($fp)
	lw $t1, -24($fp)
	sub $t2, $t0, $t1
	addi $sp, $sp, -4
	sw $t2, -28($fp)
	lw $t0, -28($fp)
	addi $sp, $fp, -32
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal Fib
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -36($fp)
	move $t0, $v0
	sw $t0, -36($fp)
	lw $t0, -20($fp)
	lw $t1, -36($fp)
	add $t2, $t0, $t1
	addi $sp, $sp, -4
	sw $t2, -40($fp)
	lw $v0, -40($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra

main:
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	li $t1, 1
	sw $t1, -20($fp)
	lw $t0, -20($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -4($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -8($fp)
	move $t0, $v0
	sw $t0, -8($fp)
	lw $t0, -8($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -12($fp)
	lw $t0, -12($fp)
	addi $sp, $fp, -16
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal Fib
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -20($fp)
	move $t0, $v0
	sw $t0, -20($fp)
	lw $t0, -20($fp)
	move $t1, $t0
	sw $t1, -4($fp)
	lw $t0, -4($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t1, 0
	addi $sp, $sp, -4
	sw $t1, -24($fp)
	lw $v0, -24($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
