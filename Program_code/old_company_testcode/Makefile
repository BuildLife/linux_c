objects = miController.o DHCP_test.o
cc = gcc


all:miController.c DHCP_test.c
	gcc -pthread -o miController_S miController.c -lpcap -lbsd
	gcc -pthread -o DHCP_test_S DHCP_test.c -lpcap -lbsd

clean: 
	rm -f miController_S 
	rm -f DHCP_test_S
	#rm -f $(objects)
