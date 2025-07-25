#File:     Makefile for ode_robot simulation framwwork
#Author:   Georg Martius  <martius@informatik.uni-leipzig.de>
#Date:     June 2005


SELFORGCFG=`if type selforg-config >/dev/null 2>&1; then echo "selforg-config"; elif [ -x ../selforg/selforg-config ]; then echo "../selforg/selforg-config"; else echo "Cannot find selforg-config" 1>&2; fi`


INCLUDEDIR=include/ode_robots
# this is the command to come from the include dir back to the base
REVINCLUDEDIR=../..

# the CFGOPTS are set by the opt and dbg target
CFGOPTS=
LIB := $(shell ./ode_robots-config $(CFGOPTS) --srcprefix="." --libfile)
SHAREDLIB=$(shell ./ode_robots-config $(CFGOPTS) --srcprefix="." --solibfile)
TYPE=$(shell ./ode_robots-config --type)

STRIP=no
# this can is overwritten from commandline
PREFIX=$(shell ./ode_robots-config $(CFGOPTS) --prefix)
# for debian auto-package installation
# this may not work....?
ifneq ($(DESTDIR),)
PREFIX=$(DESTDIR)
endif
MAKEDEPEND=makedepend

ifeq ($(TYPE),DEVEL)
SELFORG=$(shell $(SELFORGCFG) $(CFGOPTS) --srcprefix)
endif

include Makefile.conf

BUILD_DIR=build

find_files = $(wildcard $(dir)/*.cpp)
CPPFILES   := $(filter-out %flymake.cpp, $(foreach dir,$(dirs),$(find_files)))
OFILES     := $(patsubst %.cpp,${BUILD_DIR}/%.o, $(CPPFILES))
find_h_files = $(wildcard $(dir)/*.h)
HFILES   := $(filter-out %flymake.h, $(foreach dir,$(dirs),$(find_h_files)))

UTILS = install_prefix.conf ode_robots-config

# additional inc paths
INC +=  -I. -Iinclude -Iinclude/ode_robots -I$(shell $(SELFORGCFG) $(CFGOPTS) --srcprefix)/include
# INC += -include allincludes.h

# use -pg for profiling
CPPFLAGS = -Wall -pipe -fpic $(INC) $(shell $(SELFORGCFG) $(CFGOPTS) --cflags) \
 $(shell ./ode_robots-config --srcprefix="." $(CFGOPTS) --intern --cflags)

# used for single file compilation
CXX = g++

# used for lib-packing
AR = ar -rcs

.PHONY: lib opt clean clean-all distclean todo depend tags search find findi install install_scripts install_lib uninstall uninstall_lib

libs: lib opt dbg
	$(MAKE) shared
# the shared lib cannot be done as a dependency because with -j 2 it collides with the lib target


lib: $(UTILS)
ifeq ($(TYPE),DEVEL)
	cd $(SELFORG) && $(MAKE) lib
endif
	$(MAKE) library

dbg: $(UTILS)
ifeq ($(TYPE),DEVEL)
	cd $(SELFORG) && $(MAKE) dbg
endif
	$(MAKE) BUILD_DIR=build_dbg CFGOPTS=--dbg library

opt: $(UTILS)
ifeq ($(TYPE),DEVEL)
	cd $(SELFORG) && $(MAKE) opt
endif
	$(MAKE) BUILD_DIR=build_opt CFGOPTS=--opt STRIP="yes" library

shared:
ifeq ($(TYPE),DEVEL)
	cd $(SELFORG) && $(MAKE) shared
endif
	$(MAKE) $(SHAREDLIB)

library: $(LIB)


$(LIB): $(UTILS) Makefile.depend $(OFILES)
	$(AR) $(LIB) $(OFILES)
	@if [ "$(STRIP)" = "yes" ]; then strip --strip-debug $(LIB); fi

$(SHAREDLIB): Makefile.depend $(OFILES)
	@if [ `uname` = "Darwin" ]; then \
		$(CXX) -dynamiclib -install_name `basename $(SHAREDLIB)` -o $(SHAREDLIB) $(OFILES) \
		$(shell $(SELFORGCFG) $(CFGOPTS) --libs) \
		$(shell ./ode_robots-config --srcprefix="." $(CFGOPTS) --intern --libs | sed 's/-lode_robots//'); \
	else \
		$(CXX) -shared -Wl,-soname,`basename $(SHAREDLIB)` -o $(SHAREDLIB) $(OFILES) \
		$(shell $(SELFORGCFG) $(CFGOPTS) --libs) \
		$(shell ./ode_robots-config --srcprefix="." $(CFGOPTS) --intern --libs | sed 's/-lode_robots//'); \
	fi

${BUILD_DIR}/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) -o "$@" "$<"

#allincludes.h.gch: allincludes.h
#	@mkdir -p $(dir $@)
#	$(CXX) -c $(CPPFLAGS) $(CFLAGS) -o "$@" allincludes.h
#  find . -name "*.cpp" | xargs -n 1 grep "^#include <" | sort | uniq -c -s 10| grep -v "^\s*1"  | sed -e 's/\s*\w\w* //' > allincludes.h



utils/splitvideo: utils/splitvideo.c
	$(CXX) -o utils/splitvideo utils/splitvideo.c

#%.o: %.cpp
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $(OBJDIR)/$(@F)  $<

Makefile.depend:
	for file in $(HFILES); do \
		ln -sf $(REVINCLUDEDIR)/$$file $(INCLUDEDIR)/; \
	done
	$(MAKEDEPEND) -- $(CPPFLAGS) -- $(CPPFILES) -p'$$(BUILD_DIR)/' -f- > Makefile.depend 2>/dev/null

depend:
	rm Makefile.depend
	make Makefile.depend

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CPPFLAGS) -Wextra -S -fsyntax-only $(CHK_SOURCES)
	cppcheck --std=c++11 --std=posix --enable=performance,information,portability --suppress=incorrectStringBooleanError --suppress=invalidscanf --quiet --template={file}:{line}:{severity}:{message} $(CHK_SOURCES)


.PHONY: cppcheck
cppcheck:
	cppcheck --std=c++11 --std=posix --enable=performance,information,portability --suppress=incorrectStringBooleanError --suppress=invalidscanf  --quiet --template={file}:{line}:{severity}:{message} $(HFILES) $(CPPFILES)

# Workaroud for bug in etags: filenames should not have leading ./
CPPFILESNoDot := $(subst ./,,$(CPPFILES))
HFILESNoDot   := $(subst ./,,$(HFILES))
##!'tags'           		creates tags file for emacs
tags:
#	@etags --language-force="C++" $(HFILESNoDot) $(CPPFILESNoDot)
#	@etags --language-force="C++" -a -o ../TAGS ${HFILESNoDot} ${CPPFILESNoDot}
	@etags --language="c++" $(HFILESNoDot) $(CPPFILESNoDot)
	@etags --language="c++" -a -o ../TAGS ${HFILESNoDot} ${CPPFILESNoDot}

##!'search'           		ask for a search string and scans all tex files
search:
	echo -n "search rexp: "; read F && grep -in "$$F" ${CPPFILES} ${HFILES}


##!'find'           		scans all tex files for the pattern stored in the file "tofind"
find:
	@[ -e "tofind" ] &&  grep -f tofind -n  ${CPPFILES} ${HFILES}


##!'findi'           		case insensitive version of target `find'
findi:
	@[ -e "tofind" ] &&  grep -f tofind -in  ${CPPFILES} ${HFILES}


distclean: clean-all
clean-all: clean

clean:
	rm -f Makefile.depend
	rm -rf build build_dbg build_opt
	rm -f $(SHAREDLIB)
	rm -f $(shell ./ode_robots-config --srcprefix="." --libfile)
	rm -f $(shell ./ode_robots-config --opt --srcprefix="." --libfile)
	rm -f $(shell ./ode_robots-config --dbg --srcprefix="." --libfile)
	find $(INCLUDEDIR) -type l -exec rm \{\} \;


install_scripts:
	@echo "*************** Install binaries and scripts to $(PREFIX)/bin **********"
	install -d $(PREFIX)/bin
	install -m 755 ode_robots-config $(PREFIX)/bin/
	install -m 755 utils/feedfile.pl $(PREFIX)/bin/
	install -m 755 utils/feedfile_with_videocommands.pl $(PREFIX)/bin/
	install -m 755 utils/encodevideo.sh $(PREFIX)/bin/
	install -m 755 utils/writeTextToFrames.sh $(PREFIX)/bin/
	install -m 755 utils/selectcolumns.pl $(PREFIX)/bin/

install_lib:
	install -m 644 $(LIB) $(PREFIX)/lib/

install: Makefile.depend install_scripts
ifneq ($(TYPE),DEVEL)
	@echo "*************** Install libs and includes $(PREFIX)*********************"
	install -d  $(PREFIX)/lib $(PREFIX)/include/ode_robots
	$(MAKE) install_lib
	$(MAKE) CFGOPTS=--dbg install_lib
	$(MAKE) CFGOPTS=--opt install_lib
	$(MAKE) LIB=$(SHAREDLIB) install_lib
	install -m 644 include/ode_robots/*.h $(PREFIX)/include/ode_robots
endif
	install -d $(PREFIX)/share/lpzrobots $(PREFIX)/share/lpzrobots/ode_robots
	cp -R osg/data $(PREFIX)/share/lpzrobots/
ifneq ($(TYPE),DEVEL)
	@echo "*************** Install example simulations ******************"
	cp -RL simulations $(PREFIX)/share/lpzrobots/ode_robots/
	chmod -R ugo+r $(PREFIX)/share/lpzrobots
endif

uninstall_lib:
	-rm -f $(PREFIX)/lib/$(LIB)

uninstall:
ifneq ($(TYPE),DEVEL)
	$(MAKE) uninstall_lib
	$(MAKE) CFGOPTS=--dbg uninstall_lib
	$(MAKE) CFGOPTS=--opt uninstall_lib
	$(MAKE) LIB=$(SHAREDLIB) uninstall_lib
	-rm -rf $(PREFIX)/include/ode_robots
endif
	-rm -rf $(PREFIX)/share/lpzrobots/ode_robots/
	-rm -f $(PREFIX)/bin/ode_robots-config
	-rm -f $(PREFIX)/bin/feedfile.pl
	-rm -f $(PREFIX)/bin/encodevideo.sh
	-rm -f $(PREFIX)/bin/selectcolumns.pl
	-rm -rf $(PREFIX)/share/lpzrobots/data

install_prefix.conf:
	@echo "create install_prefix.conf"
	@echo '// for system installations (installation type:user) the macro PREFIX should be defined:' > install_prefix.conf
	@echo '// e.g. #define PREFIX "/usr/local"'  >> install_prefix.conf

todo:
	@grep -ni "Todo" $(CPPFILES) $(HFILES)
	@grep -ni "Fixme" $(CPPFILES) $(HFILES)

ode_robots-config: ode_robots-config.m4
	$(error "call ./configure or make all in lpzrobots again!")


-include Makefile.depend
