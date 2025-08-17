PUBINC=-I../../common
PUBCPP=../../common/common.cpp

all:crtsurfdata

crtsurfdata:crtsurfdata.cpp
	g++  -g -o $@ $^ -std=c++17 $(PUBINC) $(PUBCPP) 
	cp -f crtsurfdata ../bin/.

.PHONY: clean
clean:
	rm -f crtsurfdata 
	
