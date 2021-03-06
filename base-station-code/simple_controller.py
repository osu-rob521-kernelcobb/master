import pygame

import serial
import time
ser = serial.Serial('/dev/cu.DSDTECHHC-05-DevB',9600,timeout=5)
time.sleep(2)
print(ser.name)
print("Connected")
print("Press XBox button to quit")

# event.button values
A_BUTTON = 11
B_BUTTON = 12
X_BUTTON = 13
Y_BUTTON = 14
LB_BUTTON = 8
RB_BUTTON = 9
DPAD_UP = 0
DPAD_LEFT = 2
DPAD_DOWN = 1
DPAD_RIGHT = 3
START_BUTTON = 4
BACK_BUTTON = 5
XBOX_BUTTON = 10
# event.axis values
LEFT_STICK_X = 0
LEFT_STICK_Y = 1
RIGHT_STICK_X = 2
RIGHT_STICK_Y = 3
LEFT_TRIGGER = 4
RIGHT_TRIGGER = 5

# Robot States
THROW_STATE = 0
DRIVE_STATE = 1

THUMB_DEADZONE = 0.2
TRIGGER_DEADZONE = -0.9

pygame.init()

pygame.joystick.init()
clock = pygame.time.Clock()

print(pygame.joystick.get_count())
_joystick = pygame.joystick.Joystick(0)
_joystick.init()

robot_state = 0
teleop = True
left_wheel = 0.0
right_wheel = 0.0
last_command = " "
ser.write(bytes(last_command,'utf-8'))
while teleop:
	command = "1"
	for event in pygame.event.get():
		if event.type == pygame.JOYBUTTONDOWN:
			if event.button == A_BUTTON:
				print("Shooting Beanbag")
				command = "f"
			if event.button == XBOX_BUTTON:
				print("End Teleoperation")
				command = " "
				teleop = False
			if event.button == LB_BUTTON:
				print("Reduce throw distance")
				command = "g"
			if event.button == RB_BUTTON:
				print("Increase throw distance")
				command = "t"
			if event.button == DPAD_RIGHT:
				print("Reloader left")
				command = "h"
			if event.button == DPAD_LEFT:
				print("Reloader right")
				command = "y"
			if event.button == Y_BUTTON:
				print("Sensing range")
				command = "z"
				
		if event.type == pygame.JOYAXISMOTION:
			if event.axis == LEFT_STICK_Y:
				left_wheel = -_joystick.get_axis(LEFT_STICK_Y)
				if abs(left_wheel) < THUMB_DEADZONE:
					left_wheel = 0.0
				print("Left Wheel Speed: ",left_wheel)
			if event.axis == RIGHT_STICK_Y:
				right_wheel = -_joystick.get_axis(RIGHT_STICK_Y)
				if abs(right_wheel) < THUMB_DEADZONE:
					right_wheel = 0.0
				print("Right Wheel Speed: ",right_wheel)
            
			if left_wheel > 0.5 and right_wheel > 0.5:
				drive_command = "w"
			elif left_wheel > 0.5 and right_wheel < -0.5:
				drive_command = "d"
			elif left_wheel < -0.5 and right_wheel > 0.5:
				drive_command = "a"
			elif left_wheel < -0.5 and right_wheel < -0.5:
				drive_command = "s"
			else:
				drive_command = " "
			
			if drive_command != last_command:
				command = drive_command
				
	if command != "1":
		command = command + "\n"
		print("Command: ", command)
		ser.write(bytes(command,'utf-8'))
		last_command = command
		
	if(ser.inWaiting()>0):
		myData = ser.readline()
		print(myData)
#	xdir = _joystick.get_axis(0)
#	print(xdir)
#	rtrigger = _joystick.get_axis(5)
#	#deadzone
#	if abs(xdir) < 0.2:
#		xdir = 0.0
#	if rtrigger < -0.9:
#		rtrigger = -1.0
# 
	clock.tick(30)
 
 
		
 
	'''
	print _joystick.get_init()
	print _joystick.get_id()
	print _joystick.get_name()
	print _joystick.get_numaxes()
	print _joystick.get_numballs()
	print _joystick.get_numbuttons()
	print _joystick.get_numhats()
	print _joystick.get_axis(0)
	print _joystick.get_button(0)
	n = _joystick.get_numbuttons()
	axesnum = _joystick.get_numaxes()
	for i in range(axesnum):
		print _joystick.get_axis(i)
		'''
 
ser.write(bytes(' ','utf-8'))
ser.close()
