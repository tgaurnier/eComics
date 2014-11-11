/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MainWindow.hpp                                                              *
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


#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


#include <QMainWindow>


class QStatusBar;
class QSplitter;


/**************************************************************************************************
 *                                           MENU BAR                                             *
 **************************************************************************************************
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *   SIDE PANE    *                                 MAIN VIEW                                     *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 *                *                                                                               *
 **************************************************************************************************
 *                                          STATUS BAR                                            *
 **************************************************************************************************/


/**
 * The main window, a singleton object (there should only ever be one main window, after all).
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		static bool init();
		static void destroy();

	private:
		static bool exiting;
		QStatusBar *status_bar		=	nullptr;
		QSplitter *main_splitter	=	nullptr; // Sidebar on left, mainview on right

		MainWindow();
		~MainWindow();
	private slots:
		void toggleFullscreen();
};

extern MainWindow *main_window;


#endif