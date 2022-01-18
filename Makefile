.PHONY: qmake release publish

qmake:
	cd ./build; qmake6 -makefile -o Makefile ../project.pro
	
release:
	cd ./build; mingw32-make release

publish:
	cd ./publish; windeployqt-qt6 ../publish/pscan.exe