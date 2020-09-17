include Install/Core/Makefile.Inc

INC_PATH := Install/Core/Inc
LIB_PATH := -L Install/Core/Lib -L Install/Core/SHLib

CFLAGS := $(LIB_PATH) -I $(INC_PATH) -lpng

zhuang: main.c
	$(CC) -o $@ $< $(CFLAGS)
