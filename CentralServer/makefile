#----------------------------------------------------
# Don't move the files construction and modify the VPATH
#----------------------------------------------------

CC = g++

#VPATH = include


pre = -fPIC -g -Wall -D_REENTRANT  -fno-exceptions

CPPFLAGS =   -Iinclude\
	     -I/usr/local/Aria/include/ \
	     -I/usr/local/Aria/ArNetworking/include \
	     -I/usr/local/Arnl/include

	     
LINKFLAGS += -Linclude -L/usr/local/Aria/lib -lAria -lpthread -ldl -lrt \
		-lArNetworking\
		-L/usr/local/Arnl/lib -lArnl -lBaseArnl\
		 `pkg-config opencv --libs` `pkg-config opencv --cflags` 

 #-lArNetworkingForArnl -lAriaForArnl -lpthread -ldl -lrt -Xlinker -Bstatic -lstdc++ -Xlinker -Bdynamic -lpthread -ldl -lrt 

OBJECTS += KeyPTZ.o VideoServer.o MotionsServer.o

RecogMove: $(OBJECTS)
	$(CC) $(pre) $(CPPFLAGS) serverMain.cpp -o main  $(OBJECTS) $(LINKFLAGS)
	#g++ -fPIC -g -Wall -D_REENTRANT  -fno-exceptions vcc4Camera.cpp -o vcc4Camera -I/usr/local/Aria/include  -L/usr/local/Aria/lib -lAria -lpthread -ldl -lrt
