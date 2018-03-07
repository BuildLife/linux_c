#!/usr/bin/python



import os
import sys
import datetime
import threading



MoNumber = {}

if __name__=='__main__':
	while True:
		print '***************************'
		print 'Min number and Max number  '
		print '***************************'
		
		MIN = int(raw_input('MIN module number: '))
		MAX = int(raw_input('MAX module number: '))
		
		if MAX < MIN:
			print 'please enter the right number'
			break
		Module = str(raw_input('Module name : '))	

		MoNumber[MIN] = Module

		print MoNumber[MIN]

		break
