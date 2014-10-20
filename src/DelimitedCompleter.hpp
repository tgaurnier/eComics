/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DelimitedCompleter.hpp                                                               *
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


#ifndef DELIMITEDCOMPLETER_HPP
#define DELIMITEDCOMPLETER_HPP


#include <QCompleter>


class QLineEdit;
class QString;
class QStringList;


/**
 * class DelimitedCompleter
 *
 * QCompleter that supports completing multiple words in a QLineEdit, completed words are separated
 * by delimiter.
 */
class DelimitedCompleter : public QCompleter {
	Q_OBJECT

	public:
		DelimitedCompleter(QLineEdit *parent, char delimiter);
		DelimitedCompleter(QLineEdit *parent, char delimiter, QAbstractItemModel *model);
		DelimitedCompleter(QLineEdit *parent, char delimiter, const QStringList &list);
		QString pathFromIndex(const QModelIndex &index) const;
		QStringList splitPath(const QString &path) const;

	private:
		char delimiter;
		mutable int cursor_pos = -1;

		void connectSignals();

	private slots:
		void onActivated(const QString &text);
		void onCursorPositionChanged(int old_pos, int new_pos);
};


#endif