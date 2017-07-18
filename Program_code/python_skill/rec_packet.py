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
import threading
import time

'''
def Packet_opt(option):
	if option == 'TCP'

	elif option == 'DHCP'
def 
'''

def rece_eth(interface):
	try:
		s = socket.socket(socket.AF_PACKET,socket.SOCK_RAW,socket.ntohs(0x0003))
	except socket.error , msg:
		print 'Socket could not connect : ' + str(msg[0]) + 'Message ' + msg[1]
		sys.exit()
	s.bind((interface,0))
	#while True:
	packet = s.recvfrom(65535) #receive data buffer max 65535
		
	#packet string from tuple
	packet = packet[0]

	#take first 20 characters for the ip header
	ip_header = packet[0:20]

	#now unpack ip_header
	iph = struct.unpack('!BBHHHBBH4s4s', ip_header)

	version_ihl = iph[0]
	version1 = version_ihl >> 4
	version2 = version_ihl & 0x0F
	ihl = version_ihl & 0xF

	iph_length = ihl * 4

	ttl = iph[5]
	protocol = iph[6]
	s_addr = socket.inet_ntoa(iph[8]);
	d_addr = socket.inet_ntoa(iph[9]);

	print 'Version : ' + str(version1) + str(version2) + ' IP Header Length : ' + str(ihl)
	print ' TTL : ' + str(ttl) + ' Protocol :' + str(protocol)
	print ' Section address : ' + str(s_addr) + 'Destination address : '+ str(d_addr)

if __name__=='__main__':
	while True:
		p = threading.Thread(target=rece_eth(sys.argv[1]))
		p.start()
	#rece_eth(sys.argv[0])


