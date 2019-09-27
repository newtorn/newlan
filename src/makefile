.PHNOY : clean

CC = gcc

CFLAGS = -g -lm -Wall -I cli -I utils -I include -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -fgnu89-inline

TARGET = newlan

DIRS = utils cli include

CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS = $(patsubst %.c, %.o, $(CFILES))

TARGET : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean :
	-$(RM) $(TARGET) $(OBJS)
