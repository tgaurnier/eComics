/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * HelpButton.hpp                                                               *
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


#ifndef HELPBUTTON_HPP
#define HELPBUTTON_HPP


#include <QPushButton>


/**
 * class HelpButton
 *
 * A button that when pressed, displays a help message.
 */
class HelpButton : public QPushButton {
	Q_OBJECT

	public:
		HelpButton(QWidget *parent = 0);
		HelpButton(const QString &msg, QWidget *parent = 0);
		~HelpButton();
		void setMessage(const QString &msg);

	public slots:
		void onClicked();

	private:
		QString msg;
};


#endif