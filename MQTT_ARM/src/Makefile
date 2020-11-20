cc  = arm-linux-gcc 
cflags = -lmosquitto -lpthread -ldl  -luuid -lssl -lcrypto 
cflags += -I/home/edu/tools/mqtt_arm/mosquitto_arm/home/edu/mosquitto_arm/include 
cflags += -L/home/edu/tools/mqtt_arm/mosquitto_arm/home/edu/mosquitto_arm/lib 
cflags += -L/home/edu/tools/mqtt_arm/openssl_arm/lib
cflags += -L/home/edu/tools/mqtt_arm/uuid_arm/lib


main:
	$(cc) mqtt_demo.c -o demo $(cflags)
	
.phony:clean
clean:
	rm *.o demo -fr

