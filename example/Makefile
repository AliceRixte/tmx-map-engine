EXEC = Programme
CXX = g++
CXXFLAGS = -std=c++11 -I.
LDFLAGS = -L. -ltmxme -ltmx -ljansson -lz -lxml2 `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

all : $(EXEC)

$(EXEC) : main.o
	$(CXX) -o $(EXEC) main.o $(CXXFLAGS) $(LDFLAGS)
%.o : %.cpp ../src/map.hpp
	$(CXX) -c $< $(CXXFLAGS)

clean : 
	rm -rf *.o
mrproper : clean
	rm -rf $(EXEC)
