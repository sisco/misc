#Brian Sisco
#bps16@pitt.edu
#April 17, 2009
#CS 447

.data
#Important float constants
one: .float 1.0
two: .float 2.0
width: .float 320.0
height: .float 200.0
step: .float .00625

#The address of the pixels
base: .word 0xFFFF0000
#The array of bytes for the pixels
array: .space 64000


.text

jal draw_grid

#Yellow = 11111100 = 252
#Green = 00011100 = 28
li $a0, 252						#Curve 1, y=x^2, will be yellow.
l.s $f16, step					#The step!
l.s $f1, one					#The upper bound!
neg.s $f12, $f1					#Starting the curve at x = -1
curveLoop:
c.lt.s $f12, $f1				#Is x < 1?
bc1f notLessCurve
	mul.s $f13, $f12, $f12		#y = x * x
	jal plot_xy					#plot the point of curve 1
	li $a0, 28					#switch color to green
	neg.s $f13, $f13			#negate y so to plot curve 2
	jal plot_xy					#plot the point of curve 2
	li $a0, 252					#go back to yellow
	add.s $f12, $f12, $f16		#x += step
	j curveLoop
notLessCurve:

li $v0, 10 			#ends program
syscall

plot_xy:
# void plot_xy($f12, $f13, $a0)
# plots a colored pixel at (x,y) coordinate specified
# $f12 is x-coordinate in range [-1.0,1.0]
# $f13 is y-coordinate in range [-1.0,1.0]
# $a0 is 8-bit RGB color value for the pixel
# there is no return value

#I shall make this work by converting $f12 and $f13 to pixel coordinates and then using them to call draw_pixel
l.s $f1, one				#Getting some important floats
l.s $f2, two
l.s $f3, width
l.s $f4, height
add.s $f14, $f12, $f1		#x + 1.0
div.s $f14, $f14, $f2		#(x + 1.0) / 2.0
mul.s $f14, $f14, $f3		#(x + 1.0) / 2.0 * 320.0

neg.s $f15, $f13			#-y
add.s $f15, $f15, $f1		#-y + 1.0
div.s $f15, $f15, $f2		#(-y + 1.0) / 2.0
mul.s $f15, $f15, $f4		#(-y + 1.0) / 2.0 * 200.0

cvt.w.s $f14, $f14			#converts x to an int
mfc1 $a1, $f14				#moves it to an int register
cvt.w.s $f15, $f15			#converts y to an int
mfc1 $a2, $f15				#moves it to an int register

j draw_pixel				#draw the pixel now that x and y are pixel values
							#It is nice that this function always then goes to the 
							#next fuction because it means not having to explicitly save $ra.

draw_pixel:
# void draw_pixel($a1, $a2, $a0)
# sets the pixel at (pixelx,pixely) to a color
# $a1 is pixel x-coordinate in range [0,319]
# $a2 is pixel y-coordinate in range [0,199]
# $a0 is 8-bit RGB color value for the pixel
# there is no return value

#Need to get the offset to key into the table: (row * maxcol) + col
li $t0, 320			#maxcol
mult $a2, $t0		#row * maxcol
mflo $t0			#row * maxcol
add $t0, $t0, $a1 	#row * maxcol) + col
la $t1, base
lw $t1, 0($t1)		#0xFFFF0000
add $t1, $t1, $t0	#add offset to base
sb $a0, 0($t1)		#set the pixel to the given color

jr $ra

draw_grid:
# void draw_grid()
# draws the grid lines
#White = 255

move $s0, $ra				#save the location this was called from
li $a0, 255					#white
li $a1, 319					#starting on the right side					
li $a2, 99					#the middle y-wise
loopXaxis:					#Draws the x-axis right to left
beq $a1, $zero, endXaxis
	jal draw_pixel
	addi $a1, $a1, -1
	j loopXaxis
endXaxis:

li $a1, 159					#The y-axis is in the middle of the x-axis
li $a2, 199					#Starting at the bottom
loopYaxis:					#Draws the Y-axis bottom to top
beq $a2, $zero, endYaxis
	jal draw_pixel
	addi $a2, $a2, -1
	j loopYaxis
endYaxis:
move $ra, $s0
jr $ra
