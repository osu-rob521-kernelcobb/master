#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue May  7 15:20:44 2019

@author: Zeke
"""

import pygame

import serial
import time
ser = serial.Serial('/dev/cu.DSDTECHHC-05-DevB',115200,timeout=5)
#ser = serial.Serial('/dev/cu.usbmodem14101',115200,timeout=5)
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
left_wheel_d1 = left_wheel
right_wheel = 0.0
right_wheel_d1 = right_wheel
thresh = 0.05

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
			if event.button == X_BUTTON:
				print("Reverse thrower")
				command = "m"
			if event.button == B_BUTTON:
				print("Stop thrower")
				command = "l"
				
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
				
			if abs(right_wheel - right_wheel_d1) > thresh or abs(left_wheel - left_wheel_d1) > thresh:
				command = "w," + format(left_wheel,'.2f') + "," + format(right_wheel,'.2f')
				right_wheel_d1 = right_wheel
				left_wheel_d1 = left_wheel
            
#			if left_wheel > 0.5 and right_wheel > 0.5:
#				drive_command = "w"
#			elif left_wheel > 0.5 and right_wheel < -0.5:
#				drive_command = "d"
#			elif left_wheel < -0.5 and right_wheel > 0.5:
#				drive_command = "a"
#			elif left_wheel < -0.5 and right_wheel < -0.5:
#				drive_command = "s"
#			else:
#				drive_command = " "
			
#			if drive_command != last_command:
#				command = drive_command
				
	if command != "1":
		command = "<" + command + ">"
		print("Command: ", command)
		ser.write(bytes(command,'utf-8'))
		last_command = command
		
	if(ser.inWaiting()>0):
		myData = ser.readline()
		print(myData)
	
	clock.tick(30)


ser.write(bytes(' ','utf-8'))
ser.close()
