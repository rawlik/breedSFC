CC=clang++

FLAGS= `root-config --cflags` -std=c++11
LINK_FLAGS= `root-config --glibs` -lGenVector

DEPS = breedSFC.h
PROGRAMS = playground tests
PROGRAMS_OBJS = $(foreach X,$(PROGRAMS),$(X).o)
OBJS = breedSFC.o

all: $(PROGRAMS)

# compile object files
%.o: %.cpp $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

# link each program
# a program depends on program.o and other objects
$(PROGRAMS): %: %.o $(OBJS)
	$(CC) $(LINK_FLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) $(PROGRAMS_OBJS) $(OBJS)
