/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2013 Tory Gaurnier                                                *
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


#include <QDebug>
#include <QtGlobal>
#include <QApplication>

#include "Archive.hpp"
#include "ComicFile.hpp"
#include "Config.hpp"
#include "Library.hpp"
#include "MainWindow.hpp"


int main(int argc, char **argv) {
	// First set APP_ID
	qputenv("APP_ID", "eComics");

	// Initialize Qt Application
	QApplication app(argc, argv);
	app.setOrganizationName("Tory Gaurnier");
	app.setApplicationName("eComics");

	// Initialize main window
	MainWindow::init();

	main_window->show();
	return app.exec();

	Library::destroy();
	Config::destroy();
	MainWindow::destroy();
}