/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MenuBar.hpp                                                               *
 *                                                                             *
 * Copyright 2014 Tory Gaurnier <tory.gaurnier@linuxmail.org>                  *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published by *
 * the Free Software Foundation; version 3.                                    *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef MENUBAR_HPP
#define MENUBAR_HPP


#include <QObject>


class QMainWindow;
class QMenuBar;
class QMenu;


/**
 * class MenuBar
 *
 * This is a singleton object that sets up and manages the menu bar for the QMainWindow parent.
 */
class MenuBar : QObject {
	public:
		static void init(QMainWindow *parent);
		static void destroy();

	private:
		QMenuBar *q_menu_bar;
		QMenu *file_menu;
		QMenu *edit_menu;
		QMenu *view_menu;
		QMenu *tools_menu;
		QMenu *help_menu;

		MenuBar(QMainWindow *parent);
		~MenuBar() {};
};


extern MenuBar *menu_bar;


#endif