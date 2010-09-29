## $Id$
##

SUBDIRS= ${TEXDIRS}

pkgmanualdir= ${pkgdatadir}/manual
dist_pkgmanual_DATA= gnucap-man.pdf

EXTRA_DIST= ${TEXSRCS} Makefile.template configure.old

TEXSRCS= \
	addmodel.tex behavior.tex circuit.tex commands.tex copying.tex \
	gnucap-man.tex install.tex start.tex tech.tex \
	title.tex 

MOSTLYCLEANFILES = \
	gnucap-man.aux gnucap-man.idx gnucap-man.log gnucap-man.out \
	gnucap-man.toc gnucap-man.haux gnucap-man.hidx gnucap-man.hind \
	gnucap-man.htoc gnucap-man.ilg gnucap-man.ind

CLEANFILES = $(MOSTLYCLEANFILES) \
	gnucap-man.dvi gnucap-man.txt info html

DISTCLEANFILES = $(CLEANFILES)

MAINTAINERCLEANFILES = $(DISTCLEANFILES) \
	gnucap-man.pdf

TEXDIRS= Addmodel Behavior Circuit Commands Tech

TEXFILES= ${TEXSRCS} 
TEXFILES+= $(foreach __tmp__,$(TEXDIRS),$(wildcard $(srcdir)/$(__tmp__)/*.tex))

DVIPDFM=	@DVIPDFM@
HACHA=		@HACHA@
HEVEA=		@HEVEA@ -I $(srcdir)
LATEX=		@LATEX@

gnucap-man.pdf: gnucap-man.dvi
if MISSING_DVIPDFM
	@echo "WARNING:  dvipdfm was not found on your system but"
	@echo "          $@ is out of date and needs to be"
	@echo "          rebuilt.  Changes to gnucap-man.dvi"
	@echo "          will be ignored."
else
	${DVIPDFM} gnucap-man
endif

# run it twice to fix the toc.
gnucap-man.dvi: $(TEXFILES)
if MISSING_LATEX
	@echo "WARNING:  latex was not found on your system but"
	@echo "          $@ is out of date and needs to be"
	@echo "          rebuilt from the LaTeX .tex files. Changes to"
	@echo "          the .tex files will be ignored."
else
	touch gnucap-man.ind
	env TEXINPUTS=$(srcdir): ${LATEX} -interaction=nonstopmode \
		$(srcdir)/gnucap-man |grep -v hyperref |grep .
	${MAKEINDEX} gnucap-man
	env TEXINPUTS=$(srcdir): ${LATEX} -interaction=nonstopmode \
		$(srcdir)/gnucap-man |grep -v hyperref |grep .
endif

# html version of the manual
html/index.html: $(TEXFILES)
if MISSING_HEVEA
	@echo "WARNING:  Either hevea or hacha was not found on your system but"
	@echo "          $@ is out of date and needs to be"
	@echo "          rebuilt from the LaTeX .tex files. Changes to"
	@echo "          the .tex files will be ignored."
else
	-mkdir html
	${HEVEA} -fix -o html/gnucap-man $(srcdir)/gnucap-man.tex
	cd html; ${HACHA} -nolinks gnucap-man; rm -f gnucap-man *.haux *.htoc
endif


# info version of the manual
info/gnucap-man.info: $(TEXFILES)
if MISSING_HEVEA
	@echo "WARNING:  Either hevea or hacha was not found on your system but"
	@echo "          $@ is out of date and needs to be"
	@echo "          rebuilt from the LaTeX .tex files. Changes to"
	@echo "          the .tex files will be ignored."
else
	-mkdir info
	${HEVEA} -fix -info -o $@ $(srcdir)/gnucap-man.tex
endif

# plain text
gnucap-man.txt:  $(TEXFILES)
if MISSING_HEVEA
	@echo "WARNING:  Either hevea or hacha was not found on your system but"
	@echo "          $@ is out of date and needs to be"
	@echo "          rebuilt from the LaTeX .tex files. Changes to"
	@echo "          the .tex files will be ignored."
else
	${HEVEA} -fix -text -o $@ $(srcdir)/gnucap-man.tex
endif

