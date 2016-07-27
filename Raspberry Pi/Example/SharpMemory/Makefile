

CXX = g++
CFLAGS = -Wall -c
LFLAGS = -Wall
LDFLAGS =
LDLIBS = -l bcm2835
EXECNAME = test

OBJS = SharpMemory.o main.o

all: $(EXECNAME)

$(EXECNAME): $(OBJS)
	$(CXX) $(LFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

main.o: main.cc SharpMemory.h
	$(CXX) $(CFLAGS) main.cc $(CLIBS) -o $@

SharpMemory.o: SharpMemory.h SharpMemory.cc
	$(CXX) $(CFLAGS) SharpMemory.cc $(CLIBS) -o $@

clean:
	@rm -f $(OBJS) *~ \#*

clean-all: clean
	@rm -f $(EXECNAME)
