PUBINC = -I../../common -I../../common/lib
PUBCPP=../../common/common.cpp

all:process_control process_monitor file_cleaner file_compressor ftp_getfiles ftp_putfiles tcp_putfiles_client tcp_server tcp_getfiles_client

process_control:process_control.cpp
	g++  -g -o $@ $^ -std=c++17 
	cp -f process_control ../bin/.

process_monitor:process_monitor.cpp
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) $(PUBCPP) 
	cp -f process_monitor ../bin/.

file_cleaner:file_cleaner.cpp
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) $(PUBCPP) 
	cp -f file_cleaner ../bin/.

file_compressor:file_compressor.cpp
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) $(PUBCPP) 
	cp -f file_compressor ../bin/.

ftp_getfiles:ftp_getfiles.cpp $(PUBCPP) ../../common/ftp.cpp  ../../common/lib/libftp.a
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) 
	cp -f ftp_getfiles ../bin/.

ftp_putfiles:ftp_putfiles.cpp $(PUBCPP) ../../common/ftp.cpp  ../../common/lib/libftp.a
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) 
	cp -f ftp_putfiles ../bin/.

tcp_putfiles_client:tcp_putfiles_client.cpp $(PUBCPP)
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) 
	cp -f tcp_putfiles_client ../bin/.

tcp_getfiles_client:tcp_getfiles_client.cpp $(PUBCPP)
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) 
	cp -f tcp_getfiles_client ../bin/.

tcp_server:tcp_server.cpp $(PUBCPP)
	g++  -g -o $@ $^ -std=c++17  $(PUBINC) 
	cp -f tcp_server ../bin/.
.PHONY: clean
clean:
	rm -f process_control  process_monitor file_cleaner file_compressor ftp_getfiles ftp_putfiles tcp_putfiles_client tcp_server
	rm -f tcp_getfiles_client
	