/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FixedGridLayout.hpp                                                               *
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


#ifndef FIXEDGRIDLAYOUT_HPP
#define FIXEDGRIDLAYOUT_HPP


#include <QGridLayout>


class QWidget;


/**
 * class FixedGridLayout
 *
 * QGridView with fixed size cells, MUST be constructed with at least column width, row height and
 * parent QWidget* are optional. addItem() and addLayout() methods are made private so that only
 * widgets can be added, because only widgets can have fixed sizes, and that is required for this
 * class to work properly. If a layout needs to be added, just create a QWidget and use setLayout()
 * to give it your desired layout, then add the widget to FixedGridLayout.
 *
 * IMPORTANT! When using this class it's VERY important that you don't
 * manually set the size of widgets being inserted, otherwise the outcome will not be as expected.
 */
class FixedGridLayout : public QGridLayout {
	public:
		FixedGridLayout(int col_width, int row_height = -1);
		FixedGridLayout(QWidget *parent, int col_width, int row_height = -1);
		~FixedGridLayout();
		void addWidget(QWidget *widget, int row, int col, Qt::Alignment alignment = 0);
		void addWidget(QWidget *widget, int row, int col, int row_span, int col_span,
			Qt::Alignment alignment = 0);

	private:
		int width, height;

		// Make addItem() and addLayout() private so only widgets can be added
		void addItem(QLayoutItem*, int, int, int, int, Qt::Alignment);
		void addLayout(QLayout*, int, int, Qt::Alignment);
		void addLayout(QLayout*, int, int, int rowSpan, int columnSpan, Qt::Alignment);
};


#endif