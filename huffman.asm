	#
	# YOUR NAME: Brian Sisco
	# YOUR EMAIL: bps16@pitt.edu
	#
	##########################################################
	# CS 0447 - Spring 2009 - Project #2 Driver Program
	# last modified: 03/02/2009 at 10:30pm
	#
	# see web site for project assignment.
	#
	# DO NOT CHANGE ANY PARTS OF THIS PROGRAM OTHER THAN THE
	# COMPRESSION/DECOMPRESSION ROUTINES AS NOTED IN THE
	# ASSIGNMENT. YOU MAY IMPLEMENT MENU OPTIONS 3 AND 4 IF
	# YOU WANT. 
	#
	# look for routines _compress and _decompress below to see
	# where you should add your code. DO NOT CHANGE THE FUNCTION
	# CALL PARAMETERS/RETURN VALUES!!!!
	#
	##########################################################
	#
	# questions? problems with this program? 
	#      ask childers@cs.pitt.edu or musfiq@cs.pitt.edu
	#
	##########################################################
	#
	.data
	welcome:	.asciiz	"Welcome to Huffman!!\n"
	menu:	.asciiz	"\n\nMenu\n  1) Compress\n  2) Decompress\n  5) Exit\n\nChoice?\n"
	msg_menu_1:	.asciiz "\nBuffer compressed:\n"
	msg_menu_2:	.asciiz	"\nBuffer decompressed:\n"
	msg_menu_5:	.asciiz	"\nGoodbye!\n"
	msg_menu_e:	.asciiz	"\n*** Invalid entry. Try again. ***\n"
	msg_unimpl:	.asciiz "\n*** UNIMPLEMENTED FEATURE. ***\n"
	msg:	.asciiz "HELLO WORLD"

	.align	2

	# compressed buffer -- holds compressed message
	cbuf:	.space 	128
	# decompressed buffer -- holds message after decompression
	dbuf:	.space	128
	# message buffer -- temporary space for printing strings
	mbuf:	.space	128

	# compression table - indexed by character
	ctab:		# bit len	codeword	
        .word   0x00000003, 0x00000006 # _ 110
        .word   0x00000004, 0x0000000a # A 1010
        .word   0x00000006, 0x00000025 # B 100101
        .word   0x00000004, 0x00000000 # C 0000
        .word   0x00000005, 0x0000001c # D 11100
        .word   0x00000003, 0x00000001 # E 001
        .word   0x00000006, 0x00000027 # F 100111
        .word   0x00000006, 0x00000026 # G 100110
        .word   0x00000005, 0x00000017 # H 10111
        .word   0x00000004, 0x00000008 # I 1000
        .word   0x0000000b, 0x00000095 # J 00010010101
        .word   0x00000007, 0x00000008 # K 0001000
        .word   0x00000005, 0x00000016 # L 10110
        .word   0x00000006, 0x0000003b # M 111011
        .word   0x00000004, 0x00000007 # N 0111
        .word   0x00000004, 0x00000004 # O 0100
        .word   0x00000005, 0x00000003 # P 00011
        .word   0x0000000a, 0x0000004b # Q 0001001011
        .word   0x00000004, 0x00000005 # R 0101
        .word   0x00000004, 0x00000006 # S 0110
        .word   0x00000004, 0x0000000f # T 1111
        .word   0x00000006, 0x0000003a # U 111010
        .word   0x00000007, 0x00000048 # V 1001000
        .word   0x00000006, 0x00000005 # W 000101
        .word   0x00000008, 0x00000013 # X 00010011
        .word   0x00000007, 0x00000049 # Y 1001001
        .word   0x00000009, 0x00000024 # Z 000100100
	ctab_eom:
        .word   0x0000000b, 0x00000094, -1, 0x0	 # EOM 00010010100

	#
	.text
	
	# print welcome message
	la		$a0,welcome
	li		$v0,4
	syscall

	###############################################################
	# MAIN MENU LOOP
	###############################################################

menu_loop:
	# print menu
	la		$a0,menu
	li		$v0,4
	syscall

	# get menu choice
	li		$v0,5
	syscall

	# process menu choice
	addi	$t0,$v0,-1		# check for option 1)
	bne		$t0,$0,menu_2

	#########################################################
	# 1) compress an input string and print compressed hex
	#########################################################
	#
	# prompt user to enter a message to compress
	# message must be only the chars [A-Z,\space]
	# no edit checks are done!
	#
	la		$a0,mbuf
	li		$a1,127
	li		$v0,8
	syscall
	jal		_chomp			# remove LF
	#
	# compress the message
	#
	la		$a0,mbuf		# message buffer to compress
	la		$a1,cbuf		# target buffer to hold compressed message
	jal		_compress		# call compress
	#
	# print a hex string for the compressed message
	#
	la		$a0,cbuf		# compressed buffer
	la		$a1,mbuf		# output buffer for hex string
	move	$a2,$v0			# byte length of compressed buffer
	jal		_to_hex_string	# make a hex string from cbuf
	la		$a0,msg_menu_1	# print prompt  
	li		$v0,4
	syscall
	la		$a0,mbuf		# print the hex string for cbuf
	syscall
	li		$a0,'\n'		# print newline
	li		$v0,11
	syscall
	j		menu_loop

	# check for menu option "2"
menu_2:
	addi	$t0,$v0,-2
	bne		$t0,$0,menu_3

	#########################################################
	# 2) decompress an input hex string, print decoded string
	#########################################################
	#
	# prompt user to enter a hex string for a compressed message
	#
	la		$a0,mbuf
	li		$a1,127
	li		$v0,8
	syscall
	# convert hex str into binary rep for compressed message
	la		$a0,mbuf		# hex char string
	la		$a1,cbuf		# target buffer for compressed message
	jal		_fr_hex_string

	la		$a0,cbuf		# cbuf holds compressed message
	la		$a1,dbuf		# dbuf is target buffer for decompressed message
	jal		_decompress		# make the call
	la		$a0,msg_menu_2	# print prompt
	li		$v0,4
	syscall	
	la		$a0,dbuf		# print decompressed message
	syscall					
	li		$a0,'\n'		# print newline
	li		$v0,11
	syscall
	j		menu_loop

	# check for menu option "3"
menu_3:
	addi	$t0,$v0,-3
	bne		$t0,$0,menu_4

	#########################################################
	# 3) encode a char into a codeword (as a bit string)
	#########################################################
	# UNIMPLEMENTED. YOU MAY WISH TO IMPLEMENT TO EXPLORE HOW
	# TO DECODE A SYMBOL. THIS OPTION WILL NOT BE GRADED.
	#########################################################
	la		$a0,msg_unimpl
	li		$v0,4
	syscall
	j		menu_loop

	# check for menu option "4"
menu_4:
	addi	$t0,$v0,-4
	bne		$t0,$0,menu_5

	#########################################################
	# 4) decode a symbol (as a bit string) into a character
	#########################################################
	# UNIMPLEMENTED. YOU MAY WISH TO IMPLEMENT TO EXPLORE HOW
	# TO DECODE A SYMBOL. THIS OPTION WILL NOT BE GRADED.
	#########################################################
	la		$a0,msg_unimpl
	li		$v0,4
	syscall
	j		menu_loop

menu_5:
	addi	$t0,$v0,-5
	bne		$t0,$0,menu_error

	#########################################################
	# 5) exit
	#########################################################
	la		$a0,msg_menu_5
	li		$v0,4
	syscall
	la		$v0,10
	syscall

menu_error:
	la		$a0,msg_menu_e
	li		$v0,4
	syscall
	j		menu_loop



	##############################################################
	# COMPRESSION AND DECOMPRESSION ROUTINES
	##############################################################

	#
	# YOU ARE TO PROVIDE THE TWO MAIN FUNCTIONS:
	# bytelen = _compress(msgptr, bufptr)
	# void _decompress(bufptr, msgptr, byte length)
	#
	# see below for how arguments and return values should
	# be passed. you MUST obey this convetion for the routines
	# to work correctly with the "menu driver".
	#

_compress:
	# bytelen = _compress(msgptr, bufptr)
	#   compresses message at msgptr to buffer at bufptr
	#
	# args:		$a0 is ptr to message to compress
	# 			$a1 is ptr to compression buffer
	# returns:	$a0,$a1 is final position in buffer
	#			$v0 is byte length of compressed message
	# trashes:	? ** LIST REGISTERS DESTROYED HERE. **
	#
	#########################################################
	# YOUR CODE GOES HERE. 
	#########################################################
	#
	# REMOVE LINES BELOW TO END WHEN YOU WRITE YOUR FUNCTION. 
	.data
	_compress_msg:	.asciiz "\nCompression. You provide.\n"
	.text
	
	la $t0, ctab #address for the table
	li $t1, -65	 #offset to go from ASCII to table index

	li $t5, 1
	sll $t5, $t5, 31	#mask of a one in the msb

	li $t7, 0			#the pointer to the portion of the buffer to fill
	li $s0, 0			#flag for done with word
	
	compressLoop:

	lb $t2, ($a0) 		#get a character from the message
	##Special case: if it is a null then it is EOM and a flag is set for the loop to end after this iteration.
	bne $t2, $zero, notNull
		li $t3, 11		#bit length for EOM
		li $t2, 148		#code word for EOM
		li $s0, 1		#flag for done with word
		j wasSpaceOrNull
	notNull:

	##Special case for if the char is a space
	li $t9, 32			#ascii for a space
	bne $t2, $t9, notSpace
		li $t3, 3		#bit length for a space
		li $t2, 6		#code word for a space
		j wasSpaceOrNull#skips the part about finding these two things
	notSpace:

	add $t2, $t2, $t1	#subtract 65 to get index value
	sll $t2, $t2, 3		#multiply by 8 because each letter takes up that much space in the table (bit length + code word)
	add $t2, $t0, $t2	#add address of the table
	addi $t3, $t2, 8	#address of bit length
	addi $t2, $t2, 12	#address of code word
	lb $t3, ($t3)		#bit length
	lb $t2, ($t2)		#code word

	wasSpaceOrNull:	

	add $v0, $v0, $t3	#adds the bit length to the running total of the bit length

	li $t4, 32
	sub $t4, $t4, $t3	#how much to shift by to left align the code word
	sllv $t2, $t2, $t4	#code word shifted left

	compressCharLoop:
	and $t6, $t5, $t2	#msb of code word
	srl $t6, $t6, 31	#move it to the right
	
	sllv $t6, $t6, $t7	#move it to the left some so it will go into the right spot of the buffer
	
	lb $t8, ($a1)		#get the byte from the buffer
	or $t8, $t8, $t6	#add the bit to the byte
	sb $t8, ($a1)		#put the byte back in the buffer

	sll $t2, $t2, 1		#shifts the used bit off the code word
	addi $t3, $t3, -1	#decrements the bit length
	addi $t7, $t7, 1	#increment the in-word pointer
	li $t9, 32			#the max the in-word pointer should be + 1
	bne $t7, $t9, doNotIncrementWord
		li $t7, 0		#reset teh in-word pointer
		addi $a1, $a1, 4	#increments the word of the buffer being addressed
	doNotIncrementWord:

	bne $t3, $zero, compressCharLoop
	
	bne $s0, $zero, exitCompressLoop

	addi $a0, $a0, 1	#increments the character it will look at next
	j compressLoop
	
	exitCompressLoop:

	move $v1, $v0		#save the bit length
	la		$a2, cbuf
	li $t9, 1
	loopPrint:
		lb $a0, ($a2)
		li		$v0,11
		syscall
		addi $a2, $a2, 1
		addi $t9, $t9, 1
	bne $t9, $v1, loopPrint
	
	move $v0, $v1		#puts the bit length back
	
	jr		$ra

_decompress:
	# void _decompress(bufptr, msgptr)
	#
	# args:		$a0 is compression source buffer (bufptr)
	# 			$a1 is decompression target buffer (msgptr)
	# returns:	none
	# trashes:	? ** LIST REGISTERS DESTROYED HERE. **
	#
	#########################################################
	# YOUR CODE GOES HERE. 
	#########################################################
	#
	# REMOVE LINES BELOW TO END WHEN YOU WRITE YOUR FUNCTION. 
	.data
	_decompress_msg:	.asciiz "\nDecompression. You provide.\n"
	.text
	la		$a0,_decompress_msg
	li		$v0,4
	syscall
	# do nothing -- just null terminate the msgptr
	sb		$0,0($a1)		# write the null
	# END REMOVE LINES
	#
	jr		$ra


	##############################################################
	# UTILITY ROUTINES
	##############################################################
	#
	# you should not have to touch these routines.
	# these routines are used by the menu driver to convert
	# strings to/from hex and to/from binary.
	#

_to_bin_string:
	# _to_bin_string(value, target str, bit length)
	#	
	# args:		$a0 is value to convert to binary string
	#			$a1 is destination string buffer
	#			$a2 is bit length
	# returns:	none
	# trashes:	$t0-$t3
	#
	move	$t3,$a1
	addi	$t0,$0,1
	addi	$t1,$a2,-1
	sllv	$t0,$t0,$t1
_to_bin_string_loop:
	and		$t2,$a0,$t0
	srlv	$t2,$t2,$t1
	addi	$t2,$t2,'0'
	sb		$t2,0($t3)
	srl		$t0,$t0,1
	addi	$t3,$t3,1
	addi	$t1,$t1,-1
	bgez	$t1,_to_bin_string_loop
	sb		$0,0($t3)
	jr		$ra

_fr_bin_string:
	# UNIMPLEMENTED. MIGHT WANT TO IMPLEMENT FOR OPTION 4.
	jr		$ra

	# WARNING: _to_hex_string, _fr_hex_string
	#   these convert left-to-right, packing left most char
	#   into the lsb.  for compression/decompression, we want
	#   the lsb in the lowest byte addr to be the start of the
	#   message.  thus, the routines do not work in general to
	#   convert a string to hex and vice versa.
	#
_to_hex_string:
	# _to_hex_string(bufptr, msgptr)
	#   turns binary data into an ascii string of hex characters
	#   converts from lsb to msb, so lsb will be in low char posn
	#	e.g., 0x0A is converted to "A0"
	#
	# args:		$a0 is ptr to binary message
	#			$a1 is ptr to buffer to hold string
	#			$a2 is length of buffer 
	# returns:	none
	# trashes:	$t0-$t5
	#
	.data
	_hext:	.asciiz			"0123456789ABCDEF"
	.text
	move	$t0,$a0			# source binary buffer
	move	$t1,$a1			# target string
	move	$t2,$a2			# length
	la		$t3,_hext
_to_hex_string_loop:
	lbu		$t4,0($t0)		# byte to convert to hex char
	andi	$t5,$t4,0x0f
	add		$t5,$t3,$t5
	lbu		$t5,0($t5)		# get hex char from table
	sb		$t5,0($t1)		# save to target string 
	srl		$t5,$t4,4		# upper 4 bits of byte
	add		$t5,$t3,$t5
	lbu		$t5,0($t5)		# get hex char
	sb		$t5,1($t1)		# save to target string
	addi	$t0,$t0,1		# next byte in source buffer
	addi	$t1,$t1,2		# next locations in string
	addi	$t2,$t2,-1		# decrease length
	bne		$t2,$0,_to_hex_string_loop
	sb		$0,0($t1)		# null terminate string
	jr		$ra
	
_fr_hex_string:
	# _fr_hex_string(msgptr, bufptr)
	#  converts from a hex ascii string into a binary message
	#  leftmost char is lsb (e.g., "0A" is number 0xA0)
	#
	# args:		$a0 ptr to null-terminated string of hex chars
	#			$a1 ptr to target buffer to hold binary rep.
	# returns:	$v0 is length of target buffer
	# trashes:	$t0-$t4
	#
	addi	$sp,$sp,-4
	sw		$ra,0($sp)
	move	$t0,$a0			# source string
	move	$t1,$a1			# destination buffer
_fr_hex_string_loop:
	lbu		$t2,0($t0)
	beq		$t2,$0,_fr_hex_string_exit
	jal		_fr_hex_char
	move	$t3,$v0
	lbu		$t2,1($t0)
	beq		$t2,$0,_fr_hex_string_pad
	jal		_fr_hex_char
	sll		$v0,$v0,4
	or		$t3,$t3,$v0
	sb		$t3,0($t1)
	addi	$t0,$t0,2
	addi	$t1,$t1,1
	j		_fr_hex_string_loop
_fr_hex_string_pad:
	sb		$t3,0($t1)
	addi	$t1,$t1,1
_fr_hex_string_exit:
	sub		$v0,$t1,$a1			# return length in $v0
	lw		$ra,0($sp)
	addi	$sp,$sp,4
	jr		$ra

_fr_hex_char:
	# _fr_hex_char(char)
	#	converts an ASCII char into a 4-bit binary number
	#   call only from _fr_hex_string
	#   ** no edit checks are done! call only with [0-9,A-F]
	#
	# args: 	$t2 is ASCII char to convert
	# returns:	$v0 is 4-bit binary code
	#
	andi	$v0,$t2,0x30
	beq		$v0,$0,_fr_hex_char_AF
	andi	$v0,$t2,0x0f	# 0 to 9
	jr		$ra
_fr_hex_char_AF:
	andi	$v0,$t2,0x0f
	addi	$v0,$v0,9
	jr		$ra

_chomp:
	# _chomp(strptr)
	#   removes linefeed at end of user input string
	# 
	# args:		$a0 is ptr to null-terminated string
	# returns:	none (LF removed from string)
	# trashes: 	$t0,$s1
	#
	move	$t0,$a0
_chomp_cont:
	lbu		$t1,0($t0)
	beq		$t1,$0,_chomp_exit		# end of string? (null)
	addi	$t1,$t1,-10				
	beq		$t1,$0,_chomp_exit2		# reached LF?
	addi	$t0,$t0,1
	j		_chomp_cont
_chomp_exit2:
	sb		$0,0($t0)				# overwrite LF with null
_chomp_exit:
	jr		$ra
