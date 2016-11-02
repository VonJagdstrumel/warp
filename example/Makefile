SOURCES    := $(wildcard *.c)
OBJECTS    := $(SOURCES:%.c=build/%.o)

example: $(OBJECTS)
	$(CC) -Wall -I. -s -o $@ $(OBJECTS)
	../warpize.pl . example.exe
	windres build/warp.rc build/_warp.o
	windres appinfo.rc build/_appinfo.o
	$(CC) -Wall -I. -s -o $@ build/_warp.o build/_appinfo.o -L.. -lwarp
	upx -q $@.exe

build/%.o : %.c mkdir
	$(CC) -c -std=c11 -Wall -I. -pedantic -O2 -o $@ $<

.PHONY: clean
clean:
	@$(RM) example build/*

.PHONY: mkdir
mkdir:
	@mkdir -p build
