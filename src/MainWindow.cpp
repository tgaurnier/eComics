#include "MainWindow.hpp"

#include <QSplitter>
#include <QStatusBar>

#include "Actions.hpp"
#include "ComicInfoDialog.hpp"
#include "Config.hpp"
#include "Library.hpp"
#include "MainSidePane.hpp"
#include "MainView.hpp"
#include "MenuBar.hpp"


MainWindow *main_window = nullptr;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINWINDOW PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MainWindow::init() {
	if(main_window == nullptr) {
		main_window = new MainWindow;
	}
}


void MainWindow::destroy() {
	if(main_window != nullptr) {
		delete main_window;
		main_window = nullptr;
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINWINDOW PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Setup main window, initializing child objects/widgets. Order of object initialization is
 * EXTREMELY important.
 *
 * Order of object initialization:
 * - Config
 * - Library
 * - eComics::Actions
 * - QSplitter main_splitter
 * - MenuBar
 * - MainSidePane
 * - MainView
 * - MainSidePane::initSelectedList()
 */
MainWindow::MainWindow() {
	// Initialize config singleton
	Config::init();

	setWindowIcon(QIcon(config->getIconPath()));

	// If config is empty, launch first run dialog
	if(config->isEmpty()) {
		//TODO: THIS IS TEMPORARY UNTIL I MAKE FIRST RUN DIALOG
		qDebug() << "CONFIG IS EMPTY";
		config->setComicEnabled(true);
		config->setMangaEnabled(true);
		config->setComicDir("/home/kornklown/eLibrary/TEST/Comics");
		config->setMangaDir("/home/kornklown/eLibrary/TEST/Manga");
		config->save();
	}

	// Initialize library right after Config is finished, but before everything else
	Library::init();

	// Initialize global actions
	eComics::Actions::init(this);

	// Initialize child widgets
	status_bar		=	statusBar();
	main_splitter	=	new QSplitter(this);
	MenuBar::init(this);
	MainSidePane::init(main_splitter);
	MainView::init(main_splitter); // Must be initialized after MainSidePane
	main_side_pane->initSelectedList(); // Must happen after MainView is initialized

	// Add widgets to window
	setCentralWidget(main_splitter);
	main_splitter->addWidget(main_side_pane);
	main_splitter->addWidget(main_view);
	main_splitter->setSizes( {150, 650} );
	main_splitter->setCollapsible(1, false);

	resize(800, 600);

	// Initialize comic info dialog
	comic_info_dialog->init(this);
}


MainWindow::~MainWindow() {
	// Destroy all initialized singletons
	MenuBar::destroy();
	MainSidePane::destroy();
	MainView::destroy();
	ComicInfoDialog::destroy();

	// Delete all other malloced objects
	delete main_splitter;
}