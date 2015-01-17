/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MainSidePane.hpp                                                               *
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


#ifndef MAINSIDEPANE_HPP
#define MAINSIDEPANE_HPP


#include <QWidget>
#include <QListView>


class QLabel;
class QStringListModel;
class QToolBar;
class QVBoxLayout;


/**
 * class MainSidePane
 *
 * Description
 */
class MainSidePane : public QWidget {
	Q_OBJECT

	public:
		static void init(QWidget *parent = 0);
		static void destroy();
		void refreshLibraries();
		void refreshLists();
		void initSelectedList();

	signals:
		void selectedListChanged(const QString &);

	private:
		// QListView for choosing Comics or Manga library
		class LibraryListView : public QListView {
			public:
				static void init(QWidget *parent);
				static void destroy();
				void refresh();

			private:
				QStringListModel *internal_model;

				LibraryListView(QWidget *parent);
				~LibraryListView();
				void selectionChanged(const QItemSelection &selected,
					const QItemSelection &deselected);
		};

		// QListView for choosing a user defined list
		class UserListView : public QListView {
			public:
				static void init(QWidget *parent);
				static void destroy();
				void refresh();

			private:
				QStringListModel *internal_model;

				UserListView(QWidget *parent);
				~UserListView();
				void selectionChanged(const QItemSelection &selected,
					const QItemSelection &deselected);
		};

		QVBoxLayout *layout;
		QLabel *library_label;
		QLabel *lists_label;
		static LibraryListView *library_list_view;
		static UserListView *user_list_view;
		QToolBar *tool_bar;
		QAction *new_list_action;
		QAction *remove_list_action;
		QAction *edit_list_action;
		bool is_visible;

		MainSidePane(QWidget *parent);
		~MainSidePane();
		void restoreSettings();
		void saveSettings();

	private slots:
		void toggleVisibility(bool visible);
		void onEditListActionChanged();
		void onNewListActionChanged();
		void onRemoveListActionChanged();
		void onUserListViewSelectionChanged(const QItemSelection &selected,
			const QItemSelection &deselected);
};


extern MainSidePane *main_side_pane;


#endif