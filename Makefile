 # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
 # Makefile                                                                    #
 #                                                                             #
 # Copyright 2014 Tory Gaurnier <tory.gaurnier@linuxmail.org>                  #
 #                                                                             #
 # This program is free software; you can redistribute it and/or modify        #
 # it under the terms of the GNU Lesser General Public License as published by #
 # the Free Software Foundation; version 3.                                    #
 #                                                                             #
 # This program is distributed in the hope that it will be useful,             #
 # but WITHOUT ANY WARRANTY; without even the implied warranty of              #
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               #
 # GNU Lesser General Public License for more details.                         #
 #                                                                             #
 # You should have received a copy of the GNU Lesser General Public License    #
 # along with this program.  If not, see <http://www.gnu.org/licenses/>.       #
 # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#!/bin/bash


############################################ VARIABLES #############################################

DEFINES		=	-DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB

INC_PATH	=	-I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64 -I. -I.\
				-I/usr/include/x86_64-linux-gnu/qt5\
				-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets\
				-I/usr/include/x86_64-linux-gnu/qt5/QtGui\
				-I/usr/include/x86_64-linux-gnu/qt5/QtCore -I.\
				-I/usr/include/poppler/qt5

CXX_FLAGS	=	-c -std=c++0x -m64 -O2 -fPIE -Wall -W -ggdb $(DEFINES)

LIBS		=	-lQt5Widgets -lQt5Gui -lQt5Core -lpodofo -lpoppler-qt5

MOC_SRC		=	moc/moc_Actions.cpp\
				moc/moc_Archive.cpp\
				moc/moc_ComicFile.cpp\
				moc/moc_ComicInfo.cpp\
				moc/moc_ComicInfoDialog.cpp\
				moc/moc_DelimitedCompleter.cpp\
				moc/moc_LibraryView.cpp\
				moc/moc_FilePathEdit.cpp\
				moc/moc_FirstRunDialog.cpp\
				moc/moc_HelpButton.cpp\
				moc/moc_MainSidePane.cpp\
				moc/moc_MainView.cpp\
				moc/moc_MainWindow.cpp\
				moc/moc_MetadataTag.cpp\
				moc/moc_Page.cpp\
				moc/moc_PreferencesDialog.cpp

MOC_OBJ		=	$(MOC_SRC:%.cpp=%.o)

OBJECTS		=	obj/Actions.o\
				obj/Archive.o\
				obj/ComicFile.o\
				obj/ComicInfo.o\
				obj/ComicInfoDialog.o\
				obj/Config.o\
				obj/ConfirmationDialog.o\
				obj/DelimitedCompleter.o\
				obj/FilePathEdit.o\
				obj/FileTypeList.o\
				obj/FirstRunDialog.o\
				obj/FixedGridLayout.o\
				obj/HelpButton.o\
				obj/LibraryView.o\
				obj/Library.o\
				obj/MainSidePane.o\
				obj/MainView.o\
				obj/MainWindow.o\
				obj/MenuBar.o\
				obj/MetadataTag.o\
				obj/Page.o\
				obj/PageListView.o\
				obj/Pdf.o\
				obj/PreferencesDialog.o\
				obj/ToolBar.o

# Dependency files created by `g++ -MMD -MP`
DEPS		=	$(patsubst obj/%.o, dep/%.d, $(OBJECTS)) dep/main.d


############################################### RULES ##############################################

all: bin/eComics

bin/eComics: obj/main.o $(MOC_OBJ) $(OBJECTS)
	g++ $^ $(LIBS) -o bin/eComics

# Build all moc src files
$(MOC_SRC): moc/moc_%.cpp: src/%.hpp src/%.cpp
	moc $< $(INC_PATH) -o $@

# Build all moc objects
$(MOC_OBJ): moc/moc_%.o: moc/moc_%.cpp
	g++ $< $(INC_PATH) $(CXX_FLAGS) -o $@

# Build main.o
obj/main.o: src/main.cpp
	g++ $< -MMD -MF dep/main.d $(INC_PATH) $(CXX_FLAGS) -o $@

# Build all objects (except main.o), -MMD -MF generates dependencies
$(OBJECTS): obj/%.o: src/%.cpp
	g++ $< -MMD -MF dep/$*.d $(INC_PATH) $(CXX_FLAGS) -o $@

clean:
	rm -f obj/* dep/* moc/* bin/eComics

-include $(DEPS)