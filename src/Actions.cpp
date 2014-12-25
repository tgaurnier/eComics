#include "Actions.hpp"

#include <QApplication>
#include <QShortcut>
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


/**
 * QToolBar has built in toggle action, use this method to set tool_bar_action to built in action.
 */
void Actions::setToolBarAction(QAction *action) {
	tool_bar_action = action;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										ACTIONS PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


Actions::Actions(QWidget *parent) {
	//TODO: THIS IS WHERE I'M AT, FINISH CONNECTING ACTIONS
	f12_shortcut	=	new QShortcut(QKeySequence("F12"), parent);
	style			=	QApplication::style();

	// Create file actions
	open_action			=	new QAction(tr("&Open"), parent);
	new_list_action		=	new QAction(tr("&New List"), parent);
	add_comics_action	=	new QAction(tr("&Add Comics"), parent);
	quit_action			=	new QAction(tr("&Quit"), parent);

	// Create edit actions
	add_to_list_action	=	new QAction(tr("&Add to List"), parent);
	info_action			=	new QAction(style->standardIcon(QStyle::SP_FileDialogDetailedView),
							tr("&Info"), parent);
	remove_action		=	new QAction(tr("Remove from Library"), parent);
	delete_action		=	new QAction(tr("Delete from Disk"), parent);
	preferences_action	=	new QAction(tr("&Preferences"), parent);

	// Create view actions
	full_screen_action	=	new QAction(tr("&Full Screen"), parent);
	side_pane_action	=	new QAction(tr("Show Side Pane"), parent);
	status_bar_action	=	new QAction(tr("Show Status Bar"), parent);

	// Create tools actions
	scan_library_action		=	new QAction(tr("&Scan Library"), parent);
	cleanup_files_action	=	new QAction(tr("&Cleanup Files"), parent);
	convert_action			=	new QAction(tr("&Convert"), parent);

	// Create help actions
	about_action		=	new QAction(tr("&About eComics"), parent);
	faq_action			=	new QAction(tr("&FAQ"), parent);
	report_bug_action	=	new QAction(tr("&Report Bugs"), parent);

	// Create library navigation actions
	navigate_back_action	=	new QAction(style->standardIcon(QStyle::SP_ArrowLeft), tr("&Back"),
								parent);

	full_screen_action->setCheckable(true);
	side_pane_action->setCheckable(true);
	status_bar_action->setCheckable(true);

	// Connect keyboard shortcuts
	connect(f12_shortcut, SIGNAL(activated()), full_screen_action, SLOT(toggle()));
}


Actions::~Actions() {
	// Delete file actions
	delete open_action;
	delete new_list_action;
	delete add_comics_action;
	delete quit_action;

	// Delete edit actions
	delete add_to_list_action;
	delete info_action;
	delete remove_action;
	delete delete_action;
	delete preferences_action;

	// Delete view actions
	delete full_screen_action;
	delete side_pane_action;
	delete status_bar_action;

	// Delete tools actions
	delete scan_library_action;
	delete cleanup_files_action;
	delete convert_action;

	// Delete help actions
	delete about_action;
	delete faq_action;
	delete report_bug_action;

	// Delete library navigation actions
	delete navigate_back_action;
}