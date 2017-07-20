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

	elif option == 'TFTP'
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

	#get src & des mac address
	mac_addr = packet[0:14]

	msd = struct.unpack('!6B6B2B',mac_addr)

	des_addr = '%02x' % msd[0] +':'+ '%02x' % msd[1] +':'+ '%02x' % msd[2] +':'+ '%02x' % msd[3]+':'+ '%02x' % msd[4]+':'+'%02x' % msd[5];
	src_addr = '%02x' % msd[6] +':'+ '%02x' % msd[7] +':'+ '%02x' % msd[8] +':'+ '%02x' % msd[9]+':'+ '%02x' % msd[10]+':'+'%02x' % msd[11];

	print 'Source mac : ' + des_addr + '\nDestination mac : ' + src_addr

	print 'Type : 0x%02x' % msd[12] + '%02x' % msd[13]
	
	#take first 20 characters for the ip header
	ip_header = packet[14:34]

	#now unpack ip_header
	iph = struct.unpack('!BBHHHBBH4s4s', ip_header)

	version_ihl = iph[0]
	version = version_ihl >> 4
	ihl = version_ihl & 0xF

	iph_length = ihl * 4

	ttl = iph[5]
	protocol = iph[6]
	ip_s_addr = socket.inet_ntoa(iph[8]);
	ip_d_addr = socket.inet_ntoa(iph[9]);

	print 'Version : ' + str(version) + ' IP Header Length : ' + str(iph_length)
	print ' TTL : ' + str(ttl) + ' Protocol :' + str(protocol)
	print ' Source address : ' + str(ip_s_addr) + 'Destination address : '+ str(ip_d_addr)

if __name__=='__main__':
	while True:
		p = threading.Thread(target=rece_eth(sys.argv[1]))
		p.start()
	#rece_eth(sys.argv[0])


