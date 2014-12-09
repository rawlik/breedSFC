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

PROGRAMS = breedSFC

all: $(PROGRAMS)

breedSFC: breedSFC.cpp breedSFC.h
	$(CC) $(FLAGS) -o $@ $<

clean:
	rm -f $(PROGRAMS)
