/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * PageListView.hpp                                                               *
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


#ifndef PAGELISTVIEW_HPP
#define PAGELISTVIEW_HPP


#include <QAbstractListModel>
#include <QListView>

#include "ComicFile.hpp"


class PageList;


/**
 * class PageListView
 *
 * A QListView in IconMode, shows a list of pages from comic.
 */
class PageListView : public QListView {
	public:
		PageListView(ComicFile &comic, QWidget *parent = 0);
		~PageListView();
		bool init();

	private:
		class PageListModel : public QAbstractListModel {
			public:
				PageListModel(PageList &page_list, QObject *parent = 0);
				QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
				int rowCount(const QModelIndex &parent = QModelIndex()) const;

			private:
				PageList &list;
		};

		PageListModel *model;
		ComicFile &comic;
		bool initialized = false;
};


#endif