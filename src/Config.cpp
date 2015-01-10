#include "Config.hpp"


Config *config = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									CONFIG PUBLIC METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * See Config::Config(), this may throw exceptions, in which case the program should popup an
 * appropriate message and exit, this program can not run without proper configs.
 */
void Config::init() {
	if(config == nullptr) {
		config = new Config;
	}
}


void Config::destroy() {
	if(config != nullptr) {
		delete config;
		config = nullptr;
	}
}


/**
 * Save config data to file
 */
void Config::save() {
	//THIS IS WHERE I'M AT, SAVE NEW CONFIG MEMEBERS TO FILE
	// Open file for writing
	if(!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		throw eComics::Exception(eComics::FILE_ERROR, "Config::Config()",
				QString("Failed to open ") + file->fileName() + " for writing");
	}

	// Prepare buffer to write to file
	QByteArray buff("## eComics Configuration File ##\n\n");
	buff += "# WARNING #\n";
	buff += "# Do not edit this file, to change eComics settings just use the eComics program.\n";
	buff += "# The syntax of this file is simple, setting = value, it's VERY important that there ";
	buff += "is a space on each side of the =, and each setting/value pair MUST be on it's own ";
	buff += "line.\n\n";

	buff += "manage_files = " + QByteArray::number(manage_files) + "\n";
	buff += "group_by_publisher = " + QByteArray::number(group_by_publisher) + "\n";
	buff += "temp_dir = " + temp_dir.absolutePath() + "\n";
	buff += "thumb_dir = " + thumb_dir.absolutePath() + "\n";
	buff += QByteArray("all_enabled = ") + QByteArray::number(all_enabled) + "\n";
	buff += QByteArray("comic_enabled = ") + QByteArray::number(comic_enabled) + "\n";
	buff += QByteArray("manga_enabled = ") + QByteArray::number(manga_enabled) + "\n";
	if(comic_enabled) {
		buff += QByteArray("comic_dir = ") + comic_dir.absolutePath() + "\n";
	}
	if(manga_enabled) {
		buff += QByteArray("manga_dir = ") + manga_dir.absolutePath() + "\n";
	}

	// Write buffer to file
	if(file->write(buff) == -1) {
		throw eComics::Exception(eComics::FILE_ERROR, "Config::save()",
				QString("Failed to write to ") + file->fileName());
	}

	file->close();
}


/**
 * Basic getter methods, simply return appropriate value.
 */
QString Config::getIconPath() const { return icon_path; }
QString Config::getSplashPath() const { return splash_path; }
const QString& Config::getBinDir() const { return bin_dir; }
QDir Config::getRootDir() const { return root_dir; }
QString Config::getRootPath() const { return root_dir.absolutePath(); }
QDir Config::getThumbDir() const { return thumb_dir; }
QString Config::getThumbPath() const { return thumb_dir.absolutePath(); }
QDir Config::getTempDir() const { return temp_dir; }
QString Config::getTempPath() const { return temp_dir.absolutePath(); }
QDir Config::getComicDir() const { return comic_dir; }
QString Config::getComicPath() const { return comic_dir.absolutePath(); }
QDir Config::getMangaDir() const { return manga_dir; }
QString Config::getMangaPath() const { return manga_dir.absolutePath(); }
bool Config::manageFiles() const { return manage_files; }
bool Config::groupByPublisher() const { return group_by_publisher; }
bool Config::isEmpty() const { return empty; }
bool Config::isAllEnabled() const { return all_enabled; }
bool Config::isComicEnabled() const { return comic_enabled; }
bool Config::isMangaEnabled() const { return manga_enabled; }
QString Config::getSelectedList() const { return selected_list; }


/**
 * Basic setter methods, simply set appropriate variable to value of argument.
 *
 * Possible exceptions: Dir setters may throw a eComics::DIR_ERROR().
 */
void Config::setTempDir(const QString &path) { setDir(temp_dir, path); }
void Config::setThumbDir(const QString &path) { setDir(thumb_dir, path); }
void Config::setComicDir(const QString &path) { setDir(comic_dir, path); }
void Config::setMangaDir(const QString &path) { setDir(manga_dir, path); }
void Config::setAllEnabled(const bool val) {  all_enabled = val; }
void Config::setComicEnabled(const bool val) { comic_enabled = val; }
void Config::setMangaEnabled(const bool val) { manga_enabled = val; }
void Config::setManageFiles(const bool val) { manage_files = val; }
void Config::setGroupByPublisher(const bool val) { group_by_publisher = val; }
void Config::setSelectedList(const QString &val) { selected_list = val; }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									CONFIG PRIVATE METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Init Config singleton, setting up config files/dirs.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if opening config file fails.
 * - DIR_ERROR may be thrown if creating/setting dirs fails for any reason.
 */
Config::Config() {
	// Set bin_dir
	bin_dir = QCoreApplication::applicationDirPath();

	// Set icon_path
	icon_path	=	bin_dir + "/../res/images/eComics.svg";
	splash_path	=	bin_dir + "/../res/images/Splash.png";

	// Check if config dir exists, if it doesn't create it
	root_dir.cd(QDir::homePath());
	if(root_dir.exists(".eComics/")) {
		if(!root_dir.cd(".eComics/")) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::Config()",
					"Failed to cd to ~/.eComics/");
		}
	} else {
		if(!root_dir.mkpath(".eComics/")) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::Config()",
					"Failed to make path ~/.eComics/");
		}

		if(!root_dir.cd(".eComics/")) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::Config()",
					"Failed to cd to ~/.eComics/");
		}
	}

	// Initialize file pointer
	file = new QFile(root_dir.absolutePath() + "/settings.conf");

	// Check for config file and load it, create it if necessary
	if(file->exists() && file->size() > 0) {
		loadFromFile();
		empty = false;
	} else {
		// Create blank file, it'll be filled later
		if(!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw eComics::Exception(eComics::FILE_ERROR, "Config::Config()",
					QString("Failed to open ") + file->fileName() + " for writing");
		} else file->close();

		// Set default temp dir to root_dir/temp
		setTempDir(root_dir.absolutePath() + "/temp");

		// Set default thumb dir to root_dir/thumb
		setThumbDir(root_dir.absolutePath() + "/thumb");

		empty = true;
	}
}


Config::~Config() {
	delete file;
}


/**
 * Loads config data from file.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if opening config file fails.
 * - DIR_ERROR may be thrown if setting dirs fails for any reason.
 */
void Config::loadFromFile() {
	if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw eComics::Exception(eComics::FILE_ERROR, "Config::loadFromFile()",
				QString("Failed to open ") + file->fileName() + " for reading");
	}
	// Loop through file getting contents
	while (!file->atEnd()) {
		QByteArray line = file->readLine().trimmed(); // Make sure to trim whitespace

		if(line.isEmpty() || line[0] == '#') {
			continue;
		}

		// Tokenize line to be parsed
		QStringList tok = QString(line).split(QRegExp("\\s"));

		QString value("");
		if(tok[1] == "=") {
			for(int i = 2; i < tok.size(); i++) {
				value += tok[i];
			}
		} else {
			continue;
		}

		if(tok[0] == "manage_files") {
			setManageFiles(value.toInt());
		}

		else if(tok[0] == "group_by_publisher") {
			setGroupByPublisher(value.toInt());
		}

		else if(tok[0] == "temp_dir") {
			setTempDir(value);
		}

		else if(tok[0] == "thumb_dir") {
			setThumbDir(value);
		}

		else if(tok[0] == "all_enabled") {
			setAllEnabled(value.toInt());
		}

		else if(tok[0] == "comic_enabled") {
			setComicEnabled(value.toInt());
		}

		else if(tok[0] == "manga_enabled") {
			setMangaEnabled(value.toInt());
		}

		else if(tok[0] == "comic_dir") {
			setComicDir(value);
		}
		else if(tok[0] == "manga_dir") {
			setMangaDir(value);
		}
	}

	file->close();
}


/**
 * A convience function for dir setters.
 *
 * Possible exceptions:
 * - DIR_ERROR may be thrown if setting/creating dir fails.
 */
void Config::setDir(QDir &dir, const QString &path) {
	if(QFile::exists(path)) {
		if(!dir.cd(path)) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::setDir()",
					QString("Failed to cd to ") + path);
		}
	} else {
		if(!QDir::root().mkpath(path)) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::setDir()",
					QString("Failed to make path ") + path);
		}

		if(!dir.cd(path)) {
			throw eComics::Exception(eComics::DIR_ERROR, "Config::setDir()",
					QString("Failed to cd to ") + path);
		}
	}
}