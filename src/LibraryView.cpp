#include "LibraryView.hpp"

//#include <QPalette>
#include <QPainter>

#include "Actions.hpp"
#include "ComicFile.hpp"
#include "Config.hpp"
#include "Library.hpp"
#include "MainSidePane.hpp"
#include "ToolBar.hpp"


LibraryView *library_view = nullptr;
LibraryView::LibraryModel *LibraryView::model = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYVIEW PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void LibraryView::init(QWidget *parent) {
	if(library_view == nullptr) {
		library_view = new LibraryView(parent);
	}
}


void LibraryView::destroy() {
	delete library_view;
}


ReferenceList<ComicFile> LibraryView::getSelectedComics() {
	ReferenceList<ComicFile> selected_list;

	for(QModelIndex index : selectionModel()->selectedIndexes()) {
		const QString &str = model->list.at(index.row());
		switch(model->cur_scope.category) {
			case model->PUBLISHER_SCOPE:
				selected_list.append(library->comicsFromPublisher(str));
				break;

			case model->SERIES_SCOPE:
				selected_list.append(library->comicsFromSeries(str, model->cur_scope.publisher));
				break;

			case model->VOLUME_SCOPE:
				selected_list.append(library->comicsFromVolume(model->cur_scope.series, str,
					model->cur_scope.publisher));
				break;

			case model->TITLE_SCOPE:
			case model->LIST_SCOPE:
				ComicFile &comic = (*library)[str.toLocal8Bit()];
				if(!comic.isNull()) selected_list.append(comic);
				break;
		}
	}

	return selected_list;
}


void LibraryView::refreshModel() {
	model->refreshList();
}


void LibraryView::onListChanged(QString list) {
	if(list.startsWith("Library::")) {
		model->setSelectedLibrary(list.right(list.size() - QString("Library::").size()));
	} else if(list.startsWith("UserList::")) {
		model->setSelectedUserList(list.right(list.size() - QString("UserList::").size()));
	}
}


/**
 * Pass activated signal to nested LibraryModel class.
 */
void LibraryView::onItemActivated(const QModelIndex &index) {
	model->onItemActivated(index);
}


void LibraryView::onNavigateBack() {
	model->navigateBack();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								LIBRARYVIEW PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


LibraryView::LibraryView(QWidget *parent) : QListView(parent) {
	icon_size = 256;
	LibraryModel::init(this);
	setModel(model);
	setSelectionMode(ExtendedSelection);
	setViewMode(QListView::IconMode);
	setIconSize(QSize(icon_size, icon_size));
	setSpacing(15);
	setResizeMode(Adjust);
	connect(eComics::actions->navigateBack(), SIGNAL(triggered()), this, SLOT(onNavigateBack()));
	connect(main_side_pane, SIGNAL(selectedListChanged(const QString &)),
		this, SLOT(onListChanged(const QString &)));
	connect(this, SIGNAL(activated(const QModelIndex &)),
		this, SLOT(onItemActivated(const QModelIndex &)));
}


LibraryView::~LibraryView() {
	LibraryModel::destroy();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYMODEL PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void LibraryView::LibraryModel::init(QObject *parent) {
	if(model == nullptr) {
		model = new LibraryModel(parent);
	}
}


void LibraryView::LibraryModel::destroy() {
	delete model;
}


QVariant LibraryView::LibraryModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid() || index.row() >= list.size()) {
		return QVariant();
	}

	if(role == Qt::DecorationRole) {
		switch(cur_scope.category) {
			case PUBLISHER_SCOPE: {
				QList<ComicFile> comic_list = library->getComicsFromPublisher(list.at(index.row()));
				return layeredIcon(comic_list);
			}

			case SERIES_SCOPE: {
				QList<ComicFile> comic_list = library->getComicsFromSeries(list.at(index.row()));
				return layeredIcon(comic_list);
			}

			case VOLUME_SCOPE: {
				QList<ComicFile> comic_list = library->getComicsFromVolume(cur_scope.series,
					list.at(index.row()));
				return layeredIcon(comic_list);
			}

			case TITLE_SCOPE: {
				ComicFile comic = library->at(list.at(index.row()).toLocal8Bit());
				return QIcon(comic.getThumbPath());
			}

			default:
				return QVariant();
		}
	} else if(role == Qt::DisplayRole) {
		QString str(""); // Initialize string to empty so .append() can be used

		switch(cur_scope.category) {
			case PUBLISHER_SCOPE:
			case SERIES_SCOPE:
				return list.at(index.row());

			case VOLUME_SCOPE:
				return cur_scope.series + "\nVol." + list.at(index.row());

			case TITLE_SCOPE:
			case LIST_SCOPE: {
				ComicInfo info = library->at(list.at(index.row()).toLocal8Bit()).info;

				if(!info.getSeries().isEmpty()) str.append(info.getSeries());
				if(!str.isEmpty()) str.append("\n");
				if(!info.getVolume().isEmpty()) str.append(QString("Vol. ") + info.getVolume());
				if(!str.endsWith("\n") && !info.getNumber().isEmpty()) str.append(" ");
				if(!info.getNumber().isEmpty()) str.append(QString("No. ") + info.getNumber());
				if(!str.endsWith("\n")) str.append("\n");
				str.append(info.getTitle());

				return str;
			}

			default:
				return QVariant();
		}
	} else return QVariant();
}


//Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const {
//	//TODO: DO WE NEED FLAGS?
//}


//QMimeData * LibraryModel::mimeData(const QModelIndexList &index_list) const {
//	//TODO: IMPLEMENT MIMEDATA() FOR DRAG/DROPPING COMICS
//}


/**
 * There should be a row for each item in internal list.
 */
int LibraryView::LibraryModel::rowCount(const QModelIndex &parent) const {
	return list.size();
}


/**
 * Populate internal list of LibraryModel to be displayed in LibraryView.
 */
void LibraryView::LibraryModel::refreshList() {
	emit layoutAboutToBeChanged();
	list.clear();

	for(ComicFile comic : *library) {
		// Skip comics that are not part of selected library
		if(!(comic.info.getManga() == "Yes" && cur_scope.library == "Manga") &&
			!(comic.info.getManga() == "No" && cur_scope.library == "Comics") &&
			cur_scope.user_list.isEmpty()) continue;

		switch(cur_scope.category) {
			case PUBLISHER_SCOPE:
				if(!list.contains(comic.info.getPublisher())) {
					list.append(comic.info.getPublisher());
				}
				break;

			case SERIES_SCOPE:
				if((cur_scope.publisher == comic.info.getPublisher() ||
						cur_scope.publisher == "All") && !list.contains(comic.info.getSeries())) {
					list.append(comic.info.getSeries());
				}
				break;

			case VOLUME_SCOPE:
				if((cur_scope.publisher == comic.info.getPublisher() ||
						cur_scope.publisher == "All") &&
						cur_scope.series == comic.info.getSeries() &&
						!list.contains(comic.info.getVolume())) {
					list.append(comic.info.getVolume());
				}
				break;

			case TITLE_SCOPE:
				if((cur_scope.publisher == comic.info.getPublisher() ||
						cur_scope.publisher == "All") &&
						cur_scope.series == comic.info.getSeries() &&
						cur_scope.volume == comic.info.getVolume()) {
					list.append(comic.getMd5Hash());
				}
				break;

			case LIST_SCOPE:
				//TODO: PARSE USER-MADE LIST TO ADD COMICS
				break;
		}
	}

	emit layoutChanged();

	// Update ToolBar by enabling/disabling back button
	if(cur_scope.category == PUBLISHER_SCOPE || cur_scope.category == LIST_SCOPE ||
			(!config->groupByPublisher() && cur_scope.category == SERIES_SCOPE)) {
		tool_bar->disableBackButton();
	} else tool_bar->enableBackButton();
}


void LibraryView::LibraryModel::setSelectedLibrary(const QString &library) {
	if(cur_scope.library != library) {
		cur_scope.library = library;
		if(config->groupByPublisher()) {
			cur_scope.category = PUBLISHER_SCOPE;
			cur_scope.publisher.clear();
		} else {
			cur_scope.category	=	SERIES_SCOPE;
			cur_scope.publisher	=	"All";
		}

		cur_scope.series.clear();
		cur_scope.volume.clear();
		cur_scope.user_list.clear();

		refreshList();
	}
}


void LibraryView::LibraryModel::setSelectedUserList(const QString &list) {
	cur_scope.category	=	LIST_SCOPE;
	cur_scope.user_list	=	list;
	cur_scope.library.clear();
	cur_scope.publisher.clear();
	cur_scope.series.clear();
	cur_scope.volume.clear();
	refreshList();
}


void LibraryView::LibraryModel::navigateBack() {
	if(cur_scope.category == TITLE_SCOPE) {
		cur_scope.category = VOLUME_SCOPE;
		cur_scope.volume.clear();
	} else if(cur_scope.category == VOLUME_SCOPE) {
		cur_scope.category = SERIES_SCOPE;
		cur_scope.series.clear();
	} else if(cur_scope.category == SERIES_SCOPE && config->groupByPublisher()) {
		cur_scope.category = PUBLISHER_SCOPE;
		cur_scope.publisher.clear();
	}

	refreshList();
}


void LibraryView::LibraryModel::onItemActivated(const QModelIndex &index) {
	switch(cur_scope.category) {
		case PUBLISHER_SCOPE:
			cur_scope.publisher	=	list.at(index.row());
			cur_scope.category	=	SERIES_SCOPE;
			break;

		case SERIES_SCOPE:
			cur_scope.series	=	list.at(index.row());
			cur_scope.category	=	VOLUME_SCOPE;
			break;

		case VOLUME_SCOPE:
			cur_scope.volume	=	list.at(index.row());
			cur_scope.category	=	TITLE_SCOPE;
			break;

		case TITLE_SCOPE:
		case LIST_SCOPE:
			// TODO: OPEN COMIC TO VIEW
			qDebug() << "OPENING: " << library->at(list.at(index.row()).toLocal8Bit()).getPath();
			break;
	}

	refreshList();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARYMODEL PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


LibraryView::LibraryModel::LibraryModel(QObject *parent) : QAbstractListModel(parent) {
	parent = (LibraryView*)parent;
}


LibraryView::LibraryModel::~LibraryModel() {}


/**
 * Layers covers of first 3 comics in comic_list, and returns a QIcon.
 */
QIcon LibraryView::LibraryModel::layeredIcon(const QList<ComicFile> &comic_list) const {
	if(comic_list.isEmpty()) return QIcon();

	int offset = 10, square_size;
	int total_width = 0, total_height = 0;

	// First get base size
	if(comic_list.size() >= 3) {
		square_size = library_view->icon_size - (offset * 2);
	} else if(comic_list.size() >= 2) {
		square_size = library_view->icon_size - offset;
	} else square_size = library_view->icon_size;

	QImage image[3];

	// Next get images and exact total size
	for(int i = 0; i < 3 && i < comic_list.size(); i++) {
		image[i] = (
			QImage(comic_list[i].getThumbPath())
		).scaled(square_size, square_size, Qt::KeepAspectRatio);

		if(i == 0) {
			total_width = image[i].width();
			total_height = image[i].height();
		} else {
			int width_diff	=	(image[i].width() + offset) - image[i - 1].width();
			int height_diff	=	(image[i].height() + offset) - image[i - 1].height();
			if(width_diff > 0) total_width += width_diff;
			if(height_diff > 0) total_height += height_diff;
		}
	}

	QImage result(total_width, total_height, QImage::Format_RGB32);
	QPainter painter;
	painter.begin(&result);
	// Fill with proper background color, because transparency isn't working properly
	painter.fillRect(0, 0, total_width, total_height,
		library_view->palette().color(QPalette::Base));

	// Paint images together
	for(int i = 0; i < 3 && i < comic_list.size(); i++) {
		painter.drawImage(offset * i, offset * i, image[i]);
	}

	painter.end();
	return QIcon(QPixmap::fromImage(result));
}