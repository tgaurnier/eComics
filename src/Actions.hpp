/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Actions.hpp                                                               *
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


#ifndef ACTIONS_HPP
#define ACTIONS_HPP


/**
 * QAction needs to be included in header so that anything that includes Actions.hpp will know how
 * to convert QAction to QObject
 */
#include <QAction>


class QShortcut;
class QStyle;


namespace eComics {
	/**
	 * class Actions
	 *
	 * A singleton object, which contains all of the global actions in the application, these
	 * actions can be used in anything from the menu bar, to right click menus. Most of these
	 * actions are connected in other classes (for example, full_screen_action is connected in
	 * MainWindow.cpp), only certain actions are connected to 'local' slots (such as
	 * add_comics_action is connected).
	 */
	class Actions : public QObject {
		Q_OBJECT

		public:
			// Getters for file actions
			QAction * open() const { return open_action; }
			QAction * newList() const { return new_list_action; }
			QAction * addComics() const { return add_comics_action; }
			QAction * quit() const { return quit_action; }
			// Getters for edit actions
			QAction * removeList() const { return remove_list_action; }
			QAction * editList() const { return edit_list_action; }
			QAction * addToList() const { return add_to_list_action; }
			QAction * info() const { return info_action; }
			QAction * remove() const { return remove_action; }
			QAction * deleteFile() const { return delete_action; }
			QAction * preferences() const { return preferences_action; }
			// Getters for view actions
			QAction * fullScreen() const { return full_screen_action; }
			QAction * toolBar() const { return tool_bar_action; }
			QAction * sidePane() const { return side_pane_action; }
			QAction * statusBar() const { return status_bar_action; }
			// Getters for tools actions
			QAction * scanLibrary() const { return scan_library_action; }
			QAction * cleanupFiles() const { return cleanup_files_action; }
			QAction * convert() const { return convert_action; }
			// Getters for help actions
			QAction * about() const { return about_action; }
			QAction * faq() const { return faq_action; }
			QAction * reportBug() const { return report_bug_action; }
			// Getters for library navigation actions
			QAction* navigateBack() const { return navigate_back_action; }

			static void init(QWidget *parent = 0);
			static void destroy();
			void setToolBarAction(QAction *action);

		private:
			QShortcut *f12_shortcut;
			QStyle *style;
			// File actions
			QAction *open_action;
			QAction *new_list_action;
			QAction *add_comics_action;
			QAction *quit_action;
			// Edit actions
			QAction *remove_list_action;
			QAction *edit_list_action;
			QAction *add_to_list_action;
			QAction *info_action;
			QAction *remove_action; // Removes from library and moves file(s) to desktop
			QAction *delete_action; // Removes from library and moves file(s) to trash
			QAction *preferences_action;
			// View actions
			QAction *full_screen_action;
			QAction *tool_bar_action;
			QAction *side_pane_action;
			QAction *status_bar_action;
			// Tools actons
			QAction *scan_library_action;
			QAction *cleanup_files_action;
			QAction *convert_action;
			// Help actions
			QAction *about_action;
			QAction *faq_action;
			QAction *report_bug_action;
			// Library navigation actions
			QAction *navigate_back_action;

			Actions(QWidget *parent);
			~Actions();
	};

	extern Actions *actions;
}


#endif