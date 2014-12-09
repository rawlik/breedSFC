CC=clang++

FLAGS=  -O2 \
		-std=c++11 \
		-mmacosx-version-min=10.10 \
		-I/usr/local/Cellar/root/5.34.24/include/root

LINK_FLAGS= $(FLAGS) \
			-L/usr/local/Cellar/root/5.34.24/lib/root \
			-lCore \
			-lCint \
			-lRIO \
			-lNet \
			-lHist \
			-lGraf \
			-lGraf3d \
			-lGpad \
			-lTree \
			-lRint \
			-lPostscript \
			-lMatrix \
			-lPhysics \
			-lMathCore \
			-lThread \
			-lpthread \
			-lm \
			-ldl

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
