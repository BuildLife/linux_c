#!/usr/bin/python

import commands

import fcntl

import getpass

import httplib

import os.path

import sys
import socket
import struct
import subprocess

import telnetlib
import time

username='root'
password='admin'
tftp_folder='/var/lib/tftpboot'
controller_folder='/home/eric/cmc_codebase/cmc_mgt/boards/rtl819xD/image/'


def cp_upfile():
	global getfilename
	global get_status
	get_filename = commands.getstatusoutput('ls ' + controller_folder +'CMCMGT-fw-* ')[1]
	getfilename = get_filename.split(controller_folder)[1]
	get_status = commands.getstatusoutput('sudo cp ' +controller_folder+getfilename+' '+tftp_folder)[0]

def get_hostip(ifname):
	s_ip = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	return socket.inet_ntoa(fcntl.ioctl(
			s_ip.fileno(), 
			0x8915, #SIOCGIFADDR 
			struct.pack('256s', ifname[:15]))[20:24])

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
		tn.write('upgrade controller firmware' + getfilename + ' ' + clientip + '\n')
	#elif cmd == 2:
	#else

	tn.close();

def changes_hostip(cmd)
	if cmd == 1:

	elif cmd == 2:

	elif cmd == 3:
	


if __name__=='__main__':

	while True:
		print "...............................................\n"
		print "Command Options:\n"
		print "1. CP file to /var/lib/tftpboot\n"
		print "2. Telnet in CMC & Upgrade the controller\n"
		print "3. \n"
		print "other Command : Leaves this Program\n"
		print "...............................................\n"

		opt = int(raw_input("\nCommand Options: "));
		print "\n"

		if opt == 1:
			cp_upfile()
			if get_status == 0:
				print "CP "+getfilename+" to "+tftp_folder+" Success\n"
			else :
				print "CP to "+tftp_folder+" Error\n"

		elif opt == 2:
			print "Telnet in CMC & Upgrade the controller\n"
			ip = get_hostip('eth14')
			do_telnet(ip, username, password, cmd)
	
		elif opt == 3:
			print "Changes an ip address.\n"

		else :
			print "Leave Program....."
			exit(1)







