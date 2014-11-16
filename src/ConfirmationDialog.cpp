#include "ConfirmationDialog.hpp"

#include <QDebug>


ConfirmationDialog *ConfirmationDialog::instance = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								CONFIRMATIONDIALOG PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


bool ConfirmationDialog::exec(const QString &title, const QString &msg, QWidget *parent) {
	if(instance == nullptr) {
		instance = new ConfirmationDialog(Question, title, msg, Yes | No, parent);
		bool result = (instance->exec() == Yes);
		delete instance;
		instance = nullptr;
		return result;
	}
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								CONFIRMATIONDIALOG PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ConfirmationDialog::ConfirmationDialog(Icon icon, const QString &title, const QString &msg,
		StandardButtons buttons, QWidget *parent) : QMessageBox(icon, title, msg, buttons, parent) {
}


ConfirmationDialog::~ConfirmationDialog() {}