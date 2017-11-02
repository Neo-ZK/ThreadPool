ThreadPoolTest: main.cpp 
		g++ main.cpp -o ThreadPoolTest -g -pthread
.PHONY:clean
clean:
	-rm -f ThreadPoolTest
