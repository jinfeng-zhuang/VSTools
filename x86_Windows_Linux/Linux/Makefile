SRC += ../lib/chip_id.cpp
SRC += ../lib/dbg_client.cpp
SRC += ../lib/file_append.cpp
SRC += ../lib/file_save.cpp
SRC += ../lib/hexdump.cpp
SRC += ../lib/log.cpp
SRC += ../lib/mpegformat.cpp
SRC += ../lib/param_parser.cpp
SRC += ../lib/tcp_client.cpp

SRC += ../3rdparty/getopt9/getopt.c

SRC += ../app/avlog/main.cpp

INC += ../inc

avlog: $(SRC)
	g++ -o $@ $^ -I $(INC) -fpermissive
