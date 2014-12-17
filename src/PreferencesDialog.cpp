#include "PreferencesDialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>

#include "Actions.hpp"
#include "Config.hpp"
#include "FilePathEdit.hpp"
#include "HelpButton.hpp"
#include "Library.hpp"
#include "LibraryView.hpp"
#include "MainSidePane.hpp"


PreferencesDialog *preferences_dialog							=	nullptr;
PreferencesDialog::LibraryTab *PreferencesDialog::library_tab	=	nullptr;
PreferencesDialog::LayoutTab *PreferencesDialog::layout_tab		=	nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								PREFERENCESDIALOG PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void PreferencesDialog::init(QWidget *parent) {
	if(preferences_dialog == nullptr) {
		preferences_dialog = new PreferencesDialog(parent);
	}
}


void PreferencesDialog::destroy() {
	delete preferences_dialog;
	preferences_dialog = nullptr;
}


void PreferencesDialog::show() {
	exec();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								PREFERENCESDIALOG PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent) {
	// Init child objects
	v_box		=	new QVBoxLayout(this);
	tab_widget	=	new QTabWidget;
	button_box	=	new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	LibraryTab::init();
	LayoutTab::init();

	// Add itmems to v_box
	v_box->addWidget(tab_widget);
	v_box->addWidget(button_box);

	// Add tabs to tab widget
	tab_widget->addTab(library_tab, tr("Library"));
	tab_widget->addTab(layout_tab, tr("Layout"));

	// Connect signals
	connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(const int)));
	connect(eComics::actions->preferences(), SIGNAL(triggered()), this, SLOT(show()));

	// Connect signals for LibraryTab
	connect(library_tab->comic_enabled, SIGNAL(stateChanged(int)),
			this, SLOT(toggleComicEnabled(int)));
	connect(library_tab->manga_enabled, SIGNAL(stateChanged(int)),
			this, SLOT(toggleMangaEnabled(int)));
	connect(library_tab->comic_location, SIGNAL(pathChanged(QString)),
			this, SLOT(verifyOkButton()));
	connect(library_tab->manga_location, SIGNAL(pathChanged(QString)),
			this, SLOT(verifyOkButton()));

	setWindowTitle(tr("Preferences"));
}


PreferencesDialog::~PreferencesDialog() {
	LibraryTab::destroy();
	LayoutTab::destroy();
	delete tab_widget;
	delete button_box;
	delete v_box;
}


void PreferencesDialog::onFinished(const int result) {
	if(result == Accepted) {
		// Store original values
		bool was_comic_enabled	=	config->isComicEnabled();
		bool was_manga_enabled	=	config->isMangaEnabled();
		QString orig_comic_path	=	config->getComicPath();
		QString orig_manga_path	=	config->getMangaPath();

		// Manage files
		if(config->manageFiles() != library_tab->manage_files->isChecked()) {
			config->setManageFiles(library_tab->manage_files->isChecked());
		}

		// Group by publisher
		if(config->groupByPublisher() != library_tab->group_by_publisher->isChecked()) {
			config->setGroupByPublisher(library_tab->group_by_publisher->isChecked());
		}

		// Comics enabled
		if(config->isComicEnabled() != library_tab->comic_enabled->isChecked()) {
			config->setComicEnabled(library_tab->comic_enabled->isChecked());
		}

		// Comic location
		if(config->getComicPath() != library_tab->comic_location->getPath()) {
			config->setComicDir(library_tab->comic_location->getPath());
		}

		// Manga enabled
		if(config->isMangaEnabled() != library_tab->manga_enabled->isChecked()) {
			config->setMangaEnabled(library_tab->manga_enabled->isChecked());
		}

		// Manga location
		if(config->getMangaPath() != library_tab->manga_location->getPath()) {
			config->setMangaDir(library_tab->manga_location->getPath());
		}

		// Save config to file
		config->save();

		// If manage files is enabled, then cleanup files
		if(config->manageFiles()) {
			library->cleanupFiles();
		} else {
			// Else, if Comics or Manga were disabled, warn that they will be removed from library
			if(!config->isComicEnabled() && was_comic_enabled) {
				QString msg	=	tr("You have disabled the Comics library, however 'manage ") +
								tr("files' is disabled. This means that the comics will be ") +
								tr("removed from the library, but remain in their current ") +
								tr("location: ") + orig_comic_path;
				QMessageBox::warning(this, tr("Warning"), msg);
			} else if(!config->isMangaEnabled() && was_manga_enabled) {
				QString msg	=	tr("You have disabled the Manga library, however 'mangage ") +
								tr("files' is disabled. This means that the manga will be ") +
								tr("removed from the library, but remain in their current ") +
								tr("location: ") + orig_manga_path;
				QMessageBox::warning(this, tr("Warning"), msg);
			}
		}

		main_side_pane->refreshLibraries();
		library_view->resetScope();
	}
}


void PreferencesDialog::toggleComicEnabled(int state) {
	library_tab->comic_location->setEnabled(state);
	library_tab->comic_location_help->setEnabled(state);
	verifyOkButton();
}


void PreferencesDialog::toggleMangaEnabled(int state) {
	library_tab->manga_location->setEnabled(state);
	library_tab->manga_location_help->setEnabled(state);
	verifyOkButton();
}


/**
 * If both Comics and Manga are disabled then OK button should be disabled.
 */
void PreferencesDialog::verifyOkButton() {
	if(
		(
			library_tab->comic_enabled->checkState() == Qt::Checked &&
			library_tab->comic_location->isValid()
		) || (
			library_tab->manga_enabled->checkState() == Qt::Checked &&
			library_tab->manga_location->isValid()
		)
	) button_box->button(QDialogButtonBox::Ok)->setEnabled(true);
	else button_box->button(QDialogButtonBox::Ok)->setEnabled(false);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYTAB PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void PreferencesDialog::LibraryTab::init() {
	if(library_tab == nullptr) {
		library_tab = new LibraryTab;
	}
}


void PreferencesDialog::LibraryTab::destroy() {
	delete library_tab;
	library_tab = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYTAB PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PreferencesDialog::LibraryTab::LibraryTab() {
	form_layout				=	new QFormLayout(this);

	// Setup form items
	manage_files			=	new QCheckBox(tr("Manage Files"), this);
	manage_files_help		=	new HelpButton(tr("If this is enabled then eComics will keep ") +
								tr("your files organized, meaning it will move and rename them ") +
								tr("when necessary."), this);
	group_by_publisher		=	new QCheckBox(tr("Group by Publisher"), this);
	group_by_publisher_help	=	new HelpButton(tr("If this is enabled then comics will be ") +
								tr("organized by Publisher/Series/Volume, if it is not enabled ") +
								tr("then they will be organized by Series/Volume."), this);
	comic_enabled			=	new QCheckBox(tr("Enable Comics"), this);
	comic_enabled_help		=	new HelpButton(tr("eComics supports organizing Comics and Manga ") +
								tr("in different folders. If you have Comics, or you just want ") +
								tr("everthing in the same folder, then enable this option. You ") +
								tr("MUST have either Comics or Manga enabled."), this);
	comic_location			=	new FilePathEdit(this, tr("Location of Comics"),
								tr("Open Comics Directory"), QDir::homePath(),
								QFileDialog::Directory);
	comic_location_help		=	new HelpButton(tr("The path to where your Comics are located ") +
								tr("should go here."), this);
	manga_enabled			=	new QCheckBox(tr("Enable Manga"), this);
	manga_enabled_help		=	new HelpButton(tr("If you have Manga, and you want it to be in ") +
								tr("it's own folder then you should enable this. If you have ") +
								tr("Comics and Manga, and you want them both in the same ") +
								tr("folder, then you should just enable Comics and leave this ") +
								tr("disabled."), this);
	manga_location			=	new FilePathEdit(this, tr("Location of Manga"),
								tr("Open Manga Directory"), QDir::homePath(),
								QFileDialog::Directory);
	manga_location_help		=	new HelpButton(tr("The path to where your Manga is located ") +
								tr("should go here."), this);

	manage_files->setChecked(config->manageFiles());
	group_by_publisher->setChecked(config->groupByPublisher());
	comic_enabled->setChecked(config->isComicEnabled());
	comic_location->setEnabled(config->isComicEnabled());
	if(config->isComicEnabled()) comic_location->setPath(config->getComicPath());
	manga_enabled->setChecked(config->isMangaEnabled());
	manga_location->setEnabled(config->isMangaEnabled());
	if(config->isMangaEnabled()) manga_location->setPath(config->getMangaPath());

	// Add items to form
	form_layout->addRow(manage_files, manage_files_help);
	form_layout->addRow(group_by_publisher, group_by_publisher_help);
	form_layout->addRow(comic_enabled, comic_enabled_help);
	form_layout->addRow(comic_location, comic_location_help);
	form_layout->addRow(manga_enabled, manga_enabled_help);
	form_layout->addRow(manga_location, manga_location_help);
}


PreferencesDialog::LibraryTab::~LibraryTab() {
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
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										LAYOUTTAB PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void PreferencesDialog::LayoutTab::init() {
	if(layout_tab == nullptr) {
		layout_tab = new LayoutTab;
	}
}


void PreferencesDialog::LayoutTab::destroy() {
	delete layout_tab;
	layout_tab = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										LAYOUTTAB PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PreferencesDialog::LayoutTab::LayoutTab() {
	// TODO: PLACE HOLDER
}


PreferencesDialog::LayoutTab::~LayoutTab() {
	// TODO: PLACE HOLDER
}