tm: tm.cpp
	$(CXX) -std=c++0x -Wall $< -o $@
clean:
	rm -f tm
