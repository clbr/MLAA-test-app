LOCALLD = -Wl,-rpath,'$$ORIGIN' -s
LDFLAGS += -lIrrlicht $(LOCALLD)

CPPFLAGS += -Wall

.PHONY: clean

mlaatest: mlaatest.o

clean:
	rm -f mlaatest *.o
