CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

TARGET = bonusHW

all: $(TARGET)

$(TARGET): bonusHW.c
	$(CC) $(CFLAGS) -o $(TARGET) bonusHW.c

clean:
	rm -f $(TARGET) output.txt