/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SplashScreen.hpp                                                               *
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


#ifndef SPLASHSCREEN_HPP
#define SPLASHSCREEN_HPP


#include <QSplashScreen>


/**
 * class SplashScreen
 *
 * Splash screen with text showing what is loading/processing.
 */
class SplashScreen : public QSplashScreen {
	Q_OBJECT

	public:
		static void start();
		static void finish();

	public slots:
		void update(const QString &msg);

	private:
		SplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = 0);
		SplashScreen(QWidget *parent, const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = 0);
		~SplashScreen();
		void initialize();
};


extern SplashScreen *splash_screen;


#endif