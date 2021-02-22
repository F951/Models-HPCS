# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh
#-------------------------------------------------------------------------------
#Modificar para que referencie a /src/
#-------------------------------------------------------------------------------
#srcdir =  ../../ 
srcdir =  ../src/ 
#top_srcdir = ../../
top_srcdir = ../src/ 
#-------------------------------------------------------------------------------
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/libcppsim
pkglibdir = $(libdir)/libcppsim
pkgincludedir = $(includedir)/libcppsim

#-------------------------------------------------------------------------------
top_builddir = ../../ 
#-------------------------------------------------------------------------------

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
CC = gcc
CXX = g++
MAKEINFO = makeinfo
PACKAGE = libcppsim
RANLIB = ranlib
VERSION = 0.2.1

bin_PROGRAMS = Simulador
EXTRA_DIST = qn

LDFLAGS :=  -static

INCLUDES = -I$(top_srcdir)/src

LDADD = ../../src/libcppsim.a ../../../openssl-1.0.2l/libssl.a ../../../openssl-1.0.2l/libcrypto.a

Simulador_SOURCES = Simulador.cc

TESTS = Simulador
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LIBS =

#----- MODIFY HERE -----------

Simulador_LDADD = $(LDADD)

Simulador_OBJECTS = obj/Simulador.o obj/util.o obj/gen_rnd.o obj/Observer.o obj/hash.o $(Layer1_OBJECTS) $(Layer2_OBJECTS) $(Layer3_OBJECTS) $(Layer4_OBJECTS) 

Layer1_OBJECTS = transportLayer/obj/redtubo.o #transportLayer/obj/MessageT.o
Layer2_OBJECTS = p2pLayer/obj/pastryProtocol.o p2pLayer/obj/leafset.o p2pLayer/obj/routingtable.o p2pLayer/obj/statebuilder.o
Layer3_OBJECTS = applicationLayer/obj/tlcProtocol.o applicationLayer/obj/lru.o applicationLayer/obj/lrulcache.o applicationLayer/obj/inlink.o applicationLayer/obj/clienttlcwse.o applicationLayer/obj/replicator.o applicationLayer/obj/amortiguador.o  # applicationLayer/obj/Query.o

Layer4_OBJECTS = wse/obj/wse.o wse/obj/lruA.o

Simulador_DEPENDENCIES =  $(LDADD)
Simulador_LDFLAGS = 
CXXFLAGS =  -g -Wall -O3 -ldl -lcrypto
#-pg
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(Simulador_SOURCES)
OBJECTS = $(Simulador_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .cc .o .s
#$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
#	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps test/Makefile

#Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
#	cd $(top_builddir) \
#	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	     $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	#-rm -f *.o obj/*.o applicationLayer/obj/*.o p2pLayer/obj/*.o transport/obj/*.o core *.core
	-rm -f *.o obj/*.o applicationLayer/obj/*.o applicationLayerLeafset/obj/*.o p2pLayer/obj/*.o transport/obj/*.o transportLayer/obj/*.o core *.core wse/obj/*.o
clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

Simulador: $(Simulador_OBJECTS) $(Simulador_DEPENDENCIES)
	@rm -f Simulador
	$(CXXLINK) $(Simulador_LDFLAGS) $(Simulador_OBJECTS) $(Simulador_LDADD) $(LIBS)
.cc.o:
	$(CXXCOMPILE) -c $<

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = test

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
#-------------------------------------------------------------------------------

obj/Simulador.o: Simulador.cc ../../src/cppsim.hh ../../src//process.hh \
	../../src//coroutine.hh ../..//config.h ../../src//handle.hh \
	../../src//logger.hh ../../src//sqs.hh ../../src//simulation.hh \
	../../src//sqsCal.hh ../../src//sqsDll.hh ../../src//sqsPrio.hh ../../src//mean.hh \
	../../src//statistic.hh ../../src//var.hh ../../src//repmean.hh \
	../../src//trremoval.hh ../../src//accum.hh ../../src//bmeans.hh \
	../../src//bmeans2.hh ../../src//counter.hh ../../src//dump.hh \
	../../src//histogram.hh ../../src//jain.hh ../../src//quantile.hh \
	../../src//welch.hh ../../src//rng.hh ../../src//assertions.hh ../../bigint/BigIntegerLibrary.hh
	g++ -g -O3 -c Simulador.cc -o obj/Simulador.o -Wno-deprecated -Wall
#-------------------------------------------------------------------------------
#---------- CAMBIAR ACA -------------------


obj/util.o: auxiliar/Util.cc auxiliar/Util.h
	g++ -g -O3 -c -o obj/util.o auxiliar/Util.cc  -Wno-deprecated -Wall
obj/hash.o: auxiliar/Hash.cc auxiliar/Hash.h	
	g++ -g -O3 -c -o obj/hash.o auxiliar/Hash.cc -Wno-deprecated -Wall 
obj/gen_rnd.o: generador/gen_rnd.cc generador/gen_rnd.h
	g++ -g -O3 -c -o obj/gen_rnd.o generador/gen_rnd.cc  -Wno-deprecated -Wall
obj/Observer.o: Observer.cc Observer.h generador/gen_rnd.h
	g++ -g -O3 -c -o obj/Observer.o Observer.cc -Wno-deprecated -Wall
transportLayer/obj/redtubo.o: transportLayer/redtubo.cc transportLayer/redtubo.h transportLayer/MessageT.h transportLayer/NodeT.h
	g++ -g -O3 -c -o transportLayer/obj/redtubo.o transportLayer/redtubo.cc -Wno-deprecated -Wall
p2pLayer/obj/pastryProtocol.o: p2pLayer/PastryProtocol.cc p2pLayer/PastryProtocol.h p2pLayer/PastryMessage.h p2pLayer/PastryNode.h
	g++ -g -O3 -c -o p2pLayer/obj/pastryProtocol.o p2pLayer/PastryProtocol.cc -Wno-deprecated -Wall
p2pLayer/obj/leafset.o: p2pLayer/Leafset.cc p2pLayer/Leafset.h auxiliar/Util.h
	g++ -g -O3 -c -o p2pLayer/obj/leafset.o p2pLayer/Leafset.cc -Wno-deprecated -Wall
p2pLayer/obj/routingtable.o: p2pLayer/RoutingTable.cc p2pLayer/RoutingTable.h auxiliar/Util.h
	g++ -g -O3 -c -o p2pLayer/obj/routingtable.o p2pLayer/RoutingTable.cc -Wno-deprecated -Wall
p2pLayer/obj/statebuilder.o: p2pLayer/StateBuilder.cc p2pLayer/StateBuilder.h
	g++ -g -O3 -c -o p2pLayer/obj/statebuilder.o p2pLayer/StateBuilder.cc -Wno-deprecated -Wall
applicationLayer/obj/lru.o: applicationLayer/Lru.cc applicationLayer/Lru.h applicationLayer/Entry.h
	g++ -g -O3 -c -o applicationLayer/obj/lru.o applicationLayer/Lru.cc  -Wno-deprecated -Wall 
applicationLayer/obj/lrulcache.o: applicationLayer/LruLCache.cc applicationLayer/LruLCache.h  
	g++ -g -O3 -c -o applicationLayer/obj/lrulcache.o applicationLayer/LruLCache.cc -Wno-deprecated -Wall 
applicationLayer/obj/inlink.o: applicationLayer/InLink.cc applicationLayer/InLink.h
	g++ -g -O3 -c -o applicationLayer/obj/inlink.o applicationLayer/InLink.cc -Wno-deprecated -Wall
applicationLayer/obj/tlcProtocol.o: applicationLayer/TlcProtocol.cc applicationLayer/TlcProtocol.h applicationLayer/Query.h applicationLayer/TlcMessage.h applicationLayer/Replicas.h applicationLayer/ClientTLCWSE.h 
	g++ -g -O3 -c -o applicationLayer/obj/tlcProtocol.o applicationLayer/TlcProtocol.cc  -Wno-deprecated -Wall
applicationLayer/obj/clienttlcwse.o: applicationLayer/ClientTLCWSE.cc applicationLayer/ClientTLCWSE.h applicationLayer/TlcProtocol.h
	g++ -g -O3 -c -o applicationLayer/obj/clienttlcwse.o applicationLayer/ClientTLCWSE.cc -Wno-deprecated -Wall
wse/obj/wse.o: wse/WSE.cc wse/WSE.h wse/MessageWSE.h wse/Answer.h wse/LruA.h 
	g++ -g -O3 -c -o wse/obj/wse.o wse/WSE.cc -Wno-deprecated -Wall
wse/obj/lruA.o: wse/LruA.cc wse/LruA.h auxiliar/Util.h
	g++ -g -O3 -c -o wse/obj/lruA.o wse/LruA.cc -Wno-deprecated -Wall
applicationLayer/obj/replicator.o: applicationLayer/Replicator.cc applicationLayer/Replicator.h
	g++ -g -O3 -c -o applicationLayer/obj/replicator.o applicationLayer/Replicator.cc -Wno-deprecated -Wall
applicationLayer/obj/amortiguador.o: applicationLayer/Amortiguador.cc applicationLayer/Amortiguador.h
	g++ -g -O3 -c -o applicationLayer/obj/amortiguador.o applicationLayer/Amortiguador.cc -Wno-deprecated -Wall

#COMPILACION CACHE
check-TESTS: $(TESTS)
	@failed=0; all=0; \
	srcdir=$(srcdir); export srcdir; \
	for tst in $(TESTS); do \
	  if test -f $$tst; then dir=.; \
	  else dir="$(srcdir)"; fi; \
	  if $(TESTS_ENVIRONMENT) $$dir/$$tst; then \
	    all=`expr $$all + 1`; \
	    echo "PASS: $$tst"; \
	  elif test $$? -ne 77; then \
	    all=`expr $$all + 1`; \
	    failed=`expr $$failed + 1`; \
	    echo "FAIL: $$tst"; \
	  fi; \
	done; \
	if test "$$failed" -eq 0; then \
	  banner="All $$all tests passed"; \
	else \
	  banner="$$failed of $$all tests failed"; \
	fi; \
	dashes=`echo "$$banner" | sed s/./=/g`; \
	echo "$$dashes"; \
	echo "$$banner"; \
	echo "$$dashes"; \
	test "$$failed" -eq 0
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-binPROGRAMS
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-binPROGRAMS
uninstall: uninstall-am
all-am: Makefile $(PROGRAMS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(bindir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-binPROGRAMS mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-binPROGRAMS clean-compile clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-binPROGRAMS distclean-compile distclean-tags \
		distclean-generic clean-am

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-binPROGRAMS \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-binPROGRAMS distclean-binPROGRAMS clean-binPROGRAMS \
maintainer-clean-binPROGRAMS uninstall-binPROGRAMS install-binPROGRAMS \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir check-TESTS info-am info \
dvi-am dvi check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
