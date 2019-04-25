import pygame

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
while 1:
	for event in pygame.event.get():
		if event.type == pygame.JOYBUTTONDOWN:
			if event.button == A_BUTTON:
				robot_state ^= 1
				if robot_state == THROW_STATE:
					print("Throw Mode")
				else:
					print("Drive Mode")
                
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
 
#while(1):
#	for i in range(n):
