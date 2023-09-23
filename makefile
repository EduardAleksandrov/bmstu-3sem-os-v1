
run0.c:
	g++ -std=c++17 ./0.check.cpp -o ./build/0.check -g && ./build/0.check

run0.cpp:
	gcc -std=c17 ./21.arrayinfun.c -o build/21.arrayinfun -g && ./build/21.arrayinfun