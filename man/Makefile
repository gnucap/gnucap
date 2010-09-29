# $Id: Makefile,v 24.15 2004/01/11 02:44:22 al Exp $

all: pdf dvi html

pdf: gnucap-man.pdf

dvi: gnucap-man.dvi

html: html/index.html

TEXFILES = \
addmodel.tex \
behavior.tex \
circuit.tex \
commands.tex \
copying.tex \
gnucap-man.tex \
gpl.tex \
install.tex \
start.tex \
tech.tex \
title.tex \
Addmodel/addmodel-base.tex \
Addmodel/addmodel-compile.tex \
Addmodel/addmodel.tex \
Behavior/complex.tex \
Behavior/conditional.tex \
Behavior/exp.tex \
Behavior/fit.tex \
Behavior/functions.tex \
Behavior/generator.tex \
Behavior/language.tex \
Behavior/poly.tex \
Behavior/posy.tex \
Behavior/proto.tex \
Behavior/pulse.tex \
Behavior/pwl.tex \
Behavior/sffm.tex \
Behavior/sin.tex \
Behavior/table.tex \
Behavior/tanh.tex \
Circuit/0summary.tex \
Circuit/c.tex \
Circuit/d.tex \
Circuit/e.tex \
Circuit/f.tex \
Circuit/g.tex \
Circuit/h.tex \
Circuit/i.tex \
Circuit/ibis.tex \
Circuit/j.tex \
Circuit/k.tex \
Circuit/l.tex \
Circuit/m.tex \
Circuit/q.tex \
Circuit/r.tex \
Circuit/s.tex \
Circuit/t.tex \
Circuit/tcap.tex \
Circuit/u.tex \
Circuit/v.tex \
Circuit/vccap.tex \
Circuit/vcg.tex \
Circuit/vcr.tex \
Circuit/w.tex \
Circuit/x.tex \
Circuit/y.tex \
Commands/0summary.tex \
Commands/1shell.tex \
Commands/2batch.tex \
Commands/3outfile.tex \
Commands/ac.tex \
Commands/alarm.tex \
Commands/alter.tex \
Commands/build.tex \
Commands/chdir.tex \
Commands/clear.tex \
Commands/dc.tex \
Commands/delete.tex \
Commands/disto.tex \
Commands/edit.tex \
Commands/end.tex \
Commands/exit.tex \
Commands/fanout.tex \
Commands/fault.tex \
Commands/fourier.tex \
Commands/generato.tex \
Commands/get.tex \
Commands/help.tex \
Commands/ic.tex \
Commands/include.tex \
Commands/insert.tex \
Commands/list.tex \
Commands/log.tex \
Commands/mark.tex \
Commands/merge.tex \
Commands/modify.tex \
Commands/nodeset.tex \
Commands/noise.tex \
Commands/op.tex \
Commands/options.tex \
Commands/pause.tex \
Commands/plot.tex \
Commands/print.tex \
Commands/quit.tex \
Commands/save.tex \
Commands/sens.tex \
Commands/set.tex \
Commands/status.tex \
Commands/sweep.tex \
Commands/temp.tex \
Commands/tf.tex \
Commands/title.tex \
Commands/transient.tex \
Commands/unfault.tex \
Commands/unmark.tex \
Commands/width.tex \
Tech/architecture.tex \
Tech/basic.tex \
Tech/datastruct.tex \
Tech/ds_parts-list.tex \
Tech/methods.tex \
Tech/performance.tex \
Tech/stamp.tex \
Tech/step-control.tex \
Tech/tr_basic.tex \
Tech/tr_cpoly.tex \
Tech/tr_step-control.tex \
Tech/transient.tex



#gnucap-man.pdf: $(TEXFILES) gnucap-man.dvi
#	pdflatex gnucap-man

gnucap-man.pdf: gnucap-man.dvi
	dvipdfm gnucap-man

gnucap-man.dvi: $(TEXFILES)
	latex gnucap-man
	latex gnucap-man
# run it twice to fix the toc.

html/index.html: $(TEXFILES)
	-mkdir html
	hevea -fix -exec xxdate.exe -o html/gnucap-man gnucap-man.tex
	cd html; hacha -nolinks gnucap-man; rm -f gnucap-man *.haux *.htoc
#	latex2html -toc_depth 1 -link 1 -split 4 -local_icons \
#		 -image_type png  gnucap-man.tex

checkin:
	ci -u`cat rcsversion` -m. -t/dev/null *.tex */*.tex Makefile
