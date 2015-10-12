OS           := $(shell uname)
ARCH         := $(shell uname -m)

# Name of your package.
# The shared library that will be built will get the name lib$(PACKAGE).so
PACKAGE      := g2p

USERLIB      := lib$(PACKAGE).so
USERDICT     := $(PACKAGE)Dict

CXX          := g++
CXXFLAGS     := -Wall -fPIC -O3 -g
LD           := g++
LDFLAGS      := -O3 -g
SOFLAGS      := -shared

ifndef ANALYZER
  $(error $$ANALYZER environment variable not defined)
endif

ANADIRS      := $(wildcard $(addprefix $(ANALYZER)/, src hana_decode hana_scaler))
ANACFLAGS    := $(addprefix -I,$(ANADIRS))
ANALIBS      := -L$(ANALYZER) -lHallA -ldc -lscaler

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

SYSLIBS      := -lstdc++ -lm

CXXFLAGS     += $(ROOTCFLAGS) $(ANACFLAGS)
INCFLAGS     += $(ANACFLAGS)
LIBS         += $(ROOTLIBS) $(ANALIBS) $(SYSLIBS)
GLIBS        += $(ROOTGLIBS) $(ANALIBS) $(SYSLIBS)

SRCDIR       := src

CSOURCES     := $(wildcard $(SRCDIR)/*.[Cc])
CSOURCES     += $(wildcard $(SRCDIR)/*.[Cc][Cc])
CSOURCES     += $(wildcard $(SRCDIR)/*.[Cc][XxPp][XxPp])
SOURCES      := $(CSOURCES)
HEADERS      := $(foreach n,$(subst :, ,$(SRCDIR)),$(wildcard $(n)/*.h))
OBJS         := $(addsuffix .o, $(basename $(SOURCES)))

all: $(USERLIB)

$(USERLIB):	$(OBJS) $(USERDICT).o
	$(LD) $(LDFLAGS) $(SOFLAGS) -o $@ $^ $(LIBS)

$(USERDICT).o: $(USERDICT).cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(USERDICT).cxx: $(HEADERS) $(PACKAGE)_LinkDef.h
	$(ROOTSYS)/bin/rootcint -f $@ -c $(INCFLAGS) $^

clean:
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*~ 
	rm -f *.o *~
	rm -f $(USERLIB) $(USERDICT).*

$(SRCDIR)/%.o:	$(SRCDIR)/%.cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<
