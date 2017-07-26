#!/usr/bin/python
import array
import commands
import fcntl
	   
import getpass
import httplib
			  
import os.path
import re

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


def format_macaddress(mac):
	mac_addr = '%02x:%02x:%02x:%02x:%02x:%02x' % struct.unpack('!6B',mac)
	return mac_addr


def rece_eth(interface):
	try:
		s = socket.socket(socket.AF_PACKET,socket.SOCK_RAW,socket.ntohs(0x0003))
	except socket.error , msg:
		print 'Socket could not connect : ' + str(msg[0]) + 'Message ' + msg[1]
		sys.exit()
	s.bind((interface,0))
	packet = s.recvfrom(65535) #receive data buffer max 65535
		
	#packet string from tuple
	packet = packet[0]

	#get src & des mac address
	mac_addr = packet[0:14]

	msd = struct.unpack('!6s6sH',mac_addr)

	src_addr = format_macaddress(msd[0])
	des_addr = format_macaddress(msd[1])
	Check_type = '%04x' % msd[2]

	if Check_type == '8100' or '0806' or '0800':
		print 'Source mac : ' + des_addr + '\nDestination mac : ' + src_addr

	if Check_type == '8100':
		Eth_packet = packet[14:18]
		VLAN_packet = struct.unpack('!HH',Eth_packet)
		Eth_type = '%04x' % VLAN_packet[1]
		print 'VID : '+ str(VLAN_packet[0])
		print 'Ethernet Type : 0x'+Eth_type
		if Eth_type == '0800':
			new_packet = packet[18:]
			divition_protocol(new_packet)
		elif Eth_type == '0806':
			new_packet = packet[18:]
			ARP_protocol(new_packet)
	elif Check_type == '0800':
			print 'Ethernet Type : 0x'+Check_type
			new_packet = packet[14:]
			divition_protocol(new_packet)
	elif Check_type == '0806':
			print 'Ethernet Type : 0x'+Check_type
			new_packet = packet[14:]
			ARP_protocol(new_packet)

	s.close()

def ethernet_header(header_packet):
	


def divition_protocol(internet_protocol_packet):
	
	Pro_array = [1,2,3,6,8,9,17,41,47,50,51,58,88,89]
	Pro_name = ["ICMP","IGMP","GGP","TCP","EGP","IGP","UDP","IPv6","GRE","ESP","AH","ICMPv6","EIGRP","OSPF"]

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
	print 'TTL : ' + str(ttl)
	for i in range(len(Pro_array)):
		if str(protocol) == str(Pro_array[i]):
			Pro_type = Pro_name[i]
			print 'Protocol : '+ str(protocol) + ' ('+Pro_name[i]+') '
	print 'Source address : ' + str(ip_s_addr) +'\n'+ 'Destination address : '+ str(ip_d_addr)

	if Pro_type == "UDP":
		Trans_protocol(internet_protocol_packet[20:])

def Trans_protocol(trans_packet):
	Trans_data = trans_packet[0:8]

	data_analysis = struct.unpack('!HHHH',Trans_data)

	Sour_port = data_analysis[0]
	Des_port = data_analysis[1]
	Length = data_analysis[2]
	Checksum = '%04x' % data_analysis[3]

	print 'Source Port : ' + str(Sour_port)
	print 'Destination Port : ' + str(Des_port)
	print 'Length : ' + str(Length)
	print 'Checksum : 0x'+ Checksum

	print '---------------------------------------------------\n'

def ARP_protocol(arp_packet):
	ip_header = arp_packet[0:28]	

	#now unpack ip_header
	arp = struct.unpack('!HHBBH6s4s6s4s', ip_header)

	hardware_type = '%04x' % arp[0]
	protocol_type = '%04x' % arp[1]
	hardware_size = arp[2]
	protocol_size = arp[3]
	
	Opcode = '%04x' % arp[4]

	src_addr = format_macaddress(arp[5])
	ip_s_addr = socket.inet_ntoa(arp[6])
	
	des_addr = format_macaddress(arp[7])
	ip_d_addr = socket.inet_ntoa(arp[8])

	print 'Hardware Type : ' + str(hardware_type) +' '+'Protocol type : ' + str(protocol_type)
	print 'Hardware size : ' + str(hardware_size) +' '+'Protocol size : ' + str(protocol_size)
	print 'Opcode : ' + str(Opcode)
	print 'Source Mac address : ' , src_addr
	print 'Source IP address : ' + str(ip_s_addr)
	print 'Destination Mac address : ', des_addr
	print 'Destination IP address : '+ str(ip_d_addr)

	print '---------------------------------------------------\n'

if __name__=='__main__':
	while True:
		p = threading.Thread(target=rece_eth(sys.argv[1]))
		p.start()


