#include "MainSidePane.hpp"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QSettings>
#include <QStringListModel>

#include "Actions.hpp"
#include "Config.hpp"


/**
 * Define singleton instance pointers.
 */
MainSidePane *main_side_pane = nullptr;
MainSidePane::LibraryListView *MainSidePane::library_list_view	=	nullptr;
MainSidePane::UserListView *MainSidePane::user_list_view		=	nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINSIDEPANE PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MainSidePane::init(QWidget *parent) {
	if(main_side_pane == nullptr) {
		main_side_pane = new MainSidePane(parent);
		main_side_pane->restoreSettings();
	}
}


void MainSidePane::destroy() {
	main_side_pane->saveSettings();
	delete main_side_pane;
	main_side_pane = nullptr;
}


void MainSidePane::refreshLibraries() {
	library_list_view->refresh();
}


void MainSidePane::refreshLists() {
	user_list_view->refresh();
}


void MainSidePane::initSelectedList() {
	// Set selected library or list
	//TODO: SELECT LAST SELECTED LIBRARY OR USER-MADE LIST, FOR NOW JUST SELECT COMICS OR MANGA LIBRARY
	library_list_view->setCurrentIndex(library_list_view->model()->index(
		(config->isComicEnabled() ? 0 : 1), 0, QModelIndex()
	));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINSIDEPANE PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainSidePane::MainSidePane(QWidget *parent) : QWidget(parent) {
	// Initialize child widgets
	layout			=	new QVBoxLayout(this);
	library_label	=	new QLabel(tr("Library"), this);
	lists_label		=	new QLabel(tr("Lists"), this);
	LibraryListView::init(this);
	UserListView::init(this);

	// Add items to layout and root widget
	layout->addWidget(library_label);
	layout->addWidget(library_list_view);
	layout->addWidget(lists_label);
	layout->addWidget(user_list_view);
	setLayout(layout);

	// Set width
	setMinimumWidth(50);
	setMaximumWidth(300);

	// Connect signals
	connect(eComics::actions->sidePane(), SIGNAL(toggled(bool)), this,
			SLOT(toggleVisibility(bool)));
}


MainSidePane::~MainSidePane() {
	delete layout;
	delete library_label;
	delete lists_label;
	LibraryListView::destroy();
	LibraryListView::destroy();
}


void MainSidePane::toggleVisibility(bool visible) {
	setVisible(visible);
	is_visible = visible;
}


/**
 * Restore widget settings.
 */
void MainSidePane::restoreSettings() {
	QSettings settings("ToryGaurnier", "eComics");

	if(settings.contains("MainSidePane.Visible")) {
		is_visible = settings.value("MainSidePane.Visible").toBool();
	} else {
		is_visible = true;
	}

	eComics::actions->sidePane()->setChecked(is_visible);
	setVisible(is_visible);
}


/**
 * Save widget settings.
 */
void MainSidePane::saveSettings() {
	QSettings settings("ToryGaurnier", "eComics");
	settings.setValue("MainSidePane.Visible", is_visible);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYLISTVIEW PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MainSidePane::LibraryListView::init(QWidget *parent) {
	if(library_list_view == nullptr) {
		library_list_view = new LibraryListView(parent);
	}
}


void MainSidePane::LibraryListView::destroy() {
	delete library_list_view;
	library_list_view = nullptr;
}


/**
 * Add/remove libraries (Comics and Manga) to/from view as necessary.
 */
void MainSidePane::LibraryListView::refresh() {
	int i = 0;

	if(config->isAllEnabled() && !internal_model->stringList().contains(tr("All"))) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("All"));
		i++;
	} else if(!config->isAllEnabled() && internal_model->stringList().contains(tr("All"))) {
		internal_model->removeRow(internal_model->stringList().indexOf(tr("All")));
	}

	if(config->isComicEnabled() && !internal_model->stringList().contains(tr("Comics"))) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("Comics"));
		i++;
	} else if(!config->isComicEnabled() && internal_model->stringList().contains(tr("Comics"))) {
		internal_model->removeRow(internal_model->stringList().indexOf(tr("Comics")));
	}

	if(config->isMangaEnabled() && !internal_model->stringList().contains(tr("Manga"))) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("Manga"));
	} else if(!config->isMangaEnabled() && internal_model->stringList().contains(tr("Manga"))) {
		internal_model->removeRow(internal_model->stringList().indexOf(tr("Manga")));
	}

	setFixedHeight((this->sizeHintForRow(0) + 5) * internal_model->rowCount());
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								LIBRARYLISTVIEW PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainSidePane::LibraryListView::LibraryListView(QWidget *parent) : QListView(parent) {
	internal_model = new QStringListModel(this);

	// Populate model
	int i = 0;
	if(config->isAllEnabled()) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("All"));
		i++;
	}
	if(config->isComicEnabled()) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("Comics"));
		i++;
	}
	if(config->isMangaEnabled()) {
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("Manga"));
	}

	// Attach model
	setModel(internal_model);

	// Set sizing
	setSpacing(2);
	setFixedHeight((this->sizeHintForRow(0) + 5) * internal_model->rowCount());

	// Disable edit triggers
	setEditTriggers(QAbstractItemView::NoEditTriggers);
}


MainSidePane::LibraryListView::~LibraryListView() {
	delete internal_model;
}


/**
 * Deselect any selected item from UserListView, set selected list in Config.
 */
void MainSidePane::LibraryListView::selectionChanged(const QItemSelection &selected,
		const QItemSelection &deselected) {
	user_list_view->clearSelection();
	QStringList internal_list	=	internal_model->stringList();
	int selected_index			=	selected.indexes().at(0).row();

	// If there is temporarily nothing selected, avoid segfault
	if(selected.length() > 0) {
		if(config->isAllEnabled() && internal_list.at(selected_index) == "All") {
			config->setSelectedList("Library::All");
		} else if(config->isComicEnabled() && internal_list.at(selected_index) == "Comics") {
			config->setSelectedList("Library::Comics");
		} else if(config->isMangaEnabled() && internal_list.at(selected_index) == "Manga") {
			config->setSelectedList("Library::Manga");
		}

		emit ((MainSidePane*)parentWidget())->selectedListChanged(config->getSelectedList());
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									USERLISTVIEW PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MainSidePane::UserListView::init(QWidget *parent) {
	if(user_list_view == nullptr) {
		user_list_view = new UserListView(parent);
	}
}


void MainSidePane::UserListView::destroy() {
	delete user_list_view;
	user_list_view = nullptr;
}


/**
 * Add/remove user lists to/from view as necessary.
 */
void MainSidePane::UserListView::refresh() {
	//TODO
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									USERLISTVIEW PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainSidePane::UserListView::UserListView(QWidget *parent) : QListView(parent) {
	internal_model = new QStringListModel(QStringList()/**TODO: REPLACE WITH LIST OF USER-LIST NAMES**/, this);

	setModel(internal_model);
	setSpacing(2);
	setEditTriggers(QAbstractItemView::DoubleClicked);
}


MainSidePane::UserListView::~UserListView() {
	delete internal_model;
}


/**
 * Deselect item from LibraryListView, set selected list in Config.
 */
void MainSidePane::UserListView::selectionChanged(const QItemSelection &selected,
		const QItemSelection &deselected) {
	library_list_view->clearSelection();
	//TODO config->setSelectedList("UserList::" + (LIST OF USER-LIST NAMES)[selected.indexes().at(0).row()]);

	emit ((MainSidePane*)parentWidget())->selectedListChanged(config->getSelectedList());
}