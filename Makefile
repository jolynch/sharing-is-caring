# The sources we're building
# Sources without mains
GENERIC_SOURCES = sic.c sic-internals.c sic-util.c network.c sic-message.pb-c.c
HEADERS = sic.h sic-internals.h network.h sic-util.h sic-server.h sic-message.pb-c.h google/protobuf-c/protobuf-c.h google/protobuf-c/protobuf-c-rpc.h google/protobuf-c/protobuf-c-dispatch.h google/protobuf-c/protobuf-c-private.h

TESTBARRIER_SOURCES = test-barriers.c

TESTMALLOC_SOURCES = test-malloc.c

SERVER_SOURCES = sic-server.c 

# What we're building

PRODUCT = protos test-barriers server test-malloc 
GENERIC_OBJECTS = $(patsubst %.c,%.o,$(GENERIC_SOURCES))
SERVER_OBJECTS = $(patsubst %.c,%.o,$(SERVER_SOURCES))

TESTBARRIER_OBJECTS =  $(patsubst %.c,%.o,$(TESTBARRIER_SOURCES))

TESTMALLOC_OBJECTS =  $(patsubst %.c,%.o,$(TESTMALLOC_SOURCES))


# What we're building with
CC = gcc 
CFLAGS = -Wall
LDFLAGS = -lpthread -lprotobuf-c

ifeq ($(DEBUG),1)
# We want debug mode.
CFLAGS += -gdwarf-3 -O0
else
# We want release mode.
CFLAGS += -O3 -DNDEBUG -gdwarf-3
endif

all: $(PRODUCT) 

protos: sic-message.proto
	protoc-c --c_out=. sic-message.proto

server: $(SERVER_OBJECTS) $(GENERIC_OBJECTS)
	$(CC) $(GENERIC_OBJECTS) $(SERVER_OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@

test-barriers: $(TESTBARRIER_OBJECTS) $(GENERIC_OBJECTS)
	$(CC) $(GENERIC_OBJECTS) $(TESTBARRIER_OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@

test-malloc: $(TESTMALLOC_OBJECTS) $(GENERIC_OBJECTS)
	$(CC) $(GENERIC_OBJECTS) $(TESTMALLOC_OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@

#test-%: $(TEST%_OBJECTS) $(GENERIC_OBJECTS)
#	$(CC) $(GENERIC_OBJECTS) $(TEST%_OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@

%.o:    %.c $(HEADERS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<


#demo: sighandlerdemo.o mprotectdemo.o exec-mpdemo.o
#	$(CC) demos/sighandlerdemo.o $(CFLAGS) $(EXTRA_CFLAGS) -o sigdemo 
#	$(CC) demos/mprotectdemo.o $(CFLAGS) $(EXTRA_CFLAGS) -o mpdemo 
#	$(CC) demos/exec-mpdemo.o $(CFLAGS)  $(EXTRA_CFLAGS) -o execdemo

clean:
	$(RM) sigdemo mpdemo execdemo *.o 	
