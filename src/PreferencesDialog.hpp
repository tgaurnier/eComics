/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * PreferencesDialog.hpp                                                       *
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


#ifndef PREFERENCESDIALOG_HPP
#define PREFERENCESDIALOG_HPP


#include <QDialog>


class QCheckBox;
class QDialogButtonBox;
class QFormLayout;
class QTabWidget;
class QVBoxLayout;

class FilePathEdit;
class HelpButton;


/**
 * class PreferencesDialog
 *
 * Dialog to edit various configurations.
 */
class PreferencesDialog : public QDialog {
	Q_OBJECT

	public:
		static void init(QWidget *parent = 0);
		static void destroy();

	public slots:
		void show();

	private:
		PreferencesDialog(QWidget *parent);
		~PreferencesDialog();

		/**
		 * Tab to modify library related settings.
		 */
		class LibraryTab : public QWidget {
			public:
				QFormLayout *form_layout;
				// Manage files
				QCheckBox *manage_files;
				HelpButton *manage_files_help;
				// Group by publisher
				QCheckBox *group_by_publisher;
				HelpButton *group_by_publisher_help;
				// Comic enabled
				QCheckBox *comic_enabled;
				HelpButton *comic_enabled_help;
				// Comic location
				FilePathEdit *comic_location;
				HelpButton *comic_location_help;
				// Manga enabled
				QCheckBox *manga_enabled;
				HelpButton *manga_enabled_help;
				// Manga location
				FilePathEdit *manga_location;
				HelpButton *manga_location_help;

				static void init();
				static void destroy();

			private:
				LibraryTab();
				~LibraryTab();
		};

		/**
		 * Tab to modify window layout settings.
		 */
		class LayoutTab : public QWidget {
			public:
				static void init();
				static void destroy();

			private:
				LayoutTab();
				~LayoutTab();
		};

		/**
		 * Tab to modify miscelaneous settings.
		 */
		class MiscTab : public QWidget {
			public:
				QFormLayout *form_layout;
				// Temp file location
				FilePathEdit *temp_location;
				HelpButton *temp_location_help;
				// Thumnail location
				FilePathEdit *thumb_location;
				HelpButton *thumb_location_help;

				static void init();
				static void destroy();

			private:
				MiscTab();
				~MiscTab();
		};

		// Member variables/objects
		static LibraryTab *library_tab;
		static LayoutTab *layout_tab;
		static MiscTab *misc_tab;
		QVBoxLayout *v_box;
		QTabWidget *tab_widget;
		QDialogButtonBox *button_box;

	private slots:
		void onFinished(const int result);
		void toggleComicEnabled(int check_state);
		void toggleMangaEnabled(int check_state);
		void verifyOkButton();
};


extern PreferencesDialog *preferences_dialog;


#endif