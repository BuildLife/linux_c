#!/usr/bin/python

import commands
import getpass
import httplib
import os.path
import sys
import subprocess
import telnetlib
import time

username='root'
password='admin'
tftp_folder='/var/lib/tftpboot'
controller_folder="/home/eric/cmc_codebase/cmc_mgt/boards/rtl819xD/image/"

up_file=' '

def cp_upfile():
	subprocess.call('ls ' + controller_folder +'CMCMGT-fw-* ', shell=True)
	get_filename = commands.getstatusoutput('ls ' + controller_folder +'CMCMGT-fw-* ')
	#print "file time is ", time.ctime(os.path.getmtime(controller_folder +'CMCMGT-fw-0.1.1_17_MYMAC_FPGA_SE_8.bin '))
	print "file time is ", time.ctime(os.path.getmtime('Collect_tool.py'))
	print get_filename
	#subprocess.call('sudo cp '+ controller_folder +'CMCMGT-fw-* '+ tftp_folder, shell=True)
	#get_times = subprocess.call(,shell=True)


def get_upfile():
	subprocess.call('ls '+ tftp_folder, shell=True)
	file_time = subprocess.call('ls', shell=True)
	print file_time


def do_telnet(host, username, password, cmd):
	tn = telnetlib.Telnet(host, port=23, timeout=10)
	tn.set_debuglevel(2)

	tn.read_util('login: ')
	tn.write(username + '\n')
	print "Login\n"

	time.sleep(2)

	tn.read_util('Password: ')
	tn.write(password + '\n')
	print "Password\n"
	print "Login Success....\n"

	time.sleep(2)

	tn.read_util('Controller#')

	if cmd == 1:
		tn.write('Configure terminal\n')
		tn.read_util('Controller-cfg#')
		time.sleep(2)
		tn.write('upgrade controller firmware' + up_file + ' ' + clientip + '\n')
	#elif cmd == 2:
	#else

	tn.close();


if __name__=='__main__':

	while True:
		print "Command Options:\n"
		print "1. CP file to /var/lib/tftpboot\n"
		print "2. Telnet in CMC & Upgrade the controller\n"
		print "3. \n"
		print "other Command : Leaves this Program\n"

		opt= input()

		if opt == 1:
			print opt
			print "CP File to /var/lib/tftpboot\n"
			cp_upfile()

		elif opt == 2:
			print "Telnet in CMC & Upgrade the controller\n"
			get_upfile()
			#do_telnet(host, username, password, cmd)
	
		elif opt == 3:
			print "Changes an ip address.\n"
		else :
			print "Leave Program....."
			exit(1)







