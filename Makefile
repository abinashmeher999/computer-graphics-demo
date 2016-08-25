all: 3ddss 3ddtr
	node index.js

3ddss: 3ddss.cpp 3ddss.hpp
	g++ -std=c++11 3ddss.cpp -o 3ddss

3ddtr: 3dtriangle.cpp 3ddss.hpp
	g++ -std=c++11 3dtriangle.cpp -o 3ddtr

clean:
	rm 3ddss 3ddtr
