#include "MainView.hpp"

#include <QDebug>
#include <QSettings>

#include "Actions.hpp"
#include "LibraryView.hpp"
#include "ToolBar.hpp"


MainView *main_view = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINVIEW PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MainView::init(QWidget *parent) {
	if(main_view == nullptr) {
		main_view = new MainView(parent);
	}
}


void MainView::destroy() {
	delete main_view;
	main_view = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									MAINVIEW PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MainView::MainView(QWidget *parent) : QMainWindow(parent) {
	ToolBar::init(this);
	LibraryView::init(this);

	// Add tool bar and library view
	addToolBar(tool_bar);
	setCentralWidget(library_view);

	// Set QMainWindow to a widget instead of top level window
	setWindowFlags(Qt::Widget);

	// Restore widget settings
	restoreSettings();
}


MainView::~MainView() {
	saveSettings();

	ToolBar::destroy();
	LibraryView::destroy();
}


/**
 * Save widget settings.
 */
void MainView::saveSettings() {
	QSettings settings("ToryGaurnier", "eComics");
	settings.setValue("MainView.State", saveState());
}


/**
 * Restore widget settings.
 */
void MainView::restoreSettings() {
	QSettings settings("ToryGaurnier", "eComics");

	if(settings.contains("MainView.State")) {
		restoreState(settings.value("MainView.State").toByteArray());
	}
}