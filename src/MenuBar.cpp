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
	file_menu->addAction(eComics::actions->quit());

	// Add edit actions
	edit_menu->addAction(eComics::actions->info());
}