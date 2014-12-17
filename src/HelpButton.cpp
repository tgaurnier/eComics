#include "HelpButton.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QStyle>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										HELPBUTTON PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Construct a HelpButton without a help message, message should be set with setMessage().
 */
HelpButton::HelpButton(QWidget *parent) : QPushButton(parent) {
	setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton));
	connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}


/**
 * Construct a HelpButton with a help message.
 */
HelpButton::HelpButton(const QString &msg, QWidget *parent) : QPushButton(parent), msg(msg) {
	setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton));
	connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}


HelpButton::~HelpButton() {}


void HelpButton::setMessage(const QString &msg) {
	this->msg = msg;
}


void HelpButton::onClicked() {
	QMessageBox::information(this, tr("Help"), msg);
}