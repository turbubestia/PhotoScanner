.PHONY: qmake release

qmake:
	cd ./build; qmake -makefile -o Makefile ../project.pro
	
release:
	cd ./build; mingw32-make release