code : main.cpp
	g++ -o code main.cpp memory.cpp operator.cpp storeage.cpp -O2 -std=c++14

clean :
	rm -rf code
