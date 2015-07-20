.PHONY : clean,mrproper

CXX = g++
LIB = libtmxme.a
LIBH = src/map.hpp
AR = ar
LDFLAGS = 
CXXFLAGS = -std=c++11

all : $(LIB)

$(LIB) : mapEvent.o mapMov.o mapShow.o fonctionsUtiles.o map.o
	$(AR) -q $(LIB) $^
%.o : src/%.cpp $(LIBH)
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean :
	rm -rf *.o
mrproper : clean
	rm $(LIB)
