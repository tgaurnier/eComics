#include "FirstRunDialog.hpp"

#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

#include "Config.hpp"
#include "FilePathEdit.hpp"
#include "HelpButton.hpp"


FirstRunDialog *FirstRunDialog::instance = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									FIRSTRUNDIALOG PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Construct FirstRunDialog singleton, automatically deleted after the dialog is closed. Returns the
 * result returned by QDialog::exec().
 */
int FirstRunDialog::run(QWidget *parent) {
	if(instance == nullptr) {
		instance = new FirstRunDialog(parent);
		int result = instance->exec();
		delete instance;
		instance = nullptr;

		return result;
	}

	return -1;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									FIRSTRUNDIALOG PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


FirstRunDialog::FirstRunDialog(QWidget *parent) : QDialog(parent) {
	// Init child objects
	v_box		=	new QVBoxLayout(this);
	form_widget	=	new QWidget(this);
	form_layout	=	new QFormLayout(form_widget);
	button_box	=	new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	// Add button box to v_box
	v_box->addWidget(form_widget);
	v_box->addWidget(button_box);

	// Setup form items
	manage_files			=	new QCheckBox(tr("Manage Files"), form_widget);
	manage_files_help		=	new HelpButton(tr("If this is enabled then eComics will keep ") +
								tr("your files organized, meaning it will move and rename them ") +
								tr("when necessary."), form_widget);
	group_by_publisher		=	new QCheckBox(tr("Group by Publisher"), form_widget);
	group_by_publisher_help	=	new HelpButton(tr("If this is enabled then comics will be ") +
								tr("organized by Publisher/Series/Volume, if it is not enabled ") +
								tr("then they will be organized by Series/Volume."), form_widget);
	comic_enabled			=	new QCheckBox(tr("Enable Comics"), form_widget);
	comic_enabled_help		=	new HelpButton(tr("eComics supports organizing Comics and Manga ") +
								tr("in different folders. If you have Comics, or you just want ") +
								tr("everthing in the same folder, then enable this option. You ") +
								tr("MUST have either Comics or Manga enabled."), form_widget);
	comic_location			=	new FilePathEdit(form_widget, tr("Location of Comics"),
								tr("Open Comics Directory"), QDir::homePath(),
								QFileDialog::Directory);
	comic_location_help		=	new HelpButton(tr("The path to where your Comics are located ") +
								tr("should go here."), form_widget);
	manga_enabled			=	new QCheckBox(tr("Enable Manga"), form_widget);
	manga_enabled_help		=	new HelpButton(tr("If you have Manga, and you want it to be in ") +
								tr("it's own folder then you should enable this. If you have ") +
								tr("Comics and Manga, and you want them both in the same ") +
								tr("folder, then you should just enable Comics and leave this ") +
								tr("disabled."), form_widget);
	manga_location			=	new FilePathEdit(form_widget, tr("Location of Manga"),
								tr("Open Manga Directory"), QDir::homePath(),
								QFileDialog::Directory);
	manga_location_help		=	new HelpButton(tr("The path to where your Manga is located ") +
								tr("should go here."), form_widget);

	// Start manga and comic location fields disabled
	comic_location->setEnabled(false);
	comic_location_help->setEnabled(false);
	manga_location->setEnabled(false);
	manga_location_help->setEnabled(false);
	button_box->button(QDialogButtonBox::Ok)->setEnabled(false);

	// Add items to form
	form_layout->addRow(manage_files, manage_files_help);
	form_layout->addRow(group_by_publisher, group_by_publisher_help);
	form_layout->addRow(comic_enabled, comic_enabled_help);
	form_layout->addRow(comic_location, comic_location_help);
	form_layout->addRow(manga_enabled, manga_enabled_help);
	form_layout->addRow(manga_location, manga_location_help);

	// Connect signals
	connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
	connect(comic_enabled, SIGNAL(stateChanged(int)), this, SLOT(toggleComicEnabled(int)));
	connect(manga_enabled, SIGNAL(stateChanged(int)), this, SLOT(toggleMangaEnabled(int)));
	connect(comic_location, SIGNAL(pathChanged(QString)), this, SLOT(verifyOkButton()));
	connect(manga_location, SIGNAL(pathChanged(QString)), this, SLOT(verifyOkButton()));
}


FirstRunDialog::~FirstRunDialog() {
	delete button_box;
	delete manage_files;
	delete manage_files_help;
	delete group_by_publisher;
	delete group_by_publisher_help;
	delete comic_enabled;
	delete comic_enabled_help;
	delete comic_location;
	delete comic_location_help;
	delete manga_enabled;
	delete manga_enabled_help;
	delete manga_location;
	delete manga_location_help;
	delete form_layout;
	delete form_widget;
	delete v_box;
}


/**
 * PRIVATE SLOTS
 */


void FirstRunDialog::onAccepted() {
	config->setManageFiles(manage_files->checkState() == Qt::Checked);
	config->setGroupByPublisher(group_by_publisher->checkState() == Qt::Checked);
	config->setComicEnabled(comic_enabled->checkState() == Qt::Checked);
	config->setMangaEnabled(manga_enabled->checkState() == Qt::Checked);
	if(config->isComicEnabled()) config->setComicDir(comic_location->getPath());
	if(config->isMangaEnabled()) config->setMangaDir(manga_location->getPath());
	config->save();
}


void FirstRunDialog::toggleComicEnabled(int state) {
	comic_location->setEnabled(state);
	comic_location_help->setEnabled(state);
	verifyOkButton();
}


void FirstRunDialog::toggleMangaEnabled(int state) {
	manga_location->setEnabled(state);
	manga_location_help->setEnabled(state);
	verifyOkButton();
}


/**
 * If both Comics and Manga are disabled then OK button should be disabled.
 */
void FirstRunDialog::verifyOkButton() {
	if(	(comic_enabled->checkState() == Qt::Checked && comic_location->isValid()) ||
		(manga_enabled->checkState() == Qt::Checked && manga_location->isValid())
	) button_box->button(QDialogButtonBox::Ok)->setEnabled(true);
	else button_box->button(QDialogButtonBox::Ok)->setEnabled(false);
}