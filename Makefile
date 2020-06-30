.PHONY: all build check document test

all: document build check

build: document
	R CMD build .

check: build
	R CMD check streamr*tar.gz

clean:
	-rm -f streamr*tar.gz
	-rm -fr streamr.Rcheck
	-rm -rf src/*.o src/*.so

install:
	R CMD INSTALL .

uninstall:
	R --slave -e "remove.packages('streamr')"

document: install-devtools
	R --slave -e "devtools::document()"

test: install-devtools
	R --slave -e "devtools::test()"

lintr: install-lintr
	R --slave -e "quit(status = length(print(lintr::lint_package())) != 0)"

install-devtools:
	R --slave -e "if (!require('devtools')) install.packages('devtools')"

install-lintr:
	R --slave -e "if (!require('lintr')) install.packages('lintr')"

clang-analyze: clean
	scan-build make build
