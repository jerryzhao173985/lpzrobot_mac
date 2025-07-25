#File:     Makefile for selforg, the controller for self-organized behavior
#Author:   Georg Martius  <martius@informatik.uni-leipzig.de>
#Date:     November 2005

INCLUDEDIR=include/selforg
# this is the command to come from the include dir back to the base
REVINCLUDEDIR=../..

# the CFGOPTS are set by the opt and dbg target
CFGOPTS=
LIB := $(shell ./selforg-config $(CFGOPTS) --srcprefix="." --libfile)
SHAREDLIB=$(shell ./selforg-config $(CFGOPTS) --srcprefix="." --solibfile)
TYPE=$(shell ./selforg-config --type)

STRIP=no

SELFORG=.
PREFIX=$(shell ./selforg-config $(CFGOPTS) --prefix)
# for debian auto-package installation
ifneq ($(DESTDIR),)
PREFIX=$(DESTDIR)
endif
include Makefile.conf

# was -o, see what it does in Msys
ifeq ($(shell uname -s),Msys)
REVINCLUDEDIR=.
endif

BUILD_DIR=build

find_cpp_files = $(wildcard $(dir)/*.cpp)
CPPFILES   := $(filter-out %flymake.cpp,$(foreach dir,$(dirs),$(find_cpp_files)))
find_h_files = $(wildcard $(dir)/*.h)
HFILES   := $(filter-out %flymake.h,$(foreach dir,$(dirs),$(find_h_files)))
OFILES     := $(patsubst %.cpp,${BUILD_DIR}/%.o, $(CPPFILES))

UTILS = selforg-config

# additional inc paths
INC +=  -Iinclude -Iinclude/selforg

# use -pg for profiling
CPPFLAGS = -Wall -pipe -fpic $(INC) $(shell ./selforg-config --srcprefix="." $(CFGOPTS) --intern --cflags)

# Platform detection for shared library building
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    SHARED_LIB_FLAGS = -dynamiclib
else
    SHARED_LIB_FLAGS = -shared -Wl,-soname,$(notdir $(SHAREDLIB))
endif

# used for single file compilation
CXX = g++

# used for lib-packing
AR = ar -rcs

.PHONY: lib opt dbg clean clean-all distclean todo depend tags install install_lib uninstall uninstall_lib

libs: lib opt dbg
	$(MAKE) shared
# the shared lib cannot be done as a dependency because with -j 2 it collides with the lib target

lib: $(LIB)

opt: $(UTILS)
	$(MAKE) BUILD_DIR=build_opt CFGOPTS=--opt STRIP="yes" lib

dbg: $(UTILS)
	$(MAKE) BUILD_DIR=build_dbg CFGOPTS=--dbg lib

shared: $(UTILS)
	$(MAKE) $(SHAREDLIB)


$(LIB):  $(UTILS) Makefile.depend $(OFILES)
	$(AR) $(LIB) $(OFILES)
	@if [ "$(STRIP)" = "yes" ]; then strip -S $(LIB); fi


${BUILD_DIR}/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) -o "$@" "$<"

$(SHAREDLIB): Makefile.depend $(OFILES)
	$(CXX) $(SHARED_LIB_FLAGS) -o $(SHAREDLIB) $(OFILES) $(shell gsl-config --libs) -lm -lreadline -lncurses -lpthread

# Pattern rule for any .so file (in case of direct invocation)
%.so: Makefile.depend $(OFILES)
	$(CXX) $(SHARED_LIB_FLAGS) -o $@ $(OFILES) $(shell gsl-config --libs) -lm -lreadline -lncurses -lpthread


install_scripts:
	@echo "*************** Install config script $(PREFIX)/bin *********************"
	install -d $(PREFIX)/bin
	install -m 755 selforg-config $(PREFIX)/bin/

install_lib:
	install -m 644 $(LIB) $(PREFIX)/lib/


install: Makefile.depend install_scripts
ifneq ($(TYPE),DEVEL)
	@echo "*************** Install libs and includes $(PREFIX)*********************"
	install -d $(PREFIX)/lib/ $(PREFIX)/include/selforg $(PREFIX)/share/lpzrobots/selforg
	$(MAKE) install_lib
	$(MAKE) CFGOPTS=--dbg install_lib
	$(MAKE) CFGOPTS=--opt install_lib
	$(MAKE) LIB=$(SHAREDLIB) install_lib
	install -m 644 include/selforg/*.h $(PREFIX)/include/selforg
	@echo "*************** Install example simulations ******************"
	cp -RL simulations $(PREFIX)/share/lpzrobots/selforg/
endif


uninstall_lib:
	-rm -f $(PREFIX)/lib/$(LIB)

uninstall:
	-rm -f $(PREFIX)/bin/selforg-config
ifneq ($(TYPE),DEVEL)
	$(MAKE) uninstall_lib
	$(MAKE) CFGOPTS=--dbg uninstall_lib
	$(MAKE) CFGOPTS=--opt uninstall_lib
	$(MAKE) LIB=$(SHAREDLIB) uninstall_lib
	-rm -rf $(PREFIX)/include/selforg
	-rm -rf $(PREFIX)/share/lpzrobots/selforg/
endif


Makefile.depend:
	for file in $(HFILES); do \
		ln -sf $(REVINCLUDEDIR)/$$file $(INCLUDEDIR)/; \
	done
	makedepend -- $(CPPFLAGS) -- $(CPPFILES) -p'$$(BUILD_DIR)/' -f- > Makefile.depend 2>/dev/null

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



.PHONY: search
##!'search'           		ask for a search string and scans all tex files
search:
	echo -n "search rexp: "; read F && grep -in "$$F" ${CPPFILES} ${HFILES}

##!'find'           		scans all tex files for the pattern stored in the file "tofind"
.PHONY: find
find:
	@[ -e "tofind" ] &&  grep -f tofind -n  ${CPPFILES} ${HFILES}


##!'findi'           		case insensitive version of target `find'
.PHONY: findi
findi:
	@[ -e "tofind" ] &&  grep -f tofind -in  ${CPPFILES} ${HFILES}

distclean: clean-all
clean-all: clean

clean:
	rm -f Makefile.depend
	rm -rf build build_dbg build_opt
	rm -f $(SHAREDLIB)
	rm -f $(shell ./selforg-config --srcprefix="." --libfile)
	rm -f $(shell ./selforg-config --opt --srcprefix="." --libfile)
	rm -f $(shell ./selforg-config --dbg --srcprefix="." --libfile)
	find $(INCLUDEDIR) -type l -exec rm \{\} \;

todo:
	@grep -ni "Todo"  $(CPPFILES) $(HFILES)
	@grep -ni "Fixme"  $(CPPFILES) $(HFILES)


Makefile.conf.generated:
	@echo "run ./configure first!"
	@exit 1;

selforg-config: selforg-config.m4
	$(error "call ./configure or make all in lpzrobots again!")

include Makefile.depend
