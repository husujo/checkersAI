CPPFILES=$(wildcard *.cpp)
HPPFILES=$(wildcard *.hpp)

client: $(HPPFILES)
	g++ -O3 -std=c++14 $(CPPFILES) -o client

clean:
	rm -f client

