CC=clang++

FLAGS=  -O2 \
		-std=c++11 \
		-mmacosx-version-min=10.10 \
		-I/usr/local/Cellar/root/5.34.24/include/root \
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

%.o: %.cpp $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

playground: $(OBJS) playground.o
	$(CC) $(FLAGS) -o $@ $^

tests: $(OBJS) tests.o
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) $(OBJS)
