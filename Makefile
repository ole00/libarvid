SDK_DIR := ../pru_sdk
OUT := build

CFLAGS += -O2 -Wall -Werror -I$(SDK_DIR)/include -I./include

LDLIBS += -L$(SDK_DIR)/lib -lpthread -lprussdrv 
LNFLAGS := $(LDLIBS)

OFILES := \
		$(OUT)/libarvid.o \
		$(OUT)/blitter.o \
		$(OUT)/ser_screen.o \
		$(OUT)/rates.o

ALL_TARGETS := \
		init \
		simple_test \
		framerate_test \
		arvid-server \
		arvid-splash \
		pruvideo_240.bin \
		pruvideo_256.bin \
		pruvideo_320.bin \
		pruvideo_288.bin \
		pruvideo_384.bin \
		pruvideo_392.bin \
		pruvideo_400.bin \
		prustream.bin


all: $(ALL_TARGETS)

init:
	mkdir -p $(OUT)

clean:
	rm -f example *.o *.bin *.lst *.a
	rm -rf $(OUT)


%.bin: src/pru/%.p
	pasm -b $^

prucode.lst: prucode.p
	pasm -l $^

simple_test:  $(OUT)/simple.o libarvid.a
	@echo "linking: $@"
	gcc $(LNFLAGS) -o $@ $^

framerate_test:  $(OUT)/framerate.o libarvid.a
	@echo "linking: $@"
	gcc $(LNFLAGS) -o $@ $^

arvid-server:  $(OUT)/arvid-server.o libarvid.a
	@echo "linking: $@"
	gcc $(LNFLAGS) -lz -o $@ $^

arvid-splash:  $(OUT)/arvid-splash.o libarvid.a
	@echo "linking: $@"
	gcc $(LNFLAGS) -lpng -o $@ $^


libarvid.a: $(OFILES)
	@echo "arch: $@"
	@ar rcs $@ $^

$(OUT)/%.o: src/%.c
	@echo "compiling: $<"
	@gcc $(CFLAGS) -c $< -o $@

$(OUT)/%.o: test/%.c
	@echo "compiling: $<"
	@gcc $(CFLAGS) -c $< -o $@


run: example
	./example $(args)