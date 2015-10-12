OS           := $(shell uname)
ARCH         := $(shell arch)

# Name of your package.
# The shared library that will be built will get the name lib$(PACKAGE).so
PACKAGE      := g2p

USERLIB       = lib$(PACKAGE).so
USERDICT      = $(PACKAGE)Dict

ifndef ANALYZER
  $(error $$ANALYZER environment variable not defined)
endif

INCDIRS       = $(wildcard $(addprefix $(ANALYZER)/, include src hana_decode hana_scaler))

LIBDIR       := $(ANALYZER)
HALLALIBS    := -L$(LIBDIR) -lHallA -ldc -lscaler

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

SYSLIBS      := -lstdc++

INCLUDES      = $(ROOTCFLAGS) $(addprefix -I, $(INCDIRS))

CXX           = g++
CXXFLAGS      = -O3
LDFLAGS       = -O3
DEFINES       = -DLINUXVERS -DHAS_SSTREAM
CXXFLAGS     += -Wall -Woverloaded-virtual -fPIC -fno-strict-aliasing
LD            = g++
SOFLAGS       = -shared

CXXFLAGS     += $(DEFINES) $(INCLUDES)
LIBS         += $(ROOTLIBS) $(HALLALIBS) $(SYSLIBS)
GLIBS        += $(ROOTGLIBS) $(HALLALIBS) $(SYSLIBS)

SRCDIR       := src

CSOURCES     := $(wildcard $(SRCDIR)/*.[Cc])
CSOURCES     += $(wildcard $(SRCDIR)/*.[Cc][Cc])
CSOURCES     += $(wildcard $(SRCDIR)/*.[Cc][XxPp][XxPp])
SOURCES      := $(CSOURCES)
HEADERS      := $(foreach n,$(subst :, ,$(SRCDIR)),$(wildcard $(n)/*.h))
OBJS         := $(addsuffix .o, $(basename $(SOURCES)))

all:		$(USERLIB)

$(USERLIB):	$(OBJS) $(USERDICT).o
	$(LD) $(LDFLAGS) $(SOFLAGS) -o $@ $(OBJS) $(USERDICT).o $(LIBS)

$(USERDICT).o: $(USERDICT).cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(USERDICT).cxx: $(HEADERS) $(PACKAGE)_LinkDef.h
	$(ROOTSYS)/bin/rootcint -f $@ -c $(INCLUDES) $^

clean:
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*~ 
	rm -f *.o *~
	rm -f $(USERLIB) $(USERDICT).*

$(SRCDIR)/%.o:	$(SRCDIR)/%.cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<
