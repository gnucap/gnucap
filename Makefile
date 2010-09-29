all: ibis modelgen gnucap

ibis: nothing
	(cd ibis; ${MAKE} -k)

modelgen: nothing
	(cd modelgen; ${MAKE} -k)

gnucap: modelgen
	(cd src; ${MAKE} -k)

clean:
	(cd ibis; make clean)
	(cd modelgen; make clean)
	(cd src; make clean)

install: nothing
	(cd src; make install)

nothing:

