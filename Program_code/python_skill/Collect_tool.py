#!/bin/sh/python

import commands
import getpass
import httplib
import sys
import telnetlib
import time

username='root'
password='admin'


def cp_file

def get_upfile



def do_telnet(host, username, password, cmd)
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
	elif cmd == 2:
		











if __name__=='__main__':








