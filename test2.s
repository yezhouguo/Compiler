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

main:
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -4($fp)
	lw $t0, -4($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -8($fp)
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -12($fp)
	lw $t0, -12($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -16($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	lw $t0, -20($fp)
	move $t0, $v0
	sw $t0, -20($fp)
	lw $t0, -20($fp)
	move $t1, $t0
	addi $sp, $sp, -4
	sw $t1, -24($fp)
label1:
	lw $t0, -8($fp)
	lw $t1, -24($fp)
	ble $t0, $t1, label2
	sw $t0, -8($fp)
	sw $t1, -24($fp)
	j label3
label2:
	lw $t0, -16($fp)
	lw $t1, -8($fp)
	mul $t2, $t0, $t1
	sw $t2, -20($fp)
	lw $t0, -20($fp)
	move $t1, $t0
	sw $t1, -16($fp)
	li $t1, 1
	addi $sp, $sp, -4
	sw $t1, -28($fp)
	lw $t0, -8($fp)
	lw $t1, -28($fp)
	add $t2, $t0, $t1
	sw $t2, -4($fp)
	lw $t0, -4($fp)
	move $t1, $t0
	sw $t1, -8($fp)
	j label1
label3:
	lw $t0, -16($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t1, 0
	addi $sp, $sp, -4
	sw $t1, -32($fp)
	lw $v0, -32($fp)
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
