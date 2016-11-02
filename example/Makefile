SOURCES    := $(wildcard *.c)
OBJECTS    := $(SOURCES:%.c=build/%.o)
RESOBJS    := build/_warp.o build/_appinfo.o

example: build/warp.rc
	windres build/warp.rc build/_warp.o
	windres appinfo.rc build/_appinfo.o
	$(CC) -Wall -I. -s -o $@ $(RESOBJS) -L.. -lwarp
	upx -q $@.exe

build/warp.rc: build/example
	../warpize.pl . example.exe

build/example: $(OBJECTS)
	$(CC) -Wall -I. -s -o $@ $(OBJECTS)

build/%.o : %.c mkdir
	$(CC) -c -std=c11 -Wall -I. -pedantic -O2 -o $@ $<

.PHONY: clean
clean:
	@$(RM) example build/*

.PHONY: mkdir
mkdir:
	@mkdir -p build
