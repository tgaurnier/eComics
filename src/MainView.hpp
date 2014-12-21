/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MainView.hpp                                                               *
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


#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP


#include <QMainWindow>


class LibraryView;


/*********************************************************************************
 *                                  TOOL BAR                                     *
 *********************************************************************************
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                             LIBRARY GRID VIEW                                 *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *                                                                               *
 *********************************************************************************/


/**
 * class MainView
 *
 * The MainView is a QMainWindow styled as a widget rather than a top level window, it contains a
 * movable toolbar and a library view.
 */
class MainView : public QMainWindow {
	Q_OBJECT

	public:
		static void init(QWidget *parent = 0);
		static void destroy();

	private:
		MainView(QWidget *parent);
		~MainView();
		void saveSettings();
		void restoreSettings();
};


extern MainView *main_view;


#endif