# $Id: Makefile,v 20.14 2001/10/19 06:21:44 al Exp $

all: pdf dvi html

pdf:
	pdflatex man
dvi:
	latex man
html:
	latex2html -toc_depth 1 -link 1 -split 4 -local_icons \
		 -image_type png  man.tex
checkin:
	ci -u`cat rcsversion` -m. -t/dev/null *.tex */*.tex Makefile
