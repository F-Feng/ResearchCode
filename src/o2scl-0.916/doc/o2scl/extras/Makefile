# Makefile.in generated by automake 1.14.1 from Makefile.am.
# doc/o2scl/extras/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994-2013 Free Software Foundation, Inc.

# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.




am__is_gnu_make = test -n '$(MAKEFILE_LIST)' && test -n '$(MAKELEVEL)'
am__make_running_with_option = \
  case $${target_option-} in \
      ?) ;; \
      *) echo "am__make_running_with_option: internal error: invalid" \
              "target option '$${target_option-}' specified" >&2; \
         exit 1;; \
  esac; \
  has_opt=no; \
  sane_makeflags=$$MAKEFLAGS; \
  if $(am__is_gnu_make); then \
    sane_makeflags=$$MFLAGS; \
  else \
    case $$MAKEFLAGS in \
      *\\[\ \	]*) \
        bs=\\; \
        sane_makeflags=`printf '%s\n' "$$MAKEFLAGS" \
          | sed "s/$$bs$$bs[$$bs $$bs	]*//g"`;; \
    esac; \
  fi; \
  skip_next=no; \
  strip_trailopt () \
  { \
    flg=`printf '%s\n' "$$flg" | sed "s/$$1.*$$//"`; \
  }; \
  for flg in $$sane_makeflags; do \
    test $$skip_next = yes && { skip_next=no; continue; }; \
    case $$flg in \
      *=*|--*) continue;; \
        -*I) strip_trailopt 'I'; skip_next=yes;; \
      -*I?*) strip_trailopt 'I';; \
        -*O) strip_trailopt 'O'; skip_next=yes;; \
      -*O?*) strip_trailopt 'O';; \
        -*l) strip_trailopt 'l'; skip_next=yes;; \
      -*l?*) strip_trailopt 'l';; \
      -[dEDm]) skip_next=yes;; \
      -[JT]) skip_next=yes;; \
    esac; \
    case $$flg in \
      *$$target_option*) has_opt=yes; break;; \
    esac; \
  done; \
  test $$has_opt = yes
am__make_dryrun = (target_option=n; $(am__make_running_with_option))
am__make_keepgoing = (target_option=k; $(am__make_running_with_option))
pkgdatadir = $(datadir)/o2scl
pkgincludedir = $(includedir)/o2scl
pkglibdir = $(libdir)/o2scl
pkglibexecdir = $(libexecdir)/o2scl
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = x86_64-apple-darwin13.3.0
host_triplet = x86_64-apple-darwin13.3.0
subdir = doc/o2scl/extras
DIST_COMMON = $(srcdir)/Makefile.in $(srcdir)/Makefile.am \
	$(dist_docextras_DATA)
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/m4/libtool.m4 \
	$(top_srcdir)/m4/ltoptions.m4 $(top_srcdir)/m4/ltsugar.m4 \
	$(top_srcdir)/m4/ltversion.m4 $(top_srcdir)/m4/lt~obsolete.m4 \
	$(top_srcdir)/acinclude.m4 $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
CONFIG_CLEAN_VPATH_FILES =
AM_V_P = $(am__v_P_$(V))
am__v_P_ = $(am__v_P_$(AM_DEFAULT_VERBOSITY))
am__v_P_0 = false
am__v_P_1 = :
AM_V_GEN = $(am__v_GEN_$(V))
am__v_GEN_ = $(am__v_GEN_$(AM_DEFAULT_VERBOSITY))
am__v_GEN_0 = @echo "  GEN     " $@;
am__v_GEN_1 = 
AM_V_at = $(am__v_at_$(V))
am__v_at_ = $(am__v_at_$(AM_DEFAULT_VERBOSITY))
am__v_at_0 = @
am__v_at_1 = 
SOURCES =
DIST_SOURCES =
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = f=`echo $$p | sed -e 's|^.*/||'`;
am__install_max = 40
am__nobase_strip_setup = \
  srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*|]/\\\\&/g'`
am__nobase_strip = \
  for p in $$list; do echo "$$p"; done | sed -e "s|$$srcdirstrip/||"
am__nobase_list = $(am__nobase_strip_setup); \
  for p in $$list; do echo "$$p $$p"; done | \
  sed "s| $$srcdirstrip/| |;"' / .*\//!s/ .*/ ./; s,\( .*\)/[^/]*$$,\1,' | \
  $(AWK) 'BEGIN { files["."] = "" } { files[$$2] = files[$$2] " " $$1; \
    if (++n[$$2] == $(am__install_max)) \
      { print $$2, files[$$2]; n[$$2] = 0; files[$$2] = "" } } \
    END { for (dir in files) print dir, files[dir] }'
am__base_list = \
  sed '$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;s/\n/ /g' | \
  sed '$$!N;$$!N;$$!N;$$!N;s/\n/ /g'
am__uninstall_files_from_dir = { \
  test -z "$$files" \
    || { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
    || { echo " ( cd '$$dir' && rm -f" $$files ")"; \
         $(am__cd) "$$dir" && rm -f $$files; }; \
  }
am__installdirs = "$(DESTDIR)$(docextrasdir)"
DATA = $(dist_docextras_DATA)
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) $(LISP)
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/missing aclocal-1.14
AMTAR = $${TAR-tar}
AM_DEFAULT_VERBOSITY = 1
AR = ar
AUTOCONF = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/missing autoconf
AUTOHEADER = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/missing autoheader
AUTOMAKE = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/missing automake-1.14
AWK = awk
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2
CPP = gcc -E
CPPFLAGS = -I ~/local/include
CXX = g++
CXXCPP = g++ -E
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -g -O2
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
DLLTOOL = false
DSYMUTIL = dsymutil
DUMPBIN = 
ECHO_C = \c
ECHO_N = 
ECHO_T = 
EGREP = /usr/bin/grep -E
EXEEXT = 
FGREP = /usr/bin/grep -F
GREP = /usr/bin/grep
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = $(install_sh) -c -s
LD = /opt/local/bin/ld
LDFLAGS = -L /Users/Kyle/local/lib
LIBOBJS = 
LIBS = -lgsl -lcblas 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIPO = lipo
LN_S = ln -s
LTLIBOBJS = 
MAKEINFO = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/missing makeinfo
MANIFEST_TOOL = :
MKDIR_P = ../../.././install-sh -c -d
NM = /opt/local/bin/nm
NMEDIT = nmedit
OBJDUMP = false
OBJEXT = o
OLIB_LT_VERSION = 6:0:6
OTOOL = otool
OTOOL64 = :
PACKAGE = o2scl
PACKAGE_BUGREPORT = awsteiner@users.sourceforge.net
PACKAGE_NAME = o2scl
PACKAGE_STRING = o2scl 0.916
PACKAGE_TARNAME = o2scl
PACKAGE_URL = 
PACKAGE_VERSION = 0.916
PATH_SEPARATOR = :
POW_LIB = 
RANLIB = ranlib
SED = /usr/bin/sed
SET_MAKE = 
SHELL = /bin/sh
STRIP = strip
VERSION = 0.916
abs_builddir = /Users/Kyle/local/src/o2scl-0.916/doc/o2scl/extras
abs_srcdir = /Users/Kyle/local/src/o2scl-0.916/doc/o2scl/extras
abs_top_builddir = /Users/Kyle/local/src/o2scl-0.916
abs_top_srcdir = /Users/Kyle/local/src/o2scl-0.916
ac_ct_AR = ar
ac_ct_CC = gcc
ac_ct_CXX = g++
ac_ct_DUMPBIN = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = $${TAR-tar} chof - "$$tardir"
am__untar = $${TAR-tar} xf -
bindir = ${exec_prefix}/bin
build = x86_64-apple-darwin13.3.0
build_alias = 
build_cpu = x86_64
build_os = darwin13.3.0
build_vendor = apple
builddir = .
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host = x86_64-apple-darwin13.3.0
host_alias = 
host_cpu = x86_64
host_os = darwin13.3.0
host_vendor = apple
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = ${SHELL} /Users/Kyle/local/src/o2scl-0.916/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = $(MKDIR_P)
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /Users/Kyle/local
program_transform_name = s,x,x,
psdir = ${docdir}
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
srcdir = .
sysconfdir = ${prefix}/etc
target_alias = 
top_build_prefix = ../../../
top_builddir = ../../..
top_srcdir = ../../..
docextrasdir = $(prefix)/doc/o2scl/extras
dist_docextras_DATA = *.png *.cpp *.html *.txt *.odg

#ex_fptr.png: 
#	cd ../../../examples; make o2scl-examples
#	cat ../../../examples/ex_fptr.scr | grep -v brent | grep -v value \
#		> temp1
O2SCL_LIB_DIR = /home/asteiner/install/lib
O2SCL_INC_DIR = /home/asteiner/install/include
READLINE_LIBS = -lreadline -lncurses
FLAGS = -ggdb -Wreturn-type -Wparentheses -Wconversion -Wall \
	-Wno-unused -Wshadow -Wimplicit -I$(O2SCL_INC_DIR) \
	-DO2SCL_ARRAY_ABORT


# This ordering must be maintained for the static compilation to work
LIB = -lm -L$(O2SCL_LIB_DIR) -lo2int_base -lo2scl_ext -lo2scl_eos \
	-lo2scl_part -lo2scl_root -lo2scl_other -lo2scl_nuclei \
	-lo2scl_minimize -lo2scl_inte -lo2scl_base -lgsl -lgslcblas \
	$(READLINE_LIBS)

ROOT_INC = -I/home/asteiner/root/include
ROOT_LIB = /home/asteiner/svn/o2int/trunk/src/other/graph.o \
	-L/home/asteiner/root/lib \
	-lgsl -lgslcblas -lm -lCore -lCint -lHist -lGraf \
	-lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lGeom \
	-lPhysics -lGui -lm -ldl -lpthread -rdynamic

all: all-am

.SUFFIXES:
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu doc/o2scl/extras/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --gnu doc/o2scl/extras/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(am__aclocal_m4_deps):

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs
install-dist_docextrasDATA: $(dist_docextras_DATA)
	@$(NORMAL_INSTALL)
	@list='$(dist_docextras_DATA)'; test -n "$(docextrasdir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(docextrasdir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(docextrasdir)" || exit 1; \
	fi; \
	for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  echo "$$d$$p"; \
	done | $(am__base_list) | \
	while read files; do \
	  echo " $(INSTALL_DATA) $$files '$(DESTDIR)$(docextrasdir)'"; \
	  $(INSTALL_DATA) $$files "$(DESTDIR)$(docextrasdir)" || exit $$?; \
	done

uninstall-dist_docextrasDATA:
	@$(NORMAL_UNINSTALL)
	@list='$(dist_docextras_DATA)'; test -n "$(docextrasdir)" || list=; \
	files=`for p in $$list; do echo $$p; done | sed -e 's|^.*/||'`; \
	dir='$(DESTDIR)$(docextrasdir)'; $(am__uninstall_files_from_dir)
tags TAGS:

ctags CTAGS:

cscope cscopelist:


distdir: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*) $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d "$(distdir)/$$file"; then \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -fpR $(srcdir)/$$file "$(distdir)$$dir" || exit 1; \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    cp -fpR $$d/$$file "$(distdir)$$dir" || exit 1; \
	  else \
	    test -f "$(distdir)/$$file" \
	    || cp -p $$d/$$file "$(distdir)/$$file" \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(DATA)
installdirs:
	for dir in "$(DESTDIR)$(docextrasdir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	if test -z '$(STRIP)'; then \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	      install; \
	else \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	    "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install; \
	fi
mostlyclean-generic:

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || test -z "$(CONFIG_CLEAN_VPATH_FILES)" || rm -f $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libtool mostlyclean-am

distclean: distclean-am
	-rm -f Makefile
distclean-am: clean-am distclean-generic

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am: install-dist_docextrasDATA

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am:

install-html: install-html-am

install-html-am:

install-info: install-info-am

install-info-am:

install-man:

install-pdf: install-pdf-am

install-pdf-am:

install-ps: install-ps-am

install-ps-am:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-generic mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-dist_docextrasDATA

.MAKE: install-am install-strip

.PHONY: all all-am check check-am clean clean-generic clean-libtool \
	cscopelist-am ctags-am distclean distclean-generic \
	distclean-libtool distdir dvi dvi-am html html-am info info-am \
	install install-am install-data install-data-am \
	install-dist_docextrasDATA install-dvi install-dvi-am \
	install-exec install-exec-am install-html install-html-am \
	install-info install-info-am install-man install-pdf \
	install-pdf-am install-ps install-ps-am install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-generic \
	mostlyclean-libtool pdf pdf-am ps ps-am tags-am uninstall \
	uninstall-am uninstall-dist_docextrasDATA


# *.eps 

help:
	@echo
	@echo "File descriptions:"
	@echo "--------------------------------------------------------"
	@echo "contourg2.eps: Picture for contour documentation "
	@echo "contourg2.png: Picture for contour documentation "
	@echo "contourg3.eps: Picture for contour documentation "
	@echo "contourg3.png: Picture for contour documentation "
	@echo "contourg.eps: Picture for contour documentation "
	@echo "contourg.png: Picture for contour documentation "
	@echo "desc.png: Description for HTML front page"
	@echo "dl.png: Download button for HTML front page"
	@echo "doc.png: Documentation button for HTML front page"
	@echo "fdl_license.txt: GNU Doc license"
	@echo "fptr_doc.cpp: Docs for function pointers used in main.h"
	@echo "gpl_license.txt: GNU GPL"
	@echo "gsl_vector_doc.cpp: Docs for gsl_vector used in main.h"
	@echo "index.html: HTML front page"
	@echo "io_doc1.cpp: Docs for I/O used in main.h"
	@echo "io_doc2.cpp: Docs for I/O used in main.h"
	@echo "io_doc4.cpp: Docs for I/O used in main.h"
	@echo "io_doc5.cpp: Docs for I/O used in main.h"
	@echo "io_doc6.cpp: Docs for I/O used in main.h"
	@echo "otitle.odg: OpenOffice base file for HTML images"
	@echo "otitle.png: Title for HTML front page"
	@echo "ovector_doc1.cpp: Docs for ovector used in main.h"
	@echo "ovector_doc2.cpp: Docs for ovector used in main.h"
	@echo "ovector_doc3.cpp: Docs for ovector used in main.h"
	@echo "ovector_doc4.cpp: Docs for ovector used in main.h"
	@echo

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
