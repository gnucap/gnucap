

gnucap: nothing
	#(cd include; ${MAKE} gnucap)
	(cd lib; ${MAKE} -k)
	(cd modelgen; ${MAKE} -k)
	(cd main; ${MAKE} -k)
	(cd apps; ${MAKE} -k)

debug:
	#(cd include; ${MAKE} debug)
	(cd lib; ${MAKE} debug)
	(cd modelgen; ${MAKE} debug)
	(cd main; ${MAKE} debug)
	(cd apps; ${MAKE} debug)

g++:
	#(cd include; ${MAKE} g++)
	(cd lib; ${MAKE} g++)
	(cd modelgen; ${MAKE} g++)
	(cd main; ${MAKE} g++)
	(cd apps; ${MAKE} g++)

mingw:
	#(cd include; ${MAKE} mingw)
	(cd lib; ${MAKE} mingw)
	(cd modelgen; ${MAKE} mingw)
	(cd main; ${MAKE} mingw)
	(cd apps; ${MAKE} mingw)

clean:
	#(cd include; ${MAKE} clean)
	(cd lib; ${MAKE} clean)
	(cd modelgen; ${MAKE} clean)
	(cd main; ${MAKE} clean)
	(cd apps; ${MAKE} clean)
	-rm *~ \#*\#

depend:
	#(cd include; ${MAKE} depend)
	(cd lib; ${MAKE} depend)
	(cd modelgen; ${MAKE} depend)
	(cd main; ${MAKE} depend)
	(cd apps; ${MAKE} depend)

tags:
	#(cd include; ${MAKE} tags)
	(cd lib; ${MAKE} tags)
	(cd modelgen; ${MAKE} tags)
	(cd main; ${MAKE} tags)
	(cd apps; ${MAKE} tags)

unconfig:
	#(cd include; ${MAKE} unconfig)
	(cd lib; ${MAKE} unconfig)
	(cd modelgen; ${MAKE} unconfig)
	(cd main; ${MAKE} unconfig)
	(cd apps; ${MAKE} unconfig)

install: nothing
	(cd include; ${MAKE} install)
	(cd lib; ${MAKE} install)
	(cd modelgen; ${MAKE} install)
	(cd main; ${MAKE} install)
	(cd apps; ${MAKE} install)

install-debug: nothing
	(cd include; ${MAKE} install-debug)
	(cd lib; ${MAKE} install-debug)
	(cd modelgen; ${MAKE} install-debug)
	(cd main; ${MAKE} install-debug)
	(cd apps; ${MAKE} install-debug)

uninstall: nothing
	(cd include; ${MAKE} uninstall)
	(cd lib; ${MAKE} uninstall)
	(cd modelgen; ${MAKE} uninstall)
	(cd main; ${MAKE} uninstall)
	(cd apps; ${MAKE} uninstall)

manifest:
	(cd include; ${MAKE} manifest)
	(cd lib; ${MAKE} manifest)
	(cd modelgen; ${MAKE} manifest)
	(cd main; ${MAKE} manifest)
	(cd apps; ${MAKE} manifest)

header-check:
	(cd include; ${MAKE} header-check)
	(cd lib; ${MAKE} header-check)
	(cd modelgen; ${MAKE} header-check)
	(cd main; ${MAKE} header-check)
	(cd apps; ${MAKE} header-check)

date:
	(cd include; ${MAKE} date)

checkin:
	$(MAKE) date
	-git commit -a

nothing:

