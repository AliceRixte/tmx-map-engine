.PHONY : clean,mrproper

CXX = g++
LIB = example/libtmxme.a
LIBH = src/tme.hpp
AR = ar
LDFLAGS = 
CXXFLAGS = -std=c++11 -I/src

all : $(LIB)

$(LIB) : mapEvent.o mapMov.o mapShow.o mapCore.o
	$(AR) -q $(LIB) $^
%.o : src/%.cpp $(LIBH)
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean :
	rm -rf *.o
mrproper : clean
	rm $(LIB)
