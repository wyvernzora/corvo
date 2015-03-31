CXX      = g++ -fPIC
LIB      = -lpthread
DEBUG    = 4
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

all: http-serve

dev: clean http-serve
	./http-serve

http-serve: main.o request.o response.o server.o util.o path.o puppet.o plumber.o
	$(CXX) -o http-serve *.o $(LIB)

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I. $<

clean:
	rm -f *.o http-serve
