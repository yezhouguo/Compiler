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

fact:
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	li $t1, 1
	addi $sp, $sp, -4
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
	j label3
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
	jal fact
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -20($fp)
	move $t0, $v0
	sw $t0, -20($fp)
	lw $t0, 8($fp)
	lw $t1, -20($fp)
	mul $t2, $t0, $t1
	addi $sp, $sp, -4
	sw $t2, -24($fp)
	lw $v0, -24($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
label3:

main:
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -4($fp)
	move $t0, $v0
	sw $t0, -4($fp)
	lw $t0, -4($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -8($fp)
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -12($fp)
	lw $t0, -8($fp)
	lw $t1, -12($fp)
	bgt $t0, $t1, label4
	sw $t0, -8($fp)
	sw $t1, -12($fp)
	j label5
label4:
	lw $t0, -8($fp)
	addi $sp, $fp, -16
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal fact
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -12($fp)
	move $t0, $v0
	sw $t0, -12($fp)
	lw $t0, -12($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -20($fp)
	j label6
label5:
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -24($fp)
	lw $t0, -24($fp)
	move $t1, $t0
	sw $t1, -20($fp)
label6:
	lw $t0, -20($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t1, 0
	addi $sp, $sp, -4
	sw $t1, -28($fp)
	lw $v0, -28($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
