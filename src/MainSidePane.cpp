#include "MainSidePane.hpp"

#include <QApplication>
#include <QLabel>
#include <QListView>
#include <QSettings>
#include <QStringListModel>
#include <QToolBar>
#include <QVBoxLayout>

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
	library_list_view->setCurrentIndex(library_list_view->model()->index(0, 0, QModelIndex()));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINSIDEPANE PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainSidePane::MainSidePane(QWidget *parent) : QWidget(parent) {
	// Initialize child widgets
	layout			=	new QVBoxLayout(this);
	library_label	=	new QLabel(tr("Library"), this);
	lists_label		=	new QLabel(tr("Lists"), this);
	tool_bar		=	new QToolBar(this);
	LibraryListView::init(this);
	UserListView::init(this);

	// Set tool bar icons to small
	int metric = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
	tool_bar->setIconSize(QSize(metric, metric));
	tool_bar->setFixedHeight(metric + 5);

	/**
	 * NOTE
	 *
	 * MainSidePane has local versions of actions that go on toolbar below UserListView, they
	 * connect directly to the global versions from eComics::Actions, however these local versions
	 * add icons for the toolbar.
	 */

	// Prepare local version of 'add list action'
	new_list_action = new QAction(
		QIcon::fromTheme("list-add"), eComics::actions->newList()->text(), this);
	connect(eComics::actions->newList(), SIGNAL(changed()), this, SLOT(onNewListActionChanged()));
	connect(eComics::actions->newList(), SIGNAL(triggered()), new_list_action, SIGNAL(triggered()));

	// Prepare local version of 'remove list action'
	remove_list_action = new QAction(
		QIcon::fromTheme("list-remove"), eComics::actions->removeList()->text(), this);
	connect(eComics::actions->removeList(), SIGNAL(changed()),
		this, SLOT(onRemoveListActionChanged()));
	connect(eComics::actions->removeList(), SIGNAL(triggered()),
			remove_list_action, SIGNAL(triggered()));
	remove_list_action->setEnabled(false);

	// Prepare local version of 'edit list action'
	edit_list_action = new QAction(
		QIcon::fromTheme("document-properties"), eComics::actions->editList()->text(), this);
	connect(eComics::actions->editList(), SIGNAL(changed()), this, SLOT(onEditListActionChanged()));
	connect(eComics::actions->editList(), SIGNAL(triggered()),
		edit_list_action, SIGNAL(triggered()));
	edit_list_action->setEnabled(false);

	// Setup tool bar
	tool_bar->addAction(new_list_action);
	tool_bar->addAction(remove_list_action);
	tool_bar->addAction(edit_list_action);

	// Add items to layout and root widget
	layout->addWidget(library_label);
	layout->addWidget(library_list_view);
	layout->addWidget(lists_label);
	layout->addWidget(user_list_view);
	layout->addWidget(tool_bar);
	setLayout(layout);

	// Set width
	setMinimumWidth(50);
	setMaximumWidth(300);

	// Connect signals
	connect(eComics::actions->sidePane(), SIGNAL(toggled(bool)), this,
			SLOT(toggleVisibility(bool)));
}


MainSidePane::~MainSidePane() {
	delete new_list_action;
	delete remove_list_action;
	delete edit_list_action;
	delete layout;
	delete library_label;
	delete lists_label;
	LibraryListView::destroy();
	LibraryListView::destroy();
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


void MainSidePane::toggleVisibility(bool visible) {
	setVisible(visible);
	is_visible = visible;
}


/**
 * When edit list action from eComics::Actions is disabled or enabled, mimic that here.
 */
void MainSidePane::onEditListActionChanged() {
	if(eComics::actions->info()->isEnabled()) {
		edit_list_action->setEnabled(true);
	} else {
		edit_list_action->setEnabled(false);
	}
}


/**
 * When new list action from eComics::Actions is disabled or enabled, mimic that here.
 */
void MainSidePane::onNewListActionChanged() {
	if(eComics::actions->info()->isEnabled()) {
		new_list_action->setEnabled(true);
	} else {
		new_list_action->setEnabled(false);
	}
}


/**
 * When remove list action from eComics::Actions is disabled or enabled, mimic that here.
 */
void MainSidePane::onRemoveListActionChanged() {
	if(eComics::actions->info()->isEnabled()) {
		remove_list_action->setEnabled(true);
	} else {
		remove_list_action->setEnabled(false);
	}
}


void MainSidePane::onUserListViewSelectionChanged(const QItemSelection &selected,
		const QItemSelection &deselected) {
	if(selected.size() == 0) {
		eComics::actions->removeList()->setEnabled(false);
		eComics::actions->editList()->setEnabled(false);
	} else {
		eComics::actions->removeList()->setEnabled(true);
		eComics::actions->editList()->setEnabled(true);
	}
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