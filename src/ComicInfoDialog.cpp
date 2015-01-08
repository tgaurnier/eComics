#include "ComicInfoDialog.hpp"

#include <QApplication>
#include <QComboBox>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressDialog>
#include <QScrollArea>
#include <QScrollBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

#include "Actions.hpp"
#include "ComicFile.hpp"
#include "Config.hpp"
#include "DelimitedCompleter.hpp"
#include "FixedGridLayout.hpp"
#include "Library.hpp"
#include "LibraryView.hpp"
#include "PageListView.hpp"


ComicInfoDialog *comic_info_dialog										=	nullptr;
ComicInfoDialog::MultipleInfoPage *ComicInfoDialog::multiple_info_page	=	nullptr;
ComicInfoDialog::PropertiesTab *ComicInfoDialog::properties_tab			=	nullptr;
ComicInfoDialog::InfoTab *ComicInfoDialog::info_tab						=	nullptr;
ComicInfoDialog::PlotTab *ComicInfoDialog::plot_tab						=	nullptr;
ComicInfoDialog::PagesTab *ComicInfoDialog::pages_tab					=	nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									COMICINFODIALOG PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfoDialog::init(QWidget *parent) {
	if(comic_info_dialog == nullptr) {
		comic_info_dialog = new ComicInfoDialog(parent);
	}
}


void ComicInfoDialog::destroy() {
	delete comic_info_dialog;
	comic_info_dialog = nullptr;
}


void ComicInfoDialog::onTabChanged(int index) {
	if(index == tab_widget->indexOf(pages_tab) && !finishing) pages_tab->onShow();
}


void ComicInfoDialog::show() {
	selected_list = library_view->getSelectedComics();
	if(selected_list.isEmpty()) {
		qDebug() << "No comics selected, Actions::info_action should be disabled!";
		return;
	} else mode = (selected_list.size() > 1) ? MULTIPLE : SINGLE;


	initView();
	exec();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								COMICINFODIALOG PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::ComicInfoDialog(QWidget *parent) : QDialog(parent) {
	// Init child objects
	v_box		=	new QVBoxLayout(this);
	button_box	=	new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	// Add button box to v_box
	v_box->addWidget(button_box);

	// Connect signals
	connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(const int)));
	connect(eComics::actions->info(), SIGNAL(triggered()), this, SLOT(show()));
}


ComicInfoDialog::~ComicInfoDialog() {
	delete v_box;
	delete button_box;
}


/**
 * Setup dialog for single or multiple comics.
 */
void ComicInfoDialog::initView() {
	finishing = false;

	switch(mode) {
		// When one comic is selected, dialog will have tabs
		case SINGLE:
			setWindowTitle(tr("Comic Info (") + selected_list[0].getFileName() + ")");

			// Init tabs
			tab_widget = new QTabWidget();
			PropertiesTab::init();
			InfoTab::init();
			PlotTab::init();
			PagesTab::init();

			// Add tabs to tab widget
			tab_widget->addTab(properties_tab, tr("Properties"));
			tab_widget->addTab(info_tab, tr("Info"));
			tab_widget->addTab(plot_tab, tr("Plot"));
			tab_widget->addTab(pages_tab, tr("Pages"));

			v_box->insertWidget(0, tab_widget);

			// Connect signals
			connect(tab_widget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));

			break;

		// When multiple comics selected, ONLY show info page (no tabs)
		case MULTIPLE:
			setWindowTitle(tr("Comic Info (") + QString::number(selected_list.size()) +
				tr(" books selected)"));
			MultipleInfoPage::init();
			v_box->insertWidget(0, multiple_info_page);
			this->setFixedSize(sizeHint());
			break;
	}
}


void ComicInfoDialog::onFinished(const int result) {
	finishing = true;

	switch(mode) {
		case SINGLE:
			if(result == Accepted) comic_info_dialog->selected_list[0].startEditing();
			info_tab->onFinished(result);
			plot_tab->onFinished(result);
			pages_tab->onFinished(result);
			if(result == Accepted) comic_info_dialog->selected_list[0].finishEditing();

			v_box->removeWidget(tab_widget);
			PropertiesTab::destroy();
			InfoTab::destroy();
			PlotTab::destroy();
			PagesTab::destroy();
			delete tab_widget; // tab_widget must be destroyed last to prevent a SegFault

			break;

		case MULTIPLE:
			multiple_info_page->onFinished(result);
			v_box->removeWidget(multiple_info_page);
			MultipleInfoPage::destroy();

			break;
	}

	library_view->refreshModel();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								MULTIPLEINFOPAGE PUBLIC METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfoDialog::MultipleInfoPage::init() {
	if(multiple_info_page == nullptr) {
		multiple_info_page = new MultipleInfoPage;
	}
}


void ComicInfoDialog::MultipleInfoPage::destroy() {
	delete multiple_info_page;
	multiple_info_page = nullptr;
}


void ComicInfoDialog::MultipleInfoPage::onFinished(const int result) {
	if(result == Accepted) {
		library->startBatchEditing();

		QProgressDialog progress_dialog(tr("Saving metadata to comic files..."), 0, 0,
		comic_info_dialog->selected_list.size(), comic_info_dialog);
		progress_dialog.setWindowModality(Qt::WindowModal);
		progress_dialog.setMinimumDuration(500);
		progress_dialog.setCancelButton(0);
		progress_dialog.show();
		progress_dialog.setValue(0);

		int i = 0;
		for(ComicFile &comic : comic_info_dialog->selected_list) {
			comic.startEditing();

			if(series_value->text() != "(multiple values)" &&
					series_value->text() != comic.info.getSeries()) {
				comic.info.setSeries(series_value->text());
			}

			if(count_value->text() != "(multiple values)" &&
					count_value->text() != comic.info.getCount()) {
				comic.info.setCount(count_value->text());
			}

			if(volume_value->text() != "(multiple values)" &&
					volume_value->text() != comic.info.getVolume()) {
				comic.info.setVolume(volume_value->text());
			}

			if(alt_series_value->text() != "(multiple values)" &&
					alt_series_value->text() != comic.info.getAltSeries()) {
				comic.info.setAltSeries(alt_series_value->text());
			}

			if(alt_count_value->text() != "(multiple values)" &&
					alt_count_value->text() != comic.info.getAltCount()) {
				comic.info.setAltCount(alt_count_value->text());
			}

			if(year_value->text() != "(multiple values)" &&
					year_value->text() != comic.info.getYear()) {
				comic.info.setYear(year_value->text());
			}

			if(month_value->text() != "(multiple values)" &&
					month_value->text() != comic.info.getMonth()) {
				comic.info.setMonth(month_value->text());
			}

			if(writer_value->text() != "(multiple values)" &&
					writer_value->text() != comic.info.getWriter()) {
				comic.info.setWriter(writer_value->text());
			}

			if(penciller_value->text() != "(multiple values)" &&
					penciller_value->text() != comic.info.getPenciller()) {
				comic.info.setPenciller(penciller_value->text());
			}

			if(inker_value->text() != "(multiple values)" &&
					inker_value->text() != comic.info.getInker()) {
				comic.info.setInker(inker_value->text());
			}

			if(colorist_value->text() != "(multiple values)" &&
					colorist_value->text() != comic.info.getColorist()) {
				comic.info.setColorist(colorist_value->text());
			}

			if(letterer_value->text() != "(multiple values)" &&
					letterer_value->text() != comic.info.getLetterer()) {
				comic.info.setLetterer(letterer_value->text());
			}

			if(cover_artist_value->text() != "(multiple values)" &&
					cover_artist_value->text() != comic.info.getCoverArtist()) {
				comic.info.setCoverArtist(cover_artist_value->text());
			}

			if(editor_value->text() != "(multiple values)" &&
					editor_value->text() != comic.info.getEditor()) {
				comic.info.setEditor(editor_value->text());
			}

			if(publisher_value->text() != "(multiple values)" &&
					publisher_value->text() != comic.info.getPublisher()) {
				comic.info.setPublisher(publisher_value->text());
			}

			if(imprint_value->text() != "(multiple values)" &&
					imprint_value->text() != comic.info.getImprint()) {
				comic.info.setImprint(imprint_value->text());
			}

			if(genre_value->text() != "(multiple values)" &&
					genre_value->text() != comic.info.getGenre()) {
				comic.info.setGenre(genre_value->text());
			}

			if(language_iso_value->text() != "(multiple values)" &&
					language_iso_value->text() != comic.info.getLanguageIso()) {
				comic.info.setLanguageIso(language_iso_value->text());
			}

			if(format_value->text() != "(multiple values)" &&
					format_value->text() != comic.info.getFormat()) {
				comic.info.setFormat(format_value->text());
			}

			if(black_and_white_value->currentText() != "(multiple values)" &&
					black_and_white_value->currentText() != comic.info.getBlackAndWhite()) {
				comic.info.setBlackAndWhite(black_and_white_value->currentText());
			}

			if(manga_value->currentText() != "(multiple values)" &&
					manga_value->currentText() != comic.info.getManga()) {
				comic.info.setManga(manga_value->currentText());
			}

			comic.finishEditing();
			progress_dialog.setValue(++i);
			QApplication::processEvents();
		}

		library->finishBatchEditing();
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								MULTIPLEINFOPAGE PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::MultipleInfoPage::MultipleInfoPage() {
	ReferenceList<ComicFile> comic_list = comic_info_dialog->selected_list;

	QFontMetrics metrics(QApplication::font());
	int col_width		=	metrics.width("00000000000");
	int row_height		=	metrics.height() * 1.5; // For some reason lineHeight() is too small
	int spacing			=	metrics.height() / 3;

	grid_layout			=	new FixedGridLayout(this, col_width, row_height);
	grid_layout->setSpacing(spacing);

	// Labels
	series_label			=	new QLabel("Series:");
	count_label				=	new QLabel("Number of:");
	volume_label			=	new QLabel("Volume:");
	alt_series_label		=	new QLabel("Alternate Series or Storyline Title:");
	alt_count_label			=	new QLabel("Alt Num of:");
	year_label				=	new QLabel("Year:");
	month_label				=	new QLabel("Month:");
	writer_label			=	new QLabel("Writer:");
	penciller_label			=	new QLabel("Penciller:");
	inker_label				=	new QLabel("Inker:");
	colorist_label			=	new QLabel("Colorist:");
	letterer_label			=	new QLabel("Letterer:");
	cover_artist_label		=	new QLabel("Cover Artist:");
	editor_label			=	new QLabel("Editor:");
	publisher_label			=	new QLabel("Publisher:");
	imprint_label			=	new QLabel("Imprint:");
	genre_label				=	new QLabel("Genre:");
	language_iso_label		=	new QLabel("Language ISO:");
	format_label			=	new QLabel("Format:");
	black_and_white_label	=	new QLabel("Black/White:");
	manga_label				=	new QLabel("Manga:");

	// Get strings to prefill the editable fields
	QString	series_str, count_str, volume_str, alt_series_str, alt_count_str, year_str, month_str,
			writer_str, penciller_str, inker_str, colorist_str, letterer_str, cover_artist_str,
			editor_str, publisher_str, imprint_str, genre_str, language_iso_str, format_str,
			black_and_white_str, manga_str;
	for(const ComicFile &comic : comic_list) {
		if(series_str.isEmpty()) series_str = comic.info.getSeries();
		else if(series_str == "(multiple values)" || series_str != comic.info.getSeries()) {
			series_str = "(multiple values)";
		}

		if(count_str.isEmpty()) count_str = comic.info.getCount();
		else if(count_str == "(multiple values)" || count_str != comic.info.getCount()) {
			count_str = "(multiple values)";
		}

		if(volume_str.isEmpty()) volume_str = comic.info.getVolume();
		else if(volume_str == "(multiple values)" || volume_str != comic.info.getVolume()) {
			volume_str = "(multiple values)";
		}

		if(alt_series_str.isEmpty()) alt_series_str = comic.info.getAltSeries();
		else if(alt_series_str == "(multiple values)" ||
				alt_series_str != comic.info.getAltSeries()) {
			alt_series_str = "(multiple values)";
		}

		if(alt_count_str.isEmpty()) alt_count_str = comic.info.getAltCount();
		else if(alt_count_str == "(multiple values)" || alt_count_str != comic.info.getAltCount()) {
			alt_count_str = "(multiple values)";
		}

		if(year_str.isEmpty()) year_str = comic.info.getYear();
		else if(year_str == "(multiple values)" || year_str != comic.info.getYear()) {
			year_str = "(multiple values)";
		}

		if(month_str.isEmpty()) month_str = comic.info.getMonth();
		else if(month_str == "(multiple values)" || month_str != comic.info.getMonth()) {
			month_str = "(multiple values)";
		}

		if(writer_str.isEmpty()) writer_str = comic.info.getWriter();
		else if(writer_str == "(multiple values)" || writer_str != comic.info.getWriter()) {
			writer_str = "(multiple values)";
		}

		if(penciller_str.isEmpty()) penciller_str = comic.info.getPenciller();
		else if(penciller_str == "(multiple values)" ||
				penciller_str != comic.info.getPenciller()) {
			penciller_str = "(multiple values)";
		}

		if(inker_str.isEmpty()) inker_str = comic.info.getInker();
		else if(inker_str == "(multiple values)" || inker_str != comic.info.getInker()) {
			inker_str = "(multiple values)";
		}

		if(colorist_str.isEmpty()) colorist_str = comic.info.getColorist();
		else if(colorist_str == "(multiple values)" || colorist_str != comic.info.getColorist()) {
			colorist_str = "(multiple values)";
		}

		if(letterer_str.isEmpty()) letterer_str = comic.info.getLetterer();
		else if(letterer_str == "(multiple values)" || letterer_str != comic.info.getLetterer()) {
			letterer_str = "(multiple values)";
		}

		if(cover_artist_str.isEmpty()) cover_artist_str = comic.info.getCoverArtist();
		else if(cover_artist_str == "(multiple values)" ||
				cover_artist_str != comic.info.getCoverArtist()) {
			cover_artist_str = "(multiple values)";
		}

		if(editor_str.isEmpty()) editor_str = comic.info.getEditor();
		else if(editor_str == "(multiple values)" || editor_str != comic.info.getEditor()) {
			editor_str = "(multiple values)";
		}

		if(publisher_str.isEmpty()) publisher_str = comic.info.getPublisher();
		else if(publisher_str == "(multiple values)" ||
				publisher_str != comic.info.getPublisher()) {
			publisher_str = "(multiple values)";
		}

		if(imprint_str.isEmpty()) imprint_str = comic.info.getImprint();
		else if(imprint_str == "(multiple values)" || imprint_str != comic.info.getImprint()) {
			imprint_str = "(multiple values)";
		}

		if(genre_str.isEmpty()) genre_str = comic.info.getGenre();
		else if(genre_str == "(multiple values)" || genre_str != comic.info.getGenre()) {
			genre_str = "(multiple values)";
		}

		if(language_iso_str.isEmpty()) language_iso_str = comic.info.getLanguageIso();
		else if(language_iso_str == "(multiple values)" ||
				language_iso_str != comic.info.getLanguageIso()) {
			language_iso_str = "(multiple values)";
		}

		if(format_str.isEmpty()) format_str = comic.info.getFormat();
		else if(format_str == "(multiple values)" || format_str != comic.info.getFormat()) {
			format_str = "(multiple values)";
		}

		if(black_and_white_str.isEmpty()) black_and_white_str = comic.info.getBlackAndWhite();
		else if(black_and_white_str == "(multiple values)" ||
				black_and_white_str != comic.info.getBlackAndWhite()) {
			black_and_white_str = "(multiple values)";
		}

		if(manga_str.isEmpty()) manga_str = comic.info.getManga();
		else if(manga_str == "(multiple values)" || manga_str != comic.info.getManga()) {
			manga_str = "(multiple values)";
		}
	}

	// Editable value fields
	series_value			=	new QLineEdit(series_str);
	count_value				=	new QLineEdit(count_str);
	volume_value			=	new QLineEdit(volume_str);
	alt_series_value		=	new QLineEdit(alt_series_str);
	alt_count_value			=	new QLineEdit(alt_count_str);
	year_value				=	new QLineEdit(year_str);
	month_value				=	new QLineEdit(month_str);
	writer_value			=	new QLineEdit(writer_str);
	penciller_value			=	new QLineEdit(penciller_str);
	inker_value				=	new QLineEdit(inker_str);
	colorist_value			=	new QLineEdit(colorist_str);
	letterer_value			=	new QLineEdit(letterer_str);
	cover_artist_value		=	new QLineEdit(cover_artist_str);
	editor_value			=	new QLineEdit(editor_str);
	publisher_value			=	new QLineEdit(publisher_str);
	imprint_value			=	new QLineEdit(imprint_str);
	genre_value				=	new QLineEdit(genre_str);
	language_iso_value		=	new QLineEdit(language_iso_str);
	format_value			=	new QLineEdit(format_str);
	black_and_white_value	=	new QComboBox();
	manga_value				=	new QComboBox();

	// Setup completers
	series_completer		=	new	QCompleter(library->series_list);
	alt_series_completer	=	new	QCompleter(library->alt_series_list);
	writer_completer		=	new	QCompleter(library->writer_list);
	penciller_completer		=	new	QCompleter(library->penciller_list);
	inker_completer			=	new	QCompleter(library->inker_list);
	colorist_completer		=	new	QCompleter(library->colorist_list);
	letterer_completer		=	new	QCompleter(library->letterer_list);
	cover_artist_completer	=	new	QCompleter(library->cover_artist_list);
	editor_completer		=	new	QCompleter(library->editor_list);
	publisher_completer		=	new	QCompleter(library->publisher_list);
	imprint_completer		=	new	QCompleter(library->imprint_list);
	genre_completer			=	new	DelimitedCompleter(genre_value, ',', library->genre_list);
	language_iso_completer	=	new	QCompleter(library->language_iso_list);
	format_completer		=	new	QCompleter(library->format_list);

	// Create spacer widgets
	v_spacer1 = new QWidget;
	v_spacer2 = new QWidget;

	// Set completers case sensitivity
	series_completer->setCaseSensitivity(Qt::CaseInsensitive);
	alt_series_completer->setCaseSensitivity(Qt::CaseInsensitive);
	writer_completer->setCaseSensitivity(Qt::CaseInsensitive);
	penciller_completer->setCaseSensitivity(Qt::CaseInsensitive);
	inker_completer->setCaseSensitivity(Qt::CaseInsensitive);
	colorist_completer->setCaseSensitivity(Qt::CaseInsensitive);
	letterer_completer->setCaseSensitivity(Qt::CaseInsensitive);
	cover_artist_completer->setCaseSensitivity(Qt::CaseInsensitive);
	editor_completer->setCaseSensitivity(Qt::CaseInsensitive);
	publisher_completer->setCaseSensitivity(Qt::CaseInsensitive);
	imprint_completer->setCaseSensitivity(Qt::CaseInsensitive);
	genre_completer->setCaseSensitivity(Qt::CaseInsensitive);
	language_iso_completer->setCaseSensitivity(Qt::CaseInsensitive);
	format_completer->setCaseSensitivity(Qt::CaseInsensitive);

	// Add completers to line edit fields
	series_value->setCompleter(series_completer);
	alt_series_value->setCompleter(alt_series_completer);
	writer_value->setCompleter(writer_completer);
	penciller_value->setCompleter(penciller_completer);
	inker_value->setCompleter(inker_completer);
	colorist_value->setCompleter(colorist_completer);
	letterer_value->setCompleter(letterer_completer);
	cover_artist_value->setCompleter(cover_artist_completer);
	editor_value->setCompleter(editor_completer);
	publisher_value->setCompleter(publisher_completer);
	imprint_value->setCompleter(imprint_completer);
	language_iso_value->setCompleter(language_iso_completer);
	format_value->setCompleter(format_completer);

	// Setup combo boxes
	if(black_and_white_str == "(multiple values)") {
		black_and_white_value->addItems( { "(multiple values)" } );
	}
	black_and_white_value->addItems( {"No", "Yes"} );
	black_and_white_value->setCurrentText(black_and_white_str);
	if(manga_str == "(multiple values)") {
		manga_value->addItems( { "(multiple values)" } );
	}
	manga_value->addItems( {"No", "Yes"} );
	manga_value->setCurrentText(manga_str);

	// Primary Info
	// Rows 0 and 1
	grid_layout->addWidget(series_label, 0, 0, 1, 3);
	grid_layout->addWidget(series_value, 1, 0, 1, 3);
	grid_layout->addWidget(volume_label, 0, 3, 1, 1);
	grid_layout->addWidget(volume_value, 1, 3, 1, 1);
	grid_layout->addWidget(count_label, 0, 4, 1, 1);
	grid_layout->addWidget(count_value, 1, 4, 1, 1);
	grid_layout->addWidget(month_label, 0, 5, 1, 1);
	grid_layout->addWidget(month_value, 1, 5, 1, 1);

	// Rows 2 and 3
	grid_layout->addWidget(alt_series_label, 2, 0, 1, 4);
	grid_layout->addWidget(alt_series_value, 3, 0, 1, 4);
	grid_layout->addWidget(alt_count_label, 2, 4, 1, 1);
	grid_layout->addWidget(alt_count_value, 3, 4, 1, 1);
	grid_layout->addWidget(year_label, 2, 5, 1, 1);
	grid_layout->addWidget(year_value, 3, 5, 1, 1);

	// Add space to row 4
	grid_layout->addWidget(v_spacer1, 4, 0, 1, 6);

	// Creator Info
	// Rows 5 and 6
	grid_layout->addWidget(publisher_label, 5, 0, 1, 3);
	grid_layout->addWidget(publisher_value, 6, 0, 1, 3);
	grid_layout->addWidget(writer_label, 5, 3, 1, 3);
	grid_layout->addWidget(writer_value, 6, 3, 1, 3);

	// Rows 7 and 8
	grid_layout->addWidget(penciller_label, 7, 0, 1, 3);
	grid_layout->addWidget(penciller_value, 8, 0, 1, 3);
	grid_layout->addWidget(inker_label, 7, 3, 1, 3);
	grid_layout->addWidget(inker_value, 8, 3, 1, 3);

	// Rows 9 and 10
	grid_layout->addWidget(colorist_label, 9, 0, 1, 3);
	grid_layout->addWidget(colorist_value, 10, 0, 1, 3);
	grid_layout->addWidget(letterer_label, 9, 3, 1, 3);
	grid_layout->addWidget(letterer_value, 10, 3, 1, 3);

	// Rows 11 and 12
	grid_layout->addWidget(cover_artist_label, 11, 0, 1, 3);
	grid_layout->addWidget(cover_artist_value, 12, 0, 1, 3);
	grid_layout->addWidget(editor_label, 11, 3, 1, 3);
	grid_layout->addWidget(editor_value, 12, 3, 1, 3);

	// Add space to row 13
	grid_layout->addWidget(v_spacer2, 13, 0, 1, 6);

	// Misc info
	// Rows 14 and 15
	grid_layout->addWidget(format_label, 14, 0, 1, 2);
	grid_layout->addWidget(format_value, 15, 0, 1, 2);
	grid_layout->addWidget(imprint_label, 14, 2, 1, 2);
	grid_layout->addWidget(imprint_value, 15, 2, 1, 2);
	grid_layout->addWidget(language_iso_label, 14, 4, 1, 2);
	grid_layout->addWidget(language_iso_value, 15, 4, 1, 2);

	// Rows 16 and 17
	grid_layout->addWidget(manga_label, 16, 0, 1, 1);
	grid_layout->addWidget(manga_value, 17, 0, 1, 1);
	grid_layout->addWidget(black_and_white_label, 16, 1, 1, 1);
	grid_layout->addWidget(black_and_white_value, 17, 1, 1, 1);
	grid_layout->addWidget(genre_label, 16, 2, 1, 4);
	grid_layout->addWidget(genre_value, 17, 2, 1, 4);

	setMinimumSize(sizeHint());
}


ComicInfoDialog::MultipleInfoPage::~MultipleInfoPage() {
	delete series_completer;
	delete alt_series_completer;
	delete writer_completer;
	delete penciller_completer;
	delete inker_completer;
	delete colorist_completer;
	delete letterer_completer;
	delete cover_artist_completer;
	delete editor_completer;
	delete publisher_completer;
	delete imprint_completer;
	delete genre_completer;
	delete language_iso_completer;
	delete format_completer;
	delete series_label;
	delete count_label;
	delete volume_label;
	delete alt_series_label;
	delete alt_count_label;
	delete year_label;
	delete month_label;
	delete writer_label;
	delete penciller_label;
	delete inker_label;
	delete colorist_label;
	delete letterer_label;
	delete cover_artist_label;
	delete editor_label;
	delete publisher_label;
	delete imprint_label;
	delete genre_label;
	delete language_iso_label;
	delete format_label;
	delete black_and_white_label;
	delete manga_label;
	delete series_value;
	delete count_value;
	delete volume_value;
	delete alt_series_value;
	delete alt_count_value;
	delete year_value;
	delete month_value;
	delete writer_value;
	delete penciller_value;
	delete inker_value;
	delete colorist_value;
	delete letterer_value;
	delete cover_artist_value;
	delete editor_value;
	delete publisher_value;
	delete imprint_value;
	delete genre_value;
	delete language_iso_value;
	delete format_value;
	delete black_and_white_value;
	delete manga_value;
	delete v_spacer1;
	delete v_spacer2;
	delete grid_layout;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PROPERTIESTAB PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfoDialog::PropertiesTab::init(QWidget *parent) {
	if(properties_tab == nullptr) {
		properties_tab = new PropertiesTab(parent);
	}
}


void ComicInfoDialog::PropertiesTab::destroy() {
	delete properties_tab;
	properties_tab = nullptr;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PROPERTIESTAB PRIVATE METHODS 								 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::PropertiesTab::PropertiesTab(QWidget *parent) : QWidget(parent) {
	ComicFile &comic	=	comic_info_dialog->selected_list[0];
	h_box				=	new QHBoxLayout(this);
	v_box				=	new QVBoxLayout();

	// Cover
	cover = new QLabel();
	cover->setPixmap(QPixmap(comic.getThumbPath()).scaled(384, 384, Qt::KeepAspectRatio));

	// Properties
	type_label	=	new QLabel(QString("<b>File Type:</b> ") + comic.getExtString());
	size_label	=	new QLabel(QString("<b>File Size:</b> ") + comic.getSizeString());
	pages_label	=	new QLabel(QString("<b>Number of Pages:</b> ") + comic.getNumOfPagesString());
	path_label	=	new QLabel(QString("<b>Location:</b> ") + comic.getPath());
	path_label->setWordWrap(true);
	path_label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

	// Add items to h_box
	h_box->addWidget(cover);
	h_box->addSpacing(15);
	h_box->addLayout(v_box);
	h_box->addStretch(200);

	// Add items to v_box
	v_box->addWidget(type_label);
	v_box->addWidget(size_label);
	v_box->addWidget(pages_label);
	v_box->addWidget(path_label);
	v_box->addStretch(cover->height());
}


ComicInfoDialog::PropertiesTab::~PropertiesTab() {
	delete cover;
	delete type_label;
	delete size_label;
	delete pages_label;
	delete path_label;
	delete v_box;
	delete h_box;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										INFOTAB PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfoDialog::InfoTab::init(QWidget *parent) {
	if(info_tab == nullptr) {
		info_tab = new InfoTab(parent);
	}
}


void ComicInfoDialog::InfoTab::destroy() {
	delete info_tab;
	info_tab = nullptr;
}


/**
 * Save any changed values to comic.
 */
void ComicInfoDialog::InfoTab::onFinished(const int result) {
	if(result == Accepted) {
		ComicFile &comic = comic_info_dialog->selected_list[0];

		if(title_value->text() != comic.info.getTitle()) {
			comic.info.setTitle(title_value->text());
		}

		if(series_value->text() != comic.info.getSeries()) {
			comic.info.setSeries(series_value->text());
		}

		if(number_value->text() != comic.info.getNumber()) {
			comic.info.setNumber(number_value->text());
		}

		if(count_value->text() != comic.info.getCount()) {
			comic.info.setCount(count_value->text());
		}

		if(volume_value->text() != comic.info.getVolume()) {
			comic.info.setVolume(volume_value->text());
		}

		if(alt_series_value->text() != comic.info.getAltSeries()) {
			comic.info.setAltSeries(alt_series_value->text());
		}

		if(alt_number_value->text() != comic.info.getAltNumber()) {
			comic.info.setAltNumber(alt_number_value->text());
		}

		if(alt_count_value->text() != comic.info.getAltCount()) {
			comic.info.setAltCount(alt_count_value->text());
		}

		if(year_value->text() != comic.info.getYear()) {
			comic.info.setYear(year_value->text());
		}

		if(month_value->text() != comic.info.getMonth()) {
			comic.info.setMonth(month_value->text());
		}

		if(writer_value->text() != comic.info.getWriter()) {
			comic.info.setWriter(writer_value->text());
		}

		if(penciller_value->text() != comic.info.getPenciller()) {
			comic.info.setPenciller(penciller_value->text());
		}

		if(inker_value->text() != comic.info.getInker()) {
			comic.info.setInker(inker_value->text());
		}

		if(colorist_value->text() != comic.info.getColorist()) {
			comic.info.setColorist(colorist_value->text());
		}

		if(letterer_value->text() != comic.info.getLetterer()) {
			comic.info.setLetterer(letterer_value->text());
		}

		if(cover_artist_value->text() != comic.info.getCoverArtist()) {
			comic.info.setCoverArtist(cover_artist_value->text());
		}

		if(editor_value->text() != comic.info.getEditor()) {
			comic.info.setEditor(editor_value->text());
		}

		if(publisher_value->text() != comic.info.getPublisher()) {
			comic.info.setPublisher(publisher_value->text());
		}

		if(imprint_value->text() != comic.info.getImprint()) {
			comic.info.setImprint(imprint_value->text());
		}

		if(genre_value->text() != comic.info.getGenre()) {
			comic.info.setGenre(genre_value->text());
		}

		if(language_iso_value->text() != comic.info.getLanguageIso()) {
			comic.info.setLanguageIso(language_iso_value->text());
		}

		if(format_value->text() != comic.info.getFormat()) {
			comic.info.setFormat(format_value->text());
		}

		if(black_and_white_value->currentText() != comic.info.getBlackAndWhite()) {
			comic.info.setBlackAndWhite(black_and_white_value->currentText());
		}

		if(manga_value->currentText() != comic.info.getManga()) {
			comic.info.setManga(manga_value->currentText());
		}
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									INFOTAB PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::InfoTab::InfoTab(QWidget *parent) : QScrollArea(parent) {
	ComicFile &comic	=	comic_info_dialog->selected_list[0];

	QFontMetrics metrics(QApplication::font());
	int col_width		=	metrics.width("00000000000");
	int row_height		=	metrics.height() * 1.5; // For some reason lineHeight() is too small
	int spacing			=	metrics.height() / 3;

	grid_view			=	new QWidget();
	grid_layout			=	new FixedGridLayout(grid_view, col_width, row_height);
	grid_layout->setSpacing(spacing);
	setWidget(grid_view);

	// Labels
	title_label				=	new QLabel("Title:");
	series_label			=	new QLabel("Series:");
	number_label			=	new QLabel("No:");
	count_label				=	new QLabel("of:");
	volume_label			=	new QLabel("Volume:");
	alt_series_label		=	new QLabel("Alternate Series or Storyline Title:");
	alt_number_label		=	new QLabel("Number:");
	alt_count_label			=	new QLabel("of:");
	year_label				=	new QLabel("Year:");
	month_label				=	new QLabel("Month:");
	writer_label			=	new QLabel("Writer:");
	penciller_label			=	new QLabel("Penciller:");
	inker_label				=	new QLabel("Inker:");
	colorist_label			=	new QLabel("Colorist:");
	letterer_label			=	new QLabel("Letterer:");
	cover_artist_label		=	new QLabel("Cover Artist:");
	editor_label			=	new QLabel("Editor:");
	publisher_label			=	new QLabel("Publisher:");
	imprint_label			=	new QLabel("Imprint:");
	genre_label				=	new QLabel("Genre:");
	language_iso_label		=	new QLabel("Language ISO:");
	format_label			=	new QLabel("Format:");
	black_and_white_label	=	new QLabel("Black/White:");
	manga_label				=	new QLabel("Manga:");

	// Editable value fields
	title_value				=	new QLineEdit(comic.info.getTitle());
	series_value			=	new QLineEdit(comic.info.getSeries());
	number_value			=	new QLineEdit(comic.info.getNumber());
	count_value				=	new QLineEdit(comic.info.getCount());
	volume_value			=	new QLineEdit(comic.info.getVolume());
	alt_series_value		=	new QLineEdit(comic.info.getAltSeries());
	alt_number_value		=	new QLineEdit(comic.info.getAltNumber());
	alt_count_value			=	new QLineEdit(comic.info.getAltCount());
	year_value				=	new QLineEdit(comic.info.getYear());
	month_value				=	new QLineEdit(comic.info.getMonth());
	writer_value			=	new QLineEdit(comic.info.getWriter());
	penciller_value			=	new QLineEdit(comic.info.getPenciller());
	inker_value				=	new QLineEdit(comic.info.getInker());
	colorist_value			=	new QLineEdit(comic.info.getColorist());
	letterer_value			=	new QLineEdit(comic.info.getLetterer());
	cover_artist_value		=	new QLineEdit(comic.info.getCoverArtist());
	editor_value			=	new QLineEdit(comic.info.getEditor());
	publisher_value			=	new QLineEdit(comic.info.getPublisher());
	imprint_value			=	new QLineEdit(comic.info.getImprint());
	genre_value				=	new QLineEdit(comic.info.getGenre());
	language_iso_value		=	new QLineEdit(comic.info.getLanguageIso());
	format_value			=	new QLineEdit(comic.info.getFormat());
	black_and_white_value	=	new QComboBox();
	manga_value				=	new QComboBox();

	// Setup completers
	series_completer		=	new	QCompleter(library->series_list);
	alt_series_completer	=	new	QCompleter(library->alt_series_list);
	writer_completer		=	new	QCompleter(library->writer_list);
	penciller_completer		=	new	QCompleter(library->penciller_list);
	inker_completer			=	new	QCompleter(library->inker_list);
	colorist_completer		=	new	QCompleter(library->colorist_list);
	letterer_completer		=	new	QCompleter(library->letterer_list);
	cover_artist_completer	=	new	QCompleter(library->cover_artist_list);
	editor_completer		=	new	QCompleter(library->editor_list);
	publisher_completer		=	new	QCompleter(library->publisher_list);
	imprint_completer		=	new	QCompleter(library->imprint_list);
	genre_completer			=	new	DelimitedCompleter(genre_value, ',', library->genre_list);
	language_iso_completer	=	new	QCompleter(library->language_iso_list);
	format_completer		=	new	QCompleter(library->format_list);

	// Create spacer widgets
	v_spacer1 = new QWidget;
	v_spacer2 = new QWidget;

	// Set completers case sensitivity
	series_completer->setCaseSensitivity(Qt::CaseInsensitive);
	alt_series_completer->setCaseSensitivity(Qt::CaseInsensitive);
	writer_completer->setCaseSensitivity(Qt::CaseInsensitive);
	penciller_completer->setCaseSensitivity(Qt::CaseInsensitive);
	inker_completer->setCaseSensitivity(Qt::CaseInsensitive);
	colorist_completer->setCaseSensitivity(Qt::CaseInsensitive);
	letterer_completer->setCaseSensitivity(Qt::CaseInsensitive);
	cover_artist_completer->setCaseSensitivity(Qt::CaseInsensitive);
	editor_completer->setCaseSensitivity(Qt::CaseInsensitive);
	publisher_completer->setCaseSensitivity(Qt::CaseInsensitive);
	imprint_completer->setCaseSensitivity(Qt::CaseInsensitive);
	genre_completer->setCaseSensitivity(Qt::CaseInsensitive);
	language_iso_completer->setCaseSensitivity(Qt::CaseInsensitive);
	format_completer->setCaseSensitivity(Qt::CaseInsensitive);

	// Add completers to line edit fields
	series_value->setCompleter(series_completer);
	alt_series_value->setCompleter(alt_series_completer);
	writer_value->setCompleter(writer_completer);
	penciller_value->setCompleter(penciller_completer);
	inker_value->setCompleter(inker_completer);
	colorist_value->setCompleter(colorist_completer);
	letterer_value->setCompleter(letterer_completer);
	cover_artist_value->setCompleter(cover_artist_completer);
	editor_value->setCompleter(editor_completer);
	publisher_value->setCompleter(publisher_completer);
	imprint_value->setCompleter(imprint_completer);
	language_iso_value->setCompleter(language_iso_completer);
	format_value->setCompleter(format_completer);

	// Setup combo boxes
	black_and_white_value->addItems( {"No", "Yes"} );
	black_and_white_value->setCurrentText(comic.info.getBlackAndWhite());
	manga_value->addItems( {"No", "Yes"} );
	manga_value->setCurrentText(comic.info.getManga());

	// Primary Info
	// Rows 0 and 1
	grid_layout->addWidget(series_label, 0, 0, 1, 3);
	grid_layout->addWidget(series_value, 1, 0, 1, 3);
	grid_layout->addWidget(volume_label, 0, 3, 1, 1);
	grid_layout->addWidget(volume_value, 1, 3, 1, 1);
	grid_layout->addWidget(number_label, 0, 4, 1, 1);
	grid_layout->addWidget(number_value, 1, 4, 1, 1);
	grid_layout->addWidget(count_label, 0, 5, 1, 1);
	grid_layout->addWidget(count_value, 1, 5, 1, 1);

	// Rows 2 and 3
	grid_layout->addWidget(title_label, 2, 0, 1, 4);
	grid_layout->addWidget(title_value, 3, 0, 1, 4);
	grid_layout->addWidget(month_label, 2, 4, 1, 1);
	grid_layout->addWidget(month_value, 3, 4, 1, 1);
	grid_layout->addWidget(year_label, 2, 5, 1, 1);
	grid_layout->addWidget(year_value, 3, 5, 1, 1);

	// Rows 4 and 5
	grid_layout->addWidget(alt_series_label, 4, 0, 1, 4);
	grid_layout->addWidget(alt_series_value, 5, 0, 1, 4);
	grid_layout->addWidget(alt_number_label, 4, 4, 1, 1);
	grid_layout->addWidget(alt_number_value, 5, 4, 1, 1);
	grid_layout->addWidget(alt_count_label, 4, 5, 1, 1);
	grid_layout->addWidget(alt_count_value, 5, 5, 1, 1);

	// Add space to row 6
	grid_layout->addWidget(v_spacer1, 6, 0, 1, 6);

	// Creator Info
	// Rows 7 and 8
	grid_layout->addWidget(publisher_label, 7, 0, 1, 3);
	grid_layout->addWidget(publisher_value, 8, 0, 1, 3);
	grid_layout->addWidget(writer_label, 7, 3, 1, 3);
	grid_layout->addWidget(writer_value, 8, 3, 1, 3);

	// Rows 9 and 10
	grid_layout->addWidget(penciller_label, 9, 0, 1, 3);
	grid_layout->addWidget(penciller_value, 10, 0, 1, 3);
	grid_layout->addWidget(inker_label, 9, 3, 1, 3);
	grid_layout->addWidget(inker_value, 10, 3, 1, 3);

	// Rows 11 and 12
	grid_layout->addWidget(colorist_label, 11, 0, 1, 3);
	grid_layout->addWidget(colorist_value, 12, 0, 1, 3);
	grid_layout->addWidget(letterer_label, 11, 3, 1, 3);
	grid_layout->addWidget(letterer_value, 12, 3, 1, 3);

	// Rows 13 and 14
	grid_layout->addWidget(cover_artist_label, 13, 0, 1, 3);
	grid_layout->addWidget(cover_artist_value, 14, 0, 1, 3);
	grid_layout->addWidget(editor_label, 13, 3, 1, 3);
	grid_layout->addWidget(editor_value, 14, 3, 1, 3);

	// Add space to row 15
	grid_layout->addWidget(v_spacer2, 15, 0, 1, 6);

	// Misc info
	// Rows 16 and 17
	grid_layout->addWidget(format_label, 16, 0, 1, 2);
	grid_layout->addWidget(format_value, 17, 0, 1, 2);
	grid_layout->addWidget(imprint_label, 16, 2, 1, 2);
	grid_layout->addWidget(imprint_value, 17, 2, 1, 2);
	grid_layout->addWidget(language_iso_label, 16, 4, 1, 2);
	grid_layout->addWidget(language_iso_value, 17, 4, 1, 2);

	// Rows 18 and 19
	grid_layout->addWidget(manga_label, 18, 0, 1, 1);
	grid_layout->addWidget(manga_value, 19, 0, 1, 1);
	grid_layout->addWidget(black_and_white_label, 18, 1, 1, 1);
	grid_layout->addWidget(black_and_white_value, 19, 1, 1, 1);
	grid_layout->addWidget(genre_label, 18, 2, 1, 4);
	grid_layout->addWidget(genre_value, 19, 2, 1, 4);

	grid_view->setMinimumSize(grid_view->sizeHint());
	setMinimumWidth(grid_view->width() + verticalScrollBar()->sizeHint().width() + spacing);
}


ComicInfoDialog::InfoTab::~InfoTab() {
	delete series_completer;
	delete alt_series_completer;
	delete writer_completer;
	delete penciller_completer;
	delete inker_completer;
	delete colorist_completer;
	delete letterer_completer;
	delete cover_artist_completer;
	delete editor_completer;
	delete publisher_completer;
	delete imprint_completer;
	delete genre_completer;
	delete language_iso_completer;
	delete format_completer;
	delete title_label;
	delete series_label;
	delete number_label;
	delete count_label;
	delete volume_label;
	delete alt_series_label;
	delete alt_number_label;
	delete alt_count_label;
	delete year_label;
	delete month_label;
	delete writer_label;
	delete penciller_label;
	delete inker_label;
	delete colorist_label;
	delete letterer_label;
	delete cover_artist_label;
	delete editor_label;
	delete publisher_label;
	delete imprint_label;
	delete genre_label;
	delete language_iso_label;
	delete format_label;
	delete black_and_white_label;
	delete manga_label;
	delete title_value;
	delete series_value;
	delete number_value;
	delete count_value;
	delete volume_value;
	delete alt_series_value;
	delete alt_number_value;
	delete alt_count_value;
	delete year_value;
	delete month_value;
	delete writer_value;
	delete penciller_value;
	delete inker_value;
	delete colorist_value;
	delete letterer_value;
	delete cover_artist_value;
	delete editor_value;
	delete publisher_value;
	delete imprint_value;
	delete genre_value;
	delete language_iso_value;
	delete format_value;
	delete black_and_white_value;
	delete manga_value;
	delete v_spacer1;
	delete v_spacer2;
	delete grid_layout;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										PLOTTAB PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ComicInfoDialog::PlotTab::init(QWidget *parent) {
	if(plot_tab == nullptr) {
		plot_tab = new PlotTab(parent);
	}
}


void ComicInfoDialog::PlotTab::destroy() {
	delete plot_tab;
	plot_tab = nullptr;
}


void ComicInfoDialog::PlotTab::onFinished(const int result) {
	if(result == Accepted) {
		ComicFile &comic = comic_info_dialog->selected_list[0];

		if(summary_value->toPlainText() != comic.info.getSummary()) {
			comic.info.setSummary(summary_value->toPlainText());
		}

		if(notes_value->toPlainText() != comic.info.getNotes()) {
			comic.info.setNotes(notes_value->toPlainText());
		}

		if(web_value->text() != comic.info.getWeb()) {
			comic.info.setWeb(web_value->text());
		}
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PLOTTAB PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::PlotTab::PlotTab(QWidget *parent) : QWidget(parent) {
	ComicFile &comic	=	comic_info_dialog->selected_list[0];
	v_box				=	new QVBoxLayout(this);

	summary_label	=	new QLabel("Summary");
	notes_label		=	new QLabel("Notes");
	web_label		=	new QLabel("Web");

	summary_value	=	new QTextEdit(comic.info.getSummary());
	notes_value		=	new QTextEdit(comic.info.getNotes());
	web_value		=	new QLineEdit(comic.info.getWeb());

	v_box->addWidget(summary_label);
	v_box->addWidget(summary_value);
	v_box->addWidget(notes_label);
	v_box->addWidget(notes_value);
	v_box->addWidget(web_label);
	v_box->addWidget(web_value);

}


ComicInfoDialog::PlotTab::~PlotTab() {
	delete v_box;

	delete summary_label;
	delete notes_label;
	delete web_label;

	delete summary_value;
	delete notes_value;
	delete web_value;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PAGESTAB PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfoDialog::PagesTab::init(QWidget *parent) {
	if(pages_tab == nullptr) {
		pages_tab = new PagesTab(parent);
	}
}


void ComicInfoDialog::PagesTab::destroy() {
	delete pages_tab;
	pages_tab = nullptr;
}


void ComicInfoDialog::PagesTab::onFinished(const int result) {
	if(result == Accepted) {
		//TODO: SAVE PAGE CHANGES TO COMIC OBJECT
	}
}


void ComicInfoDialog::PagesTab::onShow() {
	if(!pages_extracted) {
		if(page_list_view->init()) pages_extracted = true;
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PAGESTAB PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


ComicInfoDialog::PagesTab::PagesTab(QWidget *parent) : QWidget(parent) {
	//TODO: ALLOW CHANGING PAGE DATA AND ORDER
	ComicFile &comic = comic_info_dialog->selected_list[0];

	v_box			=	new QVBoxLayout(this);
	page_list_view	=	new PageListView(comic, this);

	v_box->addWidget(page_list_view);
}


ComicInfoDialog::PagesTab::~PagesTab() {
	delete v_box;
	delete page_list_view;
}