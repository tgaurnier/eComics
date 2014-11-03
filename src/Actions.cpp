#include "Actions.hpp"

#include <QApplication>
#include <QStyle>


using namespace eComics;


Actions *eComics::actions = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										ACTIONS PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void Actions::init(QWidget *parent) {
	if(eComics::actions == nullptr) {
		eComics::actions = new Actions(parent);
	}
}


void Actions::destroy() {
	delete eComics::actions;
	eComics::actions = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										ACTIONS PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


Actions::Actions(QWidget *parent) {
	style = QApplication::style();
	// Create file actions
	new_list_action		=	new QAction(tr("&New list"), parent);
	add_comics_action	=	new QAction(tr("&Add comics"), parent);
	quit_action			=	new QAction(tr("&Quit"), parent);

	// Create edit actions
	info_action	=	new QAction(style->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Info"),
					parent);

	// Create library navigation actions
	navigate_back_action	=	new QAction(style->standardIcon(QStyle::SP_ArrowLeft), tr("&Back"),
								parent);
	//TODO: THIS IS WHERE I'M AT, FINISH ACTIONS

	// Connect file actions
	connect(quit_action, SIGNAL(triggered()), qApp, SLOT(quit()));

	//TODO: CONNECT REST OF ACTIONS
}


Actions::~Actions() {
	// Delete file actions
	delete new_list_action;
	delete add_comics_action;
	delete quit_action;

	// Delete edit actions
	delete info_action;

	// Delete library navigation actions
	delete navigate_back_action;
}
