SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

libwarp.a: $(OBJECTS)
	$(AR) crs $@ $^

%.o: %.c
	$(CC) -c -std=c11 -Wall -I. -pedantic -O2 -o $@ $<

.PHONY: clean
clean:
	@$(RM) libwarp.a *.o
