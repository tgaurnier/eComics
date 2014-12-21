#include "ToolBar.hpp"

#include <QLabel>

#include "Actions.hpp"


ToolBar *tool_bar = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										TOOLBAR PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ToolBar::init(QWidget *parent) {
	if(tool_bar == nullptr) {
		tool_bar = new ToolBar(tr("Toolbar"), parent);
		tool_bar->initActions();
	}
}


void ToolBar::destroy() {
	delete tool_bar;
	tool_bar = nullptr;
}


void ToolBar::disableBackButton() {
	eComics::actions->navigateBack()->setEnabled(false);
}


void ToolBar::enableBackButton() {
	eComics::actions->navigateBack()->setEnabled(true);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									TOOLBAR PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ToolBar::ToolBar(const QString &title, QWidget *parent) : QToolBar(title, parent) {
	setObjectName("ToolBar");

	// Disable back button on first load
	disableBackButton();
}


ToolBar::~ToolBar() {}

/**
 * QActions cannot be added to ToolBar within constructor, so this is used to add them after.
 */
void ToolBar::initActions() {
	addAction(eComics::actions->navigateBack());
	addAction(eComics::actions->info());
	//TODO: FINISH ADDING ACTIONS
}