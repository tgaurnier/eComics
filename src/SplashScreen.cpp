#include "SplashScreen.hpp"

#include <QApplication>

#include "Config.hpp"
#include "MainWindow.hpp"


SplashScreen *splash_screen = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									SPLASHSCREEN PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void SplashScreen::start() {
	if(splash_screen == nullptr) {
		QPixmap pixmap(config->getSplashPath());
		splash_screen = new SplashScreen(pixmap.scaledToHeight(600));
		splash_screen->show();
		QApplication::processEvents();
	}
}


void SplashScreen::finish() {
	splash_screen->QSplashScreen::finish(main_window);
	delete splash_screen;
	splash_screen = nullptr;
}


void SplashScreen::update(const QString &msg) {
	showMessage(msg, Qt::AlignCenter | Qt::AlignBottom);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									SPLASHSCREEN PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


SplashScreen::SplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen(pixmap, f) {
	initialize();
}


SplashScreen::SplashScreen(QWidget *parent, const QPixmap &pixmap, Qt::WindowFlags f) :
		QSplashScreen(parent, pixmap, f) {
	initialize();
}


SplashScreen::~SplashScreen() {}


/**
 * Used so that constructors aren't duplicating code.
 */
void SplashScreen::initialize() {
	setEnabled(false);

	// Setup font
	QFont font;
	font.setFamily("Comic Sans MS");
	font.setCapitalization(QFont::AllUppercase);
	font.setBold(true);
	font.setPixelSize(20);

	setFont(font);
}