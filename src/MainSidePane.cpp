#include "MainSidePane.hpp"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QStringListModel>

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
	}
}


void MainSidePane::destroy() {
	delete main_side_pane;
	main_side_pane = nullptr;
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
}


MainSidePane::~MainSidePane() {
	delete layout;
	delete library_label;
	delete lists_label;
	LibraryListView::destroy();
	LibraryListView::destroy();
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								LIBRARYLISTVIEW PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainSidePane::LibraryListView::LibraryListView(QWidget *parent) : QListView(parent) {
	internal_model = new QStringListModel(this);

	// Populate model
	if(config->isComicEnabled()) {
		int i = internal_model->rowCount();
		internal_model->insertRow(i);
		internal_model->setData(internal_model->index(i), tr("Comics"));
	}
	if(config->isMangaEnabled()) {
		int i = internal_model->rowCount();
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
	if(selected.indexes().at(0).row() == 0 && config->isComicEnabled()) {
		config->setSelectedList("Library::Comics");
	} else {
		config->setSelectedList("Library::Manga");
	}

	emit ((MainSidePane*)parentWidget())->selectedListChanged(config->getSelectedList());
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