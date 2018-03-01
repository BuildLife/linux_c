#!/usr/bin/python

import os
import sys
import time
import xlrd
import xlwt
import xdrlib
from openpyxl.reader.excel import load_workbook 
import threading
import serial as ser
import pandas as pd

freqlist = ["frequency 0 117000000",
			"yes",
			"frequency 1 171000000",
			"yes",
			"frequency 2 255000000",
			"yes",
			"frequency 3 279000000",
			"yes",
			"frequency 4 333000000",
			"yes",
			"frequency 5 387000000",
			"yes",
			"frequency 6 441000000",
			"yes",
			"frequency 7 495000000",
			"yes",
			"frequency 8 549000000",
			"yes",
			"frequency 9 603000000",
			"yes",
			"frequency 10 657000000",
			"yes",
			"frequency 11 711000000",
			"yes",
			"frequency 12 765000000",
			"yes",
			"frequency 13 819000000",
			"yes",
			"frequency 14 861000000",
			"yes",
			"frequency 15 927000000",
			"yes",
			"frequency 16 998000000",
			"yes"]

coeflist = ["coef if 0 1 1000",
			"yes",
			"coef if 1 1 1000",
			"yes",
			"coef if 2 1 1000",
			"yes",
			"coef if 3 1 1000",
			"yes",
			"coef if 4 1 1000",
			"yes",
			"coef if 5 1 1000",
			"yes",
			"coef if 6 1 1000",
			"yes",
			"coef if 7 1 1000",
			"yes",
			"coef if 8 1 1000",
			"yes",
			"coef if 9 1 1000",
			"yes",
			"coef if 10 1 1000",
			"yes",
			"coef if 11 1 1000",
			"yes",
			"coef if 12 1 1000",
			"yes",
			"coef if 13 1 1000",
			"yes",
			"coef if 14 1 1000",
			"yes",
			"coef if 15 1 1000",
			"yes",
			"coef if 16 1 1000",
			"yes",
			"coef if 17 1 1000",
			"yes"]


Tuner1list = ["offset (1) 0 -5",
			"offset (1)	1 -6.07",
			"offset (1)	2 -5.76",
			"offset (1)	3 -5.68",
			"offset (1)	4 -5.4",
			"offset (1)	5 -6.2",
			"offset (1)	6 -6.81",
			"offset (1)	7 -6.4",
			"offset (1)	8 -4.64",
			"offset (1)	9 -5.07",
			"offset (1)	10 -5.54",
			"offset (1)	11 -5.47",
			"offset (1)	12 -6.3",
			"offset (1)	13 -5.88",
			"offset (1)	14 -5.03",
			"offset (1)	15 -5.5",
			"offset (1)	16 -4.35"]


#Enable Factory mode Command
SnmpCommandOpen = ["snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.1.2.1.2.1 s password",
				   "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.1.1.0 i 2",
				   "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.1.1.0 i 2"]

#Disable Factory mode Command
SnmpCommandClose = ["snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.1.1.0 i 0",
				    "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.1.1.0 i 1",
					"snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.1.0 i 1"]

#Set Mac address 
SnmpCommandMacList = ['snmpset -v 2c -c sagem 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.1 x "0x"',
					  'snmpset -v 2c -c sagem 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.2 x "0x"',
					  'snmpset -v 2c -c sagem 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.3 x "0x"',
					  'snmpset -v 2c -c sagem 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.4 x "0x"',
					  'snmpset -v 2c -c sagem 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.5 x "0x"']
#Set Bpi key
SnmpCommandKeyList = ["snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.2.1.0 x 0x",
					  "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.2.2.0 x 0x",
					  "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.2.3.0 x 0x",
					  "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.2.4.0 x 0x",
					  "snmpset -v 2c -c private 192.168.100.1 .1.3.6.1.4.1.4413.2.99.1.1.2.2.2.5.0 x 0x"]

maclist = []
keylist = []

filename = 'test_excel.xlsx'

CMport = '/dev/ttyUSB0'
CMbaud = 115200

CMFile = '/var/lib/tftpboot'

BroadNum=sys.argv[1]

print 'Connect to CM.......'
#Conn = ser.Serial(CMport, CMbaud, timeout = 1, )
#if not Conn.isOpen():
#	sys.exit()

cMACNum=0
KEYNum=0

def ReadExcel(f_name, nMac, nKey):
	print 'Read MAC & KEY excel'
	
	# Read excel file
	xlsx = load_workbook(f_name)
	
	# Get first table
	Fsheetnames = xlsx.get_sheet_names()
	Fcontent = xlsx.get_sheet_by_name(Fsheetnames[0])
	if not str(Fcontent.title) == 'MAC&KEY':
		print 'Please check excel first page name\n'
		sys.exit()

	# Excel table title
	# row = A	--> Number
	# 		B C --> STB MAC
	#		D 	--> Broad NO.
	#		E 	--> Date
	#		F 	--> CM MAC
	# 		H 	--> cmBpiPublicKey (281)
	#		I 	--> cmBpiPrivateKey (1271)
	#		J 	--> cmBpiPlusRootPublicKey (541)
	#		K 	--> cmBpiPlusCmCertificate (1571)
	#		L 	--> cmBpiPlusCaCertificate (2475)
	
	Keyrowlist = ['H','I','J','K','L']

	# Get table values
	if Fcontent.cell('D1').value == "Broad No.":
		if Fcontent.cell('F1').value == "CM MAC":
			for x in range(2, 7):
				maclist.insert(nMac,Fcontent.cell('F'+ str(x)).value)
				nMac+=1
			for y in Keyrowlist:
				keylist.insert(nKey,Fcontent.cell(y + '2').value)
				nKey+=1
		else:
			print 'No " CM MAC " title'
	else:
		print 'No " Broad NO." title'

# write broad number to excel
def WriteExcel(f_name):
	print 'Write Broad NO: '
		


# Set frequnecy & coef & tuner1
def WriteFreq():
	print 'Write frequnecy & coef & Tuner1 in Console'
	Conn.write("cd /"+"\n")
	Conn.write("\n")
	Conn.write("/doc/sc"+"\n")
	Conn.write("\n")
	while True:
	#	if buf == "":
	#		Conn.write("\n")
		if buf.find("CM>") == 0:
			Conn.write("cd non/ds" + "\n")
			if buf.find("CM/NonVol/Ds Cal NonVol> ") == 0:
				print 'Enter in DS Cal NonVol folder'
				Conn.write("num_freq 17" + "\n")
				time.sleep(0.5)
				Conn.write("yes" + "\n")
				time.sleep(0.5)
				Conn.write("write" + "\n")
				time.sleep(0.5)
				Conn.write("yes" + "\n")
				time.sleep(0.5)
				for freq in freqlist:
					Conn.write(freq + "\n")
					time.sleep(0.1)
				Conn.write("write" + "\n")
				Conn.write("yes" + "\n")
				print 'write freq done'
				for coef in coeflist:
					Conn.write(coef + "\n")
					time.sleep(0.1)
				Conn.write("write" + "\n")
				Conn.write("yes" + "\n")
				print 'write coef done'
				for tuner1 in Tuner1list:
					Conn.write(tuner1 + "\n")
					time.sleep(0.1)
				Conn.write("write" + "\n")
				Conn.write("yes" + "\n")
				print 'write Tuner1 done'
				break
		
# Set Mac Address
def WriteMac(nMac, nKey):
	print 'Start to Write MAC in Console'
	Conn.write("cd /"+"\n")
	if buf.find("CM> ") == 0:
	#if maclist != '' && keylist != '':
		Conn.write("cd non/hal" + "\n")
		if buf.find("CM/NonVol/HalIf NonVol>") == 0:
			mac_num = 1
			for mac in maclist:
				if mac_num < 6:
					Conn.write("mac_address "+ str(mac_num) + " " + mac + "\n")
					mac_num += 1
					time.sleep(0.2)
			Conn.write("write" + "\n")
			Conn.write("yes" + "\n")
			print 'Set mac address down'

'''def SnmpCmd():
	print 'Snmp Command to Set MAC and Key'
	for snmpcmdopen in SnmpCommandOpen:
		os.system(snmpcmdopen)
	for snmpcmdkey in SnmpCommandKeyList:
		os.system(snmpcmdkey + )

	for snmpcmdmac in SnmpCommandMacList:
		os.system(snmpcmdmac +)

	for snmpcmdclose in SnmpCommandClose:
		os.system(snmpcmdclose)
'''
buf=''
def Read_thread(Conn):
	Conn.setRtsCts(0)
	print 'Start to read....'
	data = ''
	while True:
		#data = Conn.read(100)
		#data = Conn.readline()
		data = Conn.readall()
		#bytesToRead = Conn.inWaiting()
		#buf = Conn.read(bytesToRead)
		print data
		#time.sleep(0.5)

# Continue Reading 
#thread = threading.Thread(target=Read_thread, args=(Conn,))
#thread.start()
#WriteFreq()
#WriteMac()
#Conn.close()
ReadExcel(filename, cMACNum, KEYNum)
print maclist
#print keylist

'''
def UpgradeFirmware(Conn):
	print 'Start to upgrade firmware'
	Conn.write("/res" + "\n")
	print 'restart'
	time.sleep(7)
	Conn.write(CMBroadComPwd + "\n")
	if buf.find("  within 2 seconds or take default...") == -1:
	  	print 'Enter in upgrade mode'
		Conn.write("p")


UpgradeFirmware(Conn)
'''







