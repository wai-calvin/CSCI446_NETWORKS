
EXE = p-counter
CFLAGS = -Wall

$(EXE): p-counter.c
	gcc $(CFLAGS) -o $(EXE) p-counter.c

clean:
	rm -f $(EXE)
