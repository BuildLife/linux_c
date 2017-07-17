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

getfilename=' '

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

def do_telnet(host, username, password, cmd, ip):
	tn = telnetlib.Telnet(host, port=23, timeout=10)
	tn.set_debuglevel(2)

	tn.read_until('login: ')
	tn.write(username + '\n')
	print "Login\n"

	time.sleep(2)

	tn.read_until('Password: ')
	tn.write(password + '\n')
	print "Password\n"
	print "Login Success....\n"

	time.sleep(2)

	tn.read_until('Controller#')

	if cmd == 1:
		tn.write('configure terminal\n')
		tn.read_until('Controller-cfg#')
		time.sleep(2)
		tn.write('upgrade controller firmware' + getfilename + ' ' + ip + '\n')
	#elif cmd == 2:
	#else

	if cmd == 2:
		print "test"




	tn.close();























def changes_hostip(cmd,host_inter):
	if cmd == 1:
		status = commands.getstatusoutput('sudo ifconfig '+host_inter+' 192.168.10.99')[0]
	elif cmd == 2:
		status = commands.getstatusoutput('sudo ifconfig '+host_inter+' 192.168.100.2')[0]
	elif cmd == 3:
		status = commands.getstatusoutput('sudo ifconfig '+host_inter+' 192.168.1.5')[0]
	elif cmd == 4:
		o_ip = str(raw_input("\nKey in you want to change ip: "))
		status = commands.getstatusoutput('sudo ifconfig '+host_inter+' '+o_ip)[0]
	else:
		print "No this Option, Please input the Option again.......\n"
	
	if status == 0:
		print "Changes IP Success\n"
	else :
		print "Changes IP Error\n"

	time.sleep(2)
	ifconfig = commands.getstatusoutput('ifconfig')
	print ifconfig

if __name__=='__main__':

	while True:
		print "...............................................\n"
		print "Command Options:\n"
		print "1. CP file to /var/lib/tftpboot\n"
		print "2. Telnet in CMC & Upgrade the controller\n"
		print "3. Changes an ip address\n"
		print "other Command : Leaves this Program\n"
		print "...............................................\n"

		opt = int(raw_input("\nCommand Options: "));
		print "\n"

		if opt == 1:
			print "CP file to /var/lib/tftpboot\n"
			cp_upfile()
			if get_status == 0:
				print "CP "+getfilename+" to "+tftp_folder+" Success\n"
			else :
				print "CP to "+tftp_folder+" Error\n"

		elif opt == 2:
			print "Telnet in CMC & Upgrade the controller\n"
			comport = str(raw_input("\nPlease Input Host Interface(ex:eth14/eth2): "))
			ip = get_hostip(comport)
			host = str(raw_input("\nPlease Input CMC ip(ex:192.168.1.100): "))
			if getfilename == ' ':
				print "please cp file to tftpboot folder\n"
			else:
				do_telnet(host, username, password, 1, ip)
	
		elif opt == 3:
			print "Changes an ip address.\n"
			print "1. Lab IP(10.99).\n"
			print "2. CMC IP(100.2).\n"
			print "3. CMC TFTP IP(1.5).\n"
			print "4. Self defines an IP(XXX.XXX)\n"
			cmd = int(raw_input("\nIP Option: "))
			host_interface = str(raw_input("\nInterface : "))
			changes_hostip(cmd,host_interface)

		else :
			print "Leave Program....\n"
			exit(1)







