# list all binaries in this next line
BINARIES = randwav
CCOPTS = -g -O2

all:	${BINARIES}

# randwav
randwav:	randwav.c
	gcc ${CCOPTS} $< -o $@

