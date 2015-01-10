/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * LibraryView.hpp                                                             *
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


#ifndef LIBRARYVIEW_HPP
#define LIBRARYVIEW_HPP


#include <QAbstractListModel>
#include <QListView>

#include "ReferenceList.hpp"


class ComicFile;


/**
 * class LibraryView
 *
 * A QListView in icon mode with it's list model as a private nested class.
 */
class LibraryView : public QListView {
	Q_OBJECT

	public:
		static void init(QWidget *parent = 0);
		static void destroy();
		ReferenceList<ComicFile> getSelectedComics();
		void resetScope();

	public slots:
		void refreshModel();
		void onListChanged(QString list);
		void onItemActivated(const QModelIndex &index);
		void onNavigateBack();

	private:
		// This model uses the library singleton as it's data list
		class LibraryModel : public QAbstractListModel {
			public:
				static void init(QObject *parent);
				static void destroy();
				QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
				//Qt::ItemFlags flags(const QModelIndex &index) const; TODO
				//QMimeData * mimeData(const QModelIndexList &index_list) const; TODO
				int rowCount(const QModelIndex &parent = QModelIndex()) const;
				void refreshList();
				void setSelectedLibrary(const QString &library);
				void setSelectedUserList(const QString &list);
				void navigateBack();
				void onItemActivated(const QModelIndex &index);
				friend ReferenceList<ComicFile> LibraryView::getSelectedComics();
				friend void LibraryView::resetScope();

			private:
				/**
				 * Scope categories enumerator.
				 *
				 * IMPORTANT!!!
				 * Any last-level categories (categories where activating an item
				 * will actually open a comic rather than navigating) must be after TITLE_SCOPE.
				 */
				enum ScopeCategory {
					PUBLISHER_SCOPE, // Viewing list of publishers
					SERIES_SCOPE, // Viewing list of series from cur_publisher
					VOLUME_SCOPE, // Viewing list of volumes from cur_publisher/cur_series
					TITLE_SCOPE, // Viewing list of comics from cur_publisher/cur_series/cur_volume
					LIST_SCOPE // Viewing list of comics from cur_list
				};

				LibraryView *parent;
				QStringList list;

				// Variables to keep track of scope
				struct ScopeState {
					ScopeCategory category;
					QString publisher;
					QString series;
					QString volume;
					QString library;
					QString user_list;

					friend bool operator ==(const ScopeState &lhs, const ScopeState &rhs) {
						if(
							lhs.category	==	rhs.category &&
							lhs.publisher	==	rhs.publisher &&
							lhs.series		==	rhs.series &&
							lhs.volume		==	rhs.volume &&
							lhs.library		==	rhs.library &&
							lhs.user_list	==	rhs.user_list
						) return true;

						else return false;
					}
				} cur_scope;

				LibraryModel(QObject *parent);
				~LibraryModel();
				QIcon layeredIcon(const QList<ComicFile> &comic_list) const;
		};

		// Private vairables
		static LibraryModel *model;
		int icon_size;

		LibraryView(QWidget *parent = 0);
		~LibraryView();

		private slots:
			void onSelectionChanged(const QItemSelection &selected,
				const QItemSelection &deselected);
};

extern LibraryView *library_view;

#endif