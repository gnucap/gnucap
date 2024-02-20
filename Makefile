

gnucap:
	#(cd conf; ${MAKE} -k)
	#(cd include; ${MAKE} -k)
	(cd lib; ${MAKE} -k)
	(cd modelgen; ${MAKE} -k)
	(cd main; ${MAKE} -k)
	(cd apps; ${MAKE} -k)

debug:
	#(cd conf; ${MAKE} debug)
	#(cd include; ${MAKE} debug)
	(cd lib; ${MAKE} debug)
	(cd modelgen; ${MAKE} debug)
	(cd main; ${MAKE} debug)
	(cd apps; ${MAKE} debug)

g++:
	#(cd conf; ${MAKE} g++)
	#(cd include; ${MAKE} g++)
	(cd lib; ${MAKE} g++)
	(cd modelgen; ${MAKE} g++)
	(cd main; ${MAKE} g++)
	(cd apps; ${MAKE} g++)

clean:
	#(cd conf; ${MAKE} clean)
	#(cd include; ${MAKE} clean)
	(cd lib; ${MAKE} clean)
	(cd modelgen; ${MAKE} clean)
	(cd main; ${MAKE} clean)
	(cd apps; ${MAKE} clean)
	-rm *~ \#*\#

depend:
	#(cd conf; ${MAKE} depend)
	#(cd include; ${MAKE} depend)
	(cd lib; ${MAKE} depend)
	(cd modelgen; ${MAKE} depend)
	(cd main; ${MAKE} depend)
	(cd apps; ${MAKE} depend)

tags:
	#(cd conf; ${MAKE} tags)
	#(cd include; ${MAKE} tags)
	(cd lib; ${MAKE} tags)
	(cd modelgen; ${MAKE} tags)
	(cd main; ${MAKE} tags)
	(cd apps; ${MAKE} tags)

unconfig:
	#(cd conf; ${MAKE} unconfig)
	#(cd include; ${MAKE} unconfig)
	(cd lib; ${MAKE} unconfig)
	(cd modelgen; ${MAKE} unconfig)
	(cd main; ${MAKE} unconfig)
	(cd apps; ${MAKE} unconfig)

install:
	(cd conf; ${MAKE} install)
	(cd include; ${MAKE} install)
	(cd lib; ${MAKE} install)
	(cd modelgen; ${MAKE} install)
	(cd main; ${MAKE} install)
	(cd apps; ${MAKE} install)

install-debug:
	(cd conf; ${MAKE} install-debug)
	(cd include; ${MAKE} install-debug)
	(cd lib; ${MAKE} install-debug)
	(cd modelgen; ${MAKE} install-debug)
	(cd main; ${MAKE} install-debug)
	(cd apps; ${MAKE} install-debug)

uninstall:
	(cd conf; ${MAKE} uninstall)
	(cd include; ${MAKE} uninstall)
	(cd lib; ${MAKE} uninstall)
	(cd modelgen; ${MAKE} uninstall)
	(cd main; ${MAKE} uninstall)
	(cd apps; ${MAKE} uninstall)

manifest:
	(cd conf; ${MAKE} manifest)
	(cd include; ${MAKE} manifest)
	(cd lib; ${MAKE} manifest)
	(cd modelgen; ${MAKE} manifest)
	(cd main; ${MAKE} manifest)
	(cd apps; ${MAKE} manifest)

header-check:
	(cd conf; ${MAKE} header-check)
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

.PHONY: install
