all: modelgen gnucap

modelgen: nothing
	(cd modelgen; ${MAKE} -k)

gnucap: modelgen
	(cd src; ${MAKE} -k)

clean:
	(cd modelgen; make clean)
	(cd src; make clean)

install: nothing
	(cd src; make install)

nothing:

