#include "MenuBar.hpp"

#include <QMainWindow>
#include <QMenuBar>

#include "Actions.hpp"


MenuBar *menu_bar = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								MENUBAR PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void MenuBar::init(QMainWindow *parent) {
	if(menu_bar == nullptr) {
		menu_bar = new MenuBar(parent);
	}
}


void MenuBar::destroy() {
	delete menu_bar;
	menu_bar = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								MENUBAR PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MenuBar::MenuBar(QMainWindow *parent) : QObject(parent) {
	q_menu_bar	=	parent->menuBar();
	file_menu	=	q_menu_bar->addMenu(tr("&File"));
	edit_menu	=	q_menu_bar->addMenu(tr("&Edit"));
	view_menu	=	q_menu_bar->addMenu(tr("&View"));
	tools_menu	=	q_menu_bar->addMenu(tr("&Tools"));
	help_menu	=	q_menu_bar->addMenu(tr("&Help"));

	// Add file actions
	file_menu->addAction(eComics::actions->open());
	file_menu->addAction(eComics::actions->newList());
	file_menu->addAction(eComics::actions->addComics());
	file_menu->addAction(eComics::actions->quit());

	// Add edit actions
	edit_menu->addAction(eComics::actions->addToList());
	edit_menu->addAction(eComics::actions->info());
	edit_menu->addAction(eComics::actions->remove());
	edit_menu->addAction(eComics::actions->deleteFile());
	edit_menu->addAction(eComics::actions->preferences());

	// Add view actions
	view_menu->addAction(eComics::actions->fullScreen());
	view_menu->addAction(eComics::actions->toolBar());
	view_menu->addAction(eComics::actions->sidePane());
	view_menu->addAction(eComics::actions->statusBar());

	// Add tools actions
	tools_menu->addAction(eComics::actions->scanLibrary());
	tools_menu->addAction(eComics::actions->cleanupFiles());
	tools_menu->addAction(eComics::actions->convert());

	// Add help actions
	help_menu->addAction(eComics::actions->about());
	help_menu->addAction(eComics::actions->faq());
	help_menu->addAction(eComics::actions->reportBug());
}