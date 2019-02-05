<<<<<<< HEAD
all:
	gcc -pthread DNS_Client.c DNS_Query.c DNS_Encode.c CyoEncode.c -o DNS_Client

clean:
	rm -f DNS_Client
=======
TARGET=DNS_Encode

OBJS=${TARGET}.o

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(TARGET).o
	@echo "| Create executable object: "
	@echo -n "| "
	gcc -pthread -o $@ $^
	@echo "\--------------------------------------\n"

$(TARGET).o: $(TARGET).c
	@echo "/--------------------------------------"
	@echo "| Compilation: "
	@echo -n "| "
	gcc -pthread -Wall -c $<
	@echo "|"
>>>>>>> b314e18594536042d83fb0c0060edfc8f8df9219
