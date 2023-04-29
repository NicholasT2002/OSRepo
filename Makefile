CC=gcc
DEBUG=-g -D_DEBUG
DEFINE=-DMODULE -D__KERNEL__ -DLINUX
WARNINGS=-Wall -Wmissing-prototypes -Wmissing-declarations
CC_OPTIONS=-O1 $(WARNINGS) $(DEBUG) $(DEFINE)

# Where to look for header files
INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include

DRIVER=mapDriver.o
MODULE=mapDriver.ko
EXE=mapDriver-test
OBJ=main.o $(DRIVER)

SERVER=mapServer.c
CLIENT=mapClient.c
SERV_EXE=server
CLNT_EXE=client

obj-m += $(DRIVER)

all: $(EXE)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo ""
	@echo "mapDriver Character Device Driver and the Test Program have been built."
	@echo "Type 'sudo make register' to register mapDriver device module."
	@echo ""

clean:
	rm -f $(EXE) $(OBJ)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

compile: $(EXE) $(OBJ)

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep mapDriver
	`dmesg | grep mknod | uniq | grep mapDriver | sed 's/<minor>/1/g; s/^[[:space:]]*//' | uniq`
	chmod 666 /dev/mapDriver
	@echo ""
	@echo "ASCII Character Device Driver has been built and registered."
	@echo ""

$(EXE): main.o
	$(CC) main.o -o $(EXE)

main.o: main.c common.h
	$(CC) $(CC_OPTIONS) $(INC) -c main.c

$(DRIVER): types.h mapDriver.h mapDriver.c
	$(CC) $(CC_OPTIONS) $(INC) -c mapDriver.c

clean-all:
	make clean
	sudo rmmod mapDriver
	sudo rm /dev/mapDriver

test:
	./$(EXE) testFile

createSocket:
	$(CC) -g -Wall $(SERVER) -o $(SERV_EXE)
	$(CC) -g -Wall $(CLIENT) -o $(CLNT_EXE)

runSocketTest:
	./$(SERV_EXE) &
	sudo netstat -plnt | grep server
	for number in 1 2 3 4 5 6 7 8 9 10; do \
	./$(CLNT_EXE) ; \
	done

cleanSocket:
	pidof $(SERV_EXE) | xargs kill
	rm $(SERV_EXE)
	rm $(CLNT_EXE)
	sudo netstat -plnt | grep server
