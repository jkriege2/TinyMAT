.PHONY = clean all pdf

OS := $(shell uname -o)


ifeq ($(findstring Msys,$(OS)),Msys)
PREFIX=/mingw
EXE_SUFFIX=.exe
SO_SUFFIX=.dll
SO_PATH=$(PREFIX)/bin
else
PREFIX=/usr/local
EXE_SUFFIX=
SO_SUFFIX=.so
SO_PATH=$(PREFIX)/lib
endif



SRC_TEST = ../tinymatwriter.cpp test_tinymat.cpp


TAR_TEST = $(subst .cpp,.o,$(SRC_TEST))

CC = g++

OPTIMIZATION_OPTS = -O2 
LD_OPTS = -lm
CC_OPTS = -Wall $(OPTIMIZATION_OPTS) 

debug: CC_OPTS += -DDEBUG -g



debug: test_tinymat$(EXE_SUFFIX)
all: test_tinymat$(EXE_SUFFIX)

-include $(TAR_STATIC_LIB:.o=.d) $(TAR_TEST:.o=.d)


$(TAR_TEST): %.o: %.cpp
	$(CC) $(CC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp


test_tinymat$(EXE_SUFFIX): $(TAR_TEST)
	$(CC) $(CC_OPTS) $(LD_OPTS) $(TAR_TEST) -o $@



clean:
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f lib$(LIB_NAME).a
	-rm -f test_tinymat
	-rm -f *.exe
	-rm -f $(TAR_TEST)
