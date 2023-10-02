
run0.cpp:
	g++ -std=c++17 ./0.check.cpp -o ./build/0.check -g && ./build/0.check

run0.c:
	gcc -std=c17 ./21.arrayinfun.c -o build/21.arrayinfun -g && ./build/21.arrayinfun


run1.one:
	gcc -std=c17 ./1.one.c -o build/1.one -g && ./build/1.one
run1.oneexec:
	gcc -std=c17 ./1.oneexec.c -o build/1.oneexec -g && ./build/1.oneexec

run2.pipe:
	gcc -std=c17 ./2.pipe.c -o build/2.pipe -g && ./build/2.pipe


run3.pipe:
	gcc -std=c17 ./3.pipe.c -o build/3.pipe -g && ./build/3.pipe
run3.pipeexec:
	gcc -std=c17 ./3.pipeexec.c -o build/3.pipeexec