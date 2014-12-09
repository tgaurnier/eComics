/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FilePathEdit.hpp                                                            *
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


#ifndef FILEPATHEDIT_HPP
#define FILEPATHEDIT_HPP


#include <QFileDialog>
#include <QWidget>


class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;


/**
 * class FilePathEdit
 *
 * Contains an optional QLabel, and a QLineEdit with a file browse button next to it.
 */
class FilePathEdit : public QWidget {
	Q_OBJECT

	public:
		FilePathEdit(QWidget *parent = 0, const QString &label_text = QString(),
			const QString &dialog_caption = QString(), const QString &dialog_directory = QString(),
			QFileDialog::FileMode mode = QFileDialog::AnyFile,
			const QString &dialog_filter = QString());
		~FilePathEdit();
		QString getPath() const;
		bool isValid() const;
		void setEnabled(bool enable);
		void setDisabled(bool disable);
		void setDialogCaption(const QString &caption);
		void setDialogDirectory(const QString &directory);
		void setDialogDirectory(const QDir &directory);
		void setDialogFileMode(QFileDialog::FileMode mode);
		void setDialogFilter(const QString &filter);
		void setLabelText(const QString &label_text);
		void setPath(const QString &path);

	public slots:
		void openDialog();
		void validatePath(const QString &path);

	signals:
		void pathChanged(const QString &path);

	private:
		QVBoxLayout *v_box;
		QWidget *h_box_widget;
		QHBoxLayout *h_box;
		QLabel *error_msg;
		QLabel *label;
		QLineEdit *line_edit;
		QPushButton *browse_button;
		QString dialog_caption;
		QString dialog_directory;
		QString dialog_filter;
		QFileDialog::FileMode dialog_mode;
		bool label_created;
};


#endif