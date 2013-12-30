GUI_PATHS		=	-I/usr/include/qt5\
					-I/usr/include/qt5/QtGui\
					-I/usr/include/qt5/QtCore\
					-I/usr/include/qt5/QtQuick\
					-I/usr/include/qt5/QtConcurrent\
					-I/usr/include/qt5/QtQml

GUI_FLAGS		=	-fPIC # Flags for gui
LIBRARY_FLAGS	=	`xml2-config --cflags` # Flags for library.o
CFLAGS			=	-g -c -Wall
LIBS			=	`xml2-config --libs` -larchive -lQt5Gui -lQt5Core -lQt5Quick -lQt5Qml

OBJECTS			=	obj/main.o obj/config.o obj/gui.moc.o obj/gui.o obj/qtgui.moc.o obj/qtgui.o\
					obj/library.o obj/archive.o obj/str.o obj/strarr.o obj/files.o obj/misc.o

QML_FILES := $(patsubst src/gui/qml/%.qml, bin/qml/%.qml, $(wildcard src/gui/qml/*.qml))
JS_FILES := $(patsubst src/gui/js/%.js, bin/js/%.js, $(wildcard src/gui/js/*.js))

all: bin/eComics $(QML_FILES) $(JS_FILES)

bin/eComics: $(OBJECTS)
	g++ $(OBJECTS) $(LIBS) -g -o bin/eComics

bin/qml/%.qml: src/gui/qml/%.qml
	cp -u $< $@

bin/js/%.js: src/gui/js/%.js
	cp -u $< $@

obj/main.o: src/main.c src/config.h src/gui/gui.h src/library.h src/globalvars.h src/files.h
	gcc $< $(CFLAGS) -o $@

obj/config.o: src/config.c src/config.h src/gui/gui.h src/files.h src/misc.h src/globalvars.h\
		src/str.h src/globaldefs.h
	gcc $< $(CFLAGS) -o $@

src/gui/moc/gui.moc.cpp: src/gui/gui.h src/gui/gui.cpp
	moc $< $(GUI_PATHS) -o $@

obj/gui.moc.o: src/gui/moc/gui.moc.cpp
	g++ $< $(CFLAGS) $(GUI_FLAGS) $(GUI_PATHS) -o $@

obj/gui.o: src/gui/gui.cpp src/gui/gui.h src/gui/qtgui.h src/config.h src/globalvars.h\
		src/globaldefs.h src/library.h src/files.h src/misc.h
	g++ $< $(GUI_PATHS) $(GUI_FLAGS) $(CFLAGS) -o $@

src/gui/moc/qtgui.moc.cpp: src/gui/qtgui.h src/gui/qtgui.cpp
	moc $< $(GUI_PATHS) -o $@

obj/qtgui.moc.o: src/gui/moc/qtgui.moc.cpp
	g++ $< $(CFLAGS) $(GUI_FLAGS) $(GUI_PATHS) -o $@

obj/qtgui.o: src/gui/qtgui.cpp src/gui/qtgui.h src/gui/gui.h src/globalvars.h src/library.h\
		src/config.h src/strarr.h src/files.h src/str.h src/misc.h
	g++ $< $(GUI_PATHS) $(GUI_FLAGS) $(CFLAGS) -o $@

obj/library.o: src/library.c src/library.h src/globalvars.h src/globaldefs.h src/archive.h\
		src/files.h src/strarr.h src/str.h src/misc.h src/gui/gui.h
	gcc $< $(LIBRARY_FLAGS) $(CFLAGS) -o $@

obj/archive.o: src/archive.c src/archive.h src/files.h src/misc.h src/globalvars.h src/globaldefs.h
	gcc $< $(CFLAGS) -o $@

obj/str.o: src/str.c src/str.h src/misc.h
	gcc $< $(CFLAGS) -o $@

obj/strarr.o: src/strarr.c src/strarr.h src/misc.h src/str.h
	gcc $< $(CFLAGS) -o $@

obj/files.o: src/files.c src/files.h src/misc.h src/globalvars.h src/globaldefs.h src/strarr.h\
		src/str.h src/archive.h
	gcc $< $(CFLAGS) -o $@

obj/misc.o: src/misc.c src/misc.h src/str.h
	gcc $< $(CFLAGS) -o $@

clean:
	rm -f obj/*.o src/gui/moc/*.moc.cpp bin/eComics bin/qml/*.qml bin/js/*.js