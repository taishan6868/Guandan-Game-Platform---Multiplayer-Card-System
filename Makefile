TARGETNAME = AuthenServer
TARGETPATH = ../../bin/AuthenServer
# 0 -- Application; 1 -- Static library; 2 -- Dynamic library
TARGETTYPE = 0
TARGETSRCS = $(wildcard *.c *.cpp)
#TARGETSRCS = main.cpp
TARGETINCS = -I../include -I../../lib/include
TARGETLIBS = ../../lib/lib/GameProvider.a ../../lib/lib/Epoll.a ../../lib/lib/Common.a ../../lib/lib/libaes.a ../../lib/lib/ODBCExt.a

CPP = g++
DEBUG = -g -DDEBUG -D_REENTRANT
RELEASE = -s -O2
CPPFLAGS  = $(TARGETINCS)
LINKFLAGS = $(TARGETLIBS) -lpthread -lrt -lnsl -lgcc_s -lodbc
OBJS = $(patsubst %.c,%.o,$(TARGETSRCS:.cpp=.o))

ifeq ($(strip $(TARGETPATH)),)
    TARGETPATH := .
endif

ifneq ($(strip $(MAKECMDGOALS)), release)
    CPPFLAGS := $(CPPFLAGS) $(DEBUG)
else
    CPPFLAGS := $(CPPFLAGS) $(RELEASE)
endif

ifneq ($(TARGETTYPE), 1)
    ifeq ($(TARGETTYPE), 2)
        TYPE = -shared
        TARGETNAME := $(TARGETNAME).so
    endif
    $(TARGETNAME) : $(OBJS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(CPP) $(CPPFLAGS) -o $(TARGETPATH)/$@ $^ $(LINKFLAGS) $(TYPE)
else
    TYPE = -rcs
    TARGETNAME := $(TARGETNAME).a
    $(TARGETNAME) : $(OBJS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(AR) cq $(TARGETPATH)/$@ $^
    $(OBJS) : $(TARGETSRCS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(CPP) $(CPPFLAGS) -c $^
endif

.PHONY: all debug release clean

all debug release: $(TARGETNAME)

clean:
	rm -f *.o $(TARGETPATH)/$(TARGETNAME) $(TARGETPATH)/core*
