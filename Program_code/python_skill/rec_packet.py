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
Internet Protocol Version
----------------------------------------
| Protocol    Protocol name
----------------------------------------
|	1			ICMP
---------------------------------------
|	2			IGMP
----------------------------------------
|	3			GGP
----------------------------------------
|	6			TCP
----------------------------------------
|	8			EGP
----------------------------------------
|	9			IGP
----------------------------------------
|	17			UDP
----------------------------------------
|	41			IPv6
----------------------------------------
|	47			GRE
----------------------------------------
|	50			ESP
----------------------------------------
|	51			AH
----------------------------------------
|	58			ICMPv6
----------------------------------------
|	88			EIGRP
----------------------------------------
|	89			OSPF
----------------------------------------
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

	msd = struct.unpack('!6B6BH',mac_addr)

	des_addr = '%02x' % msd[0] +':'+ '%02x' % msd[1] +':'+ '%02x' % msd[2] +':'+ '%02x' % msd[3]+':'+ '%02x' % msd[4]+':'+'%02x' % msd[5];
	src_addr = '%02x' % msd[6] +':'+ '%02x' % msd[7] +':'+ '%02x' % msd[8] +':'+ '%02x' % msd[9]+':'+ '%02x' % msd[10]+':'+'%02x' % msd[11];
	Check_type = '%04x' % msd[12]

	print 'Source mac : ' + des_addr + '\nDestination mac : ' + src_addr


	if Check_type == '8100':
		Eth_packet = packet[14:18]
		VLAN_packet = struct.unpack('!HH',Eth_packet)
		Eth_type = '%04x' % VLAN_packet[1]
		if Eth_type == '0800' and Eth_type == '0806':
			new_packet = packet[18:]
			print 'VID : \n',VLAN_packet[0]
			print 'Ethernet Type : 0x'+Eth_type+'\n'
	else:
		if Check_type == '0800' and '0806':
			print 'Ethernet Type : 0x'+Check_type+'\n'
			new_packet = packet[14:]


	#All packet into the next function
	divition_protocol(new_packet)

def divition_protocol(internet_protocol_packet):
	
	#take first 20 characters for the ip header
	ip_header = internet_protocol_packet[0:20]	

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

	print 'Version : ' + str(version) +'\n'+'IP Header Length : ' + str(iph_length)
	print 'TTL : ' + str(ttl) +'\n'+'Protocol :' + str(protocol)
	print 'Source address : ' + str(ip_s_addr) +'\n'+ 'Destination address : '+ str(ip_d_addr)

	print '-----------------------------------------------\n'

if __name__=='__main__':
	while True:
		p = threading.Thread(target=rece_eth(sys.argv[1]))
		p.start()


