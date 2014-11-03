/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FirstRunDialog.hpp                                                          *
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


#ifndef FIRSTRUNDIALOG_HPP
#define FIRSTRUNDIALOG_HPP


#include <QDialog>


class QCheckBox;
class QDialogButtonBox;
class QFormLayout;
class QVBoxLayout;

class FilePathEdit;
class HelpButton;


/**
 * class FirstRunDialog
 *
 * The dialog that opens on first run, allows to setup some basic configuration.
 */
class FirstRunDialog : QDialog {
	Q_OBJECT

	public:
		static int run(QWidget *parent);

	private:
		static FirstRunDialog *instance;
		QVBoxLayout *v_box;
		QWidget *form_widget;
		QFormLayout *form_layout;
		QDialogButtonBox *button_box;
		QCheckBox *manage_files;
		HelpButton *manage_files_help;
		QCheckBox *group_by_publisher;
		HelpButton *group_by_publisher_help;
		QCheckBox *comic_enabled;
		HelpButton *comic_enabled_help;
		FilePathEdit *comic_location;
		HelpButton *comic_location_help;
		QCheckBox *manga_enabled;
		HelpButton *manga_enabled_help;
		FilePathEdit *manga_location;
		HelpButton *manga_location_help;

		FirstRunDialog(QWidget *parent);
		~FirstRunDialog();

	private slots:
		void onAccepted();
		void toggleComicEnabled(int check_state);
		void toggleMangaEnabled(int check_state);
		void verifyOkButton();
};


#endif