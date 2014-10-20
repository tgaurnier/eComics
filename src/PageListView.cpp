#include "PageListView.hpp"

#include <QProgressDialog>

#include "Config.hpp"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PAGELISTVIEW PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PageListView::PageListView(ComicFile &comic, QWidget *parent) : QListView(parent), comic(comic) {
	setSelectionMode(SingleSelection);
	setViewMode(IconMode);
	setIconSize(QSize(128, 128));
	setSpacing(15);
}


PageListView::~PageListView() {
	// Clear temp dir
	if(initialized) {
		for(QString file_name : config->getTempDir().entryList()) {
			config->getTempDir().remove(file_name);
		}

		delete model;
	}
}


/**
 * Show progress bar and extract pages to temp dir. Returns true on complete, and false if canceled.
 * After pages are extracted the PageListModel is populated for pages to be displayed.
 */
bool PageListView::init() {
	const PageList &page_list = comic.info.page_list;
	QProgressDialog progress_dialog(tr("Extracting pages..."), tr("Cancel"), 0, page_list.size(),
		this);
	progress_dialog.setWindowModality(Qt::WindowModal);
	progress_dialog.setValue(0);

	int i = 0;
	for(const Page &page : page_list) {
		comic.extractPage(page.getImage().toInt(), config->getTempDir().absolutePath(),
			page.getImage() + ".jpg", 128);

		progress_dialog.setValue(++i);
		if(progress_dialog.wasCanceled()) break;
	}

	if(progress_dialog.wasCanceled()) {
		// If extraction was canceled, then delete any extracted image
		for(QString file_name : config->getTempDir().entryList()) {
			config->getTempDir().remove(file_name);
		}

		return false;
	} else {
		// Else populate model with page images
		model = new PageListModel(comic.info.page_list, this);
		setModel(model);
		initialized = true;

		 return true;
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PAGELISTMODEL PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PageListView::PageListModel::PageListModel(PageList &page_list, QObject *parent)
		: QAbstractListModel(parent), list(page_list) {}


QVariant PageListView::PageListModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid() || index.row() >= list.size()) return QVariant();

	if(role == Qt::DecorationRole) {
		return QIcon(config->getTempDir().absolutePath() + "/" +
			list.at(index.row()).getImage() + ".jpg");
	} else if(role == Qt::DisplayRole) {
		QString caption = QString("Image ") + list.at(index.row()).getImage() + ".";
		if(!list.at(index.row()).getType().isEmpty()) {
			caption.append(QString(" ") + list.at(index.row()).getType());
		}

		return caption;
	} else return QVariant();
}


int PageListView::PageListModel::rowCount(const QModelIndex &parent) const {
	return list.size();
}