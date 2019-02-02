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