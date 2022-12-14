#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.2 2005/01/18 08:46:02 boonkerz Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#


# IF YOU DON'T HAVE PROBLEMS WITH "IMLIB2" USE INSTEAD "HAVE_IMLIB2=1".
#HAVE_MAGICK=1
#HAVE_IMLIB2=1
HAVE_XPM=1

# UNCOMMENT THIS , IF YOU OWN A MODDED 4MB FF-CARD
# HAVE_4MB=1


PLUGIN = weatherng

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).h | awk '{ print $$6 }' | sed -e 's/[";]//g')
APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR ?= ../../../../DVB
VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

ifdef HAVE_IMLIB2
	DEFINES += -DHAVE_IMLIB2
	LIBS += -lImlib2
endif

ifdef HAVE_MAGICK
	DEFINES += -DHAVE_MAGICK
	LIBS += -lMagick -lMagick++
endif

ifdef HAVE_XPM
	DEFINES += -DHAVE_XPM
endif

ifdef HAVE_4MB
	DEFINES += -DHAVE_4MB
endif

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

OBJS =  img/bitmap.o img/quantize.o\
	\
	$(PLUGIN).o xml.o parsing.o OsdWeather.o setup.o i18n.o vars.o http.o\
	 

### Implicit rules:

%.o: %.c

	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o$@ $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
