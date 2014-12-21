#include "MainWindow.hpp"

#include <QApplication>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>

#include "Actions.hpp"
#include "ComicInfoDialog.hpp"
#include "Config.hpp"
#include "FirstRunDialog.hpp"
#include "Library.hpp"
#include "MainSidePane.hpp"
#include "MainView.hpp"
#include "MenuBar.hpp"
#include "PreferencesDialog.hpp"


MainWindow *main_window		=	nullptr;
bool MainWindow::exiting	=	false;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINWINDOW PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Returns false when application is closed prematurely by cancelling first run dialog, returns true
 * when Window is successfully created or if it already exists.
 */
bool MainWindow::init() {
	if(main_window == nullptr) {
		main_window = new MainWindow;
		// If exited before first run dialog is finished.
		if(exiting) {
			MainWindow::destroy();
			return false;
		}
	}

	main_window->restoreSettings();
	return true;
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
		int result = FirstRunDialog::run(this);

		// If FirstRunDialog was cancelled then free memory and quit app
		if(!result) {
			Config::destroy();
			// This 'exiting' status needs to be passed back to main() function and then 'return 0'
			// Using exit(0) here sometimes results in strange segfault with no real stack trace o_O
			exiting = true;
			return;
		}
	}

	// Initialize global actions
	eComics::Actions::init(this);

	// Initialize library right after Config and Actions are finished, but before everything else
	Library::init();

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
	ComicInfoDialog::init(this);

	// Initialize preferences dialog
	PreferencesDialog::init(this);

	// Connect signals to actions
	connect(eComics::actions->quit(), SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(eComics::actions->fullscreen(), SIGNAL(triggered()), this, SLOT(toggleFullscreen()));
}


MainWindow::~MainWindow() {
	// Destroy all initialized singletons
	MenuBar::destroy();
	MainSidePane::destroy();
	MainView::destroy();
	ComicInfoDialog::destroy();

	// Delete all other malloced objects
	if(main_splitter != nullptr) delete main_splitter;
}


/**
 * On close save Window state and geometry.
 */
void MainWindow::closeEvent(QCloseEvent *event) {
	QSettings settings("ToryGaurnier", "eComics");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	QMainWindow::closeEvent(event);
}


/**
 * Restore settings.
 */
void MainWindow::restoreSettings() {
	QSettings settings("ToryGaurnier", "eComics");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}


void MainWindow::toggleFullscreen() {
	if(config->isFullscreen()) {
		showNormal();
		config->setFullscreen(false);
	} else {
		showFullScreen();
		config->setFullscreen(true);
	}
}