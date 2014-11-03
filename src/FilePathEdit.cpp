#include "FilePathEdit.hpp"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								FILEPATHEDIT PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Constructs with parent, if label_text is not blank then a label is created to left of line_edit,
 * dialog_caption, dialog_directory, and dialog_filter are all passed to QFileDialog. When opening
 * QFileDialog, QFileDialog's public static methods are used, so the dialog is never actually
 * constructed.
 */
FilePathEdit::FilePathEdit(QWidget *parent, const QString &label_text,
		const QString &dialog_caption, const QString &dialog_directory,
		QFileDialog::FileMode dialog_mode, const QString &dialog_filter) : QWidget(parent) {
	// Construct/initialize variables
	v_box					=	new QVBoxLayout(this);
	h_box_widget			=	new QWidget(this);
	error_msg				=	new QLabel(tr("Please choose a valid path"), this);
	h_box					=	new QHBoxLayout(h_box_widget);
	line_edit				=	new QLineEdit(this);
	browse_button			=	new QPushButton(tr("Browse"), this);
	this->dialog_caption	=	dialog_caption;
	this->dialog_directory	=	dialog_directory;
	this->dialog_mode		=	dialog_mode;
	this->dialog_filter		=	dialog_filter;

	// Remove spacing
	v_box->setSpacing(0);
	v_box->setMargin(0);
	v_box->setContentsMargins(0, 0, 0, 0);
	h_box->setMargin(0);
	h_box->setContentsMargins(0, 0, 0, 0);

	// Format and hide error message label
	QFont error_font		=	error_msg->font();
	QPalette error_palette	=	error_msg->palette();
	error_font.setItalic(true);
	error_palette.setColor(error_msg->foregroundRole(), Qt::red);
	error_msg->setFont(error_font);
	error_msg->setPalette(error_palette);
	error_msg->setContentsMargins(15, 0, 0, 0);
	error_msg->setVisible(false);

	// If label_text is not empty then construct label and set text, and append it to h_box
	if(!label_text.isEmpty()) {
		label = new QLabel(label_text);
		label_created = true;
		h_box->addWidget(label);
	} else label_created = false;

	// Add items to h_box
	v_box->addWidget(h_box_widget);
	v_box->addWidget(error_msg);
	h_box->addWidget(line_edit);
	h_box->addWidget(browse_button);

	// Connect signals
	connect(browse_button, SIGNAL(clicked(bool)), this, SLOT(openDialog()));
	connect(line_edit, SIGNAL(textChanged(QString)), this, SLOT(validatePath(QString)));
}


FilePathEdit::~FilePathEdit() {
	if(label_created) delete label;
	delete error_msg;
	delete browse_button;
	delete line_edit;
	delete h_box;
	delete h_box_widget;
}


QString FilePathEdit::getPath() const {
	return line_edit->text();
}


bool FilePathEdit::isValid() const {
	if(!line_edit->text().isEmpty() && QFile::exists(line_edit->text())) return true;
	else return false;
}


void FilePathEdit::setEnabled(bool enable) {
	if(!enable) error_msg->setVisible(false);
	else validatePath(line_edit->text());
	QWidget::setEnabled(enable);
}


void FilePathEdit::setDisabled(bool disable) {
	if(disable) error_msg->setVisible(false);
	else validatePath(line_edit->text());
	QWidget::setDisabled(disable);
}


/**
 * Sets QFileDialog caption.
 */
void FilePathEdit::setDialogCaption(const QString &caption) {
	dialog_caption = caption;
}


/**
 * Sets QFileDialog directory.
 */
void FilePathEdit::setDialogDirectory(const QString &directory) {
	dialog_directory = directory;
}


/**
 * Sets QFileDialog directory.
 */
void FilePathEdit::setDialogDirectory(const QDir &directory) {
	dialog_directory = directory.absolutePath();
}


/**
 * Sets QFileDialog FileMode.
 */
void FilePathEdit::setDialogFileMode(QFileDialog::FileMode mode) {
	dialog_mode = mode;
}


/**
 * Uses filter when opening QFileDialog.
 */
void FilePathEdit::setDialogFilter(const QString &filter) {
	dialog_filter = filter;
}


/**
 * Sets text of label, if label hasn't been constructed then it is constructed now.
 */
void FilePathEdit::setLabelText(const QString &label_text) {
	if(!label_created) {
		label = new QLabel(label_text);
		h_box->insertWidget(0, label);
	} else label->setText(label_text);
}


/**
 * Show dialog, get value, and put user selected path or paths in line_edit.
 */
void FilePathEdit::openDialog() {
	QString value;
	QStringList values;

	switch(dialog_mode) {
		case QFileDialog::AnyFile:
		case QFileDialog::ExistingFile:
			value	=	QFileDialog::getOpenFileName(this, dialog_caption, dialog_directory,
						dialog_filter);
			break;
		case QFileDialog::Directory:
		case QFileDialog::DirectoryOnly:
			value	=	QFileDialog::getExistingDirectory(this, dialog_caption, dialog_directory,
						QFileDialog::ShowDirsOnly);
			break;
		case QFileDialog::ExistingFiles:
			values	=	QFileDialog::getOpenFileNames(this, dialog_caption, dialog_directory,
						dialog_filter);

			for(QString str : values) {
				if(value.isEmpty()) value = str;
				else value += QString(", ") + str;
			}

			break;
	}

	line_edit->setText(value);
}


/**
 * Ensures that path entered in line edit is valid, if not an error message is shown
 */
void FilePathEdit::validatePath(const QString &path) {
	error_msg->setVisible(path.isEmpty() || !QFile::exists(path));
	emit pathChanged(path);
}