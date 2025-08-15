LIB_DIR=./lib
# 确保目录存在
$(shell mkdir -p $(LIB_DIR))

all: $(LIB_DIR)/libftp.a $(LIB_DIR)/libftp.so

# 静态库 (.a)
$(LIB_DIR)/libftp.a: $(LIB_DIR)/ftplib.o
	ar rcs $@ $^

# 动态库 (.so)
$(LIB_DIR)/libftp.so: $(LIB_DIR)/ftplib.pic.o
	gcc -g -shared -o $@ $^

# 普通目标文件（用于静态库）
$(LIB_DIR)/ftplib.o: ftplib.c ftplib.h
	gcc -g -c -o $@ $<

# 位置无关代码（PIC，用于动态库）
$(LIB_DIR)/ftplib.pic.o: ftplib.c ftplib.h
	gcc -g -fPIC -c -o $@ $<

.PHONY:clean
clean:
	rm -rf $(LIB_DIR)/*