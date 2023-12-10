
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

run4.system:
	gcc -std=c17 ./4.system.c -o build/4.system -g && ./build/4.system


run5.filetable:
	gcc -std=c17 ./5.filetable.c -o build/5.filetable -g && ./build/5.filetable

run6.pthreads:
	gcc -std=c17 ./6.pthreads.c  -lpthread -o build/6.pthreads -g && ./build/6.pthreads