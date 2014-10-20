/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ToolBar.hpp                                                               *
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


#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP


#include <QToolBar>


class QLabel;


/**
 * class ToolBar
 *
 * Toolbar with various buttons, can be snapped to any side/top/bottom of LibraryView.
 */
class ToolBar : public QToolBar {
	public:
		static void init(QWidget *parent = 0);
		static void destroy();
		void disableBackButton();
		void enableBackButton();

	private:
		ToolBar(const QString &title, QWidget *parent);
		~ToolBar();
		void initActions();
};


extern ToolBar *tool_bar;


#endif