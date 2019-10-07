.PHNOY : clean

CC = gcc

CFLAGS = -g -lm -Wall -I cli  -I include -I parser -I vm -I object -I gc -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -fgnu89-inline

TARGET = newlan

DIRS = cli include parser vm object gc

CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS = $(patsubst %.c, %.o, $(CFILES))

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean :
	-$(RM) $(TARGET) $(OBJS)
	
rebuild: clean $(TARGET)