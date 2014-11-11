#include "Actions.hpp"

#include <QApplication>
#include <QDir>
#include <QErrorMessage>
#include <QFileDialog>
#include <QShortcut>
#include <QStyle>

#include "ComicFile.hpp"
#include "Library.hpp"
#include "LibraryView.hpp"
#include "MainWindow.hpp"


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
	//TODO: THIS IS WHERE I'M AT, FINISH CONNECTING ACTIONS
	f12_shortcut	=	new QShortcut(QKeySequence("F12"), parent);
	style			=	QApplication::style();

	// Create file actions
	open_action			=	new QAction(tr("&Open"), parent);
	new_list_action		=	new QAction(tr("&New list"), parent);
	add_comics_action	=	new QAction(tr("&Add comics"), parent);
	quit_action			=	new QAction(tr("&Quit"), parent);

	// Create edit actions
	add_to_list_action	=	new QAction(tr("&Add to list"), parent);
	info_action			=	new QAction(style->standardIcon(QStyle::SP_FileDialogDetailedView),
							tr("&Info"), parent);
	remove_action		=	new QAction(tr("&Remove from library"), parent);
	delete_action		=	new QAction(tr("&Delete from disk"), parent);
	preferences_action	=	new QAction(tr("&Preferences"), parent);

	// Create view actions
	fullscreen_action	=	new QAction(tr("&Fullscreen"), parent);
	side_pane_action	=	new QAction(tr("&Show side pane"), parent);
	statusbar_action	=	new QAction(tr("&Show status bar"), parent);

	// Create tools actions
	scan_library_action	=	new QAction(tr("&Scan library"), parent);
	convert_action		=	new QAction(tr("&Convert"), parent);

	// Create help actions
	about_action		=	new QAction(tr("&About eComics"), parent);
	faq_action			=	new QAction(tr("&FAQ"), parent);
	report_bug_action	=	new QAction(tr("&Report bugs"), parent);

	// Create library navigation actions
	navigate_back_action	=	new QAction(style->standardIcon(QStyle::SP_ArrowLeft), tr("&Back"),
								parent);

	// Connect actions to local slots
	connect(add_comics_action, SIGNAL(triggered()), this, SLOT(addComicsActivated()));

	// Connect keyboard shortcuts
	connect(f12_shortcut, SIGNAL(activated()), fullscreen_action, SIGNAL(triggered()));
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
	delete fullscreen_action;
	delete side_pane_action;
	delete statusbar_action;

	// Delete tools actions
	delete scan_library_action;
	delete convert_action;

	// Delete help actions
	delete about_action;
	delete faq_action;
	delete report_bug_action;

	// Delete library navigation actions
	delete navigate_back_action;
}


/**
 * Open a files select dialog, copy selected comics to comics or manga dir, then add to library.
 */
void Actions::addComicsActivated() {
	QStringList path_list	=	QFileDialog::getOpenFileNames(
								main_window,
								"Select comics to import",
								QDir::homePath(),
								"Comic files(*.cb7 *.7z *.cbr *.rar *.cbz *zip *.pdf)");

	for(QString path : path_list) {
		ComicFile comic(path);

		if(comic.isNull()) {
			QErrorMessage err_msg(main_window);
			err_msg.showMessage(QString("Failed to open ") + path);
		} else {
			comic.move();
			library->append(comic);
			library_view->refreshModel();
		}
	}
}
