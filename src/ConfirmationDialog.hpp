/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ConfirmationDialog.hpp                                                               *
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


#ifndef CONFIRMATIONDIALOG_HPP
#define CONFIRMATIONDIALOG_HPP


#include <QMessageBox>


/**
 * class ConfirmationDialog
 *
 * A simple Yes/No confirmation dialog.
 */
class ConfirmationDialog : public QMessageBox {
	public:
		using QMessageBox::exec;
		static bool exec(const QString &title, const QString &msg, QWidget *parent = 0);

	private:
		static ConfirmationDialog *instance;

		ConfirmationDialog(Icon icon, const QString &title, const QString &msg,
			StandardButtons buttons, QWidget *parent);
		~ConfirmationDialog();
};


#endif