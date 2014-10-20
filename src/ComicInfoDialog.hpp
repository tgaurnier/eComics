/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ComicInfoDialog.hpp                                                               *
 *                                                                             *
 * Copyright 2014 Tory Gaurnier <tory.gaurnier@linuxmail.org>                  *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published by *
 * the Free Software Foundation; version 3.                                    *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef COMICINFODIALOG_HPP
#define COMICINFODIALOG_HPP


#include <QDialog>
#include <QScrollArea>

#include "ReferenceList.hpp"


class QComboBox;
class QCompleter;
class QDialogButtonBox;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QTabWidget;
class QTextEdit;
class QVBoxLayout;

class ComicFile;
class DelimitedCompleter;
class FixedGridLayout;
class PageListView;


/**
 * class ComicInfoDialog
 *
 * A dialog used to edit comic info metadata. Can edit metadata of a single, or multiple comics,
 * depending on how many are selected in LibraryView. This is a singleton object, since there should
 * only ever be one ComicInfoDialog.
 */
class ComicInfoDialog : public QDialog {
	Q_OBJECT

	public:
		static void init(QWidget *parent);
		static void destroy();

	public slots:
		void onTabChanged(int index);
		void show();

	private:
		// Mode enumerator, showing info for single or multiple comics
		enum Mode { SINGLE, MULTIPLE } mode;

		class MultipleInfoPage : public QWidget {
			public:
				static void init();
				static void destroy();
				void onFinished(const int result);

			private:
				MultipleInfoPage();
				~MultipleInfoPage();

				FixedGridLayout *grid_layout;
				QLabel *series_label;
				QLabel *count_label;
				QLabel *volume_label;
				QLabel *alt_series_label;
				QLabel *alt_count_label;
				QLabel *year_label;
				QLabel *month_label;
				QLabel *writer_label;
				QLabel *penciller_label;
				QLabel *inker_label;
				QLabel *colorist_label;
				QLabel *letterer_label;
				QLabel *cover_artist_label;
				QLabel *editor_label;
				QLabel *publisher_label;
				QLabel *imprint_label;
				QLabel *genre_label;
				QLabel *language_iso_label;
				QLabel *format_label;
				QLabel *black_and_white_label;
				QLabel *manga_label;

				QLineEdit *series_value;
				QLineEdit *count_value;
				QLineEdit *volume_value;
				QLineEdit *alt_series_value;
				QLineEdit *alt_count_value;
				QLineEdit *year_value;
				QLineEdit *month_value;
				QLineEdit *writer_value;
				QLineEdit *penciller_value;
				QLineEdit *inker_value;
				QLineEdit *colorist_value;
				QLineEdit *letterer_value;
				QLineEdit *cover_artist_value;
				QLineEdit *editor_value;
				QLineEdit *publisher_value;
				QLineEdit *imprint_value;
				QLineEdit *genre_value;
				QLineEdit *language_iso_value;
				QLineEdit *format_value;
				QComboBox *black_and_white_value;
				QComboBox *manga_value;

				QCompleter *series_completer;
				QCompleter *alt_series_completer;
				QCompleter *writer_completer;
				QCompleter *penciller_completer;
				QCompleter *inker_completer;
				QCompleter *colorist_completer;
				QCompleter *letterer_completer;
				QCompleter *cover_artist_completer;
				QCompleter *editor_completer;
				QCompleter *publisher_completer;
				QCompleter *imprint_completer;
				DelimitedCompleter *genre_completer;
				QCompleter *language_iso_completer;
				QCompleter *format_completer;

				QWidget *v_spacer1;
				QWidget *v_spacer2;
		};

		// Private classes for each info tab.
		class PropertiesTab : public QWidget {
			public:
				static void init(QWidget *parent = 0);
				static void destroy();

			private:
				QHBoxLayout *h_box;
				QVBoxLayout *v_box;
				QLabel *cover;
				QLabel *type_label;
				QLabel *size_label;
				QLabel *pages_label;
				QLabel *path_label;

				PropertiesTab(QWidget *parent);
				~PropertiesTab();
		};

		class InfoTab : public QScrollArea {
			public:
				static void init(QWidget *parent = 0);
				static void destroy();
				void onFinished(const int result);

			private:
				QWidget *grid_view;
				FixedGridLayout *grid_layout;

				QLabel *title_label;
				QLabel *series_label;
				QLabel *number_label;
				QLabel *count_label;
				QLabel *volume_label;
				QLabel *alt_series_label;
				QLabel *alt_number_label;
				QLabel *alt_count_label;
				QLabel *year_label;
				QLabel *month_label;
				QLabel *writer_label;
				QLabel *penciller_label;
				QLabel *inker_label;
				QLabel *colorist_label;
				QLabel *letterer_label;
				QLabel *cover_artist_label;
				QLabel *editor_label;
				QLabel *publisher_label;
				QLabel *imprint_label;
				QLabel *genre_label;
				QLabel *language_iso_label;
				QLabel *format_label;
				QLabel *black_and_white_label;
				QLabel *manga_label;

				QLineEdit *title_value;
				QLineEdit *series_value;
				QLineEdit *number_value;
				QLineEdit *count_value;
				QLineEdit *volume_value;
				QLineEdit *alt_series_value;
				QLineEdit *alt_number_value;
				QLineEdit *alt_count_value;
				QLineEdit *year_value;
				QLineEdit *month_value;
				QLineEdit *writer_value;
				QLineEdit *penciller_value;
				QLineEdit *inker_value;
				QLineEdit *colorist_value;
				QLineEdit *letterer_value;
				QLineEdit *cover_artist_value;
				QLineEdit *editor_value;
				QLineEdit *publisher_value;
				QLineEdit *imprint_value;
				QLineEdit *genre_value;
				QLineEdit *language_iso_value;
				QLineEdit *format_value;
				QComboBox *black_and_white_value;
				QComboBox *manga_value;

				QCompleter *series_completer;
				QCompleter *alt_series_completer;
				QCompleter *writer_completer;
				QCompleter *penciller_completer;
				QCompleter *inker_completer;
				QCompleter *colorist_completer;
				QCompleter *letterer_completer;
				QCompleter *cover_artist_completer;
				QCompleter *editor_completer;
				QCompleter *publisher_completer;
				QCompleter *imprint_completer;
				DelimitedCompleter *genre_completer;
				QCompleter *language_iso_completer;
				QCompleter *format_completer;

				QWidget *v_spacer1;
				QWidget *v_spacer2;

				InfoTab(QWidget *parent);
				~InfoTab();
		};

		class PlotTab : public QWidget {
			public:
				static void init(QWidget *parent = 0);
				static void destroy();
				void onFinished(const int result);

			private:
				QVBoxLayout *v_box;

				QLabel *summary_label;
				QLabel *notes_label;
				QLabel *web_label;

				QTextEdit *summary_value;
				QTextEdit *notes_value;
				QLineEdit *web_value;

				PlotTab(QWidget *parent);
				~PlotTab();
		};

		class PagesTab : public QWidget {
			public:
				static void init(QWidget *parent = 0);
				static void destroy();
				void onFinished(const int result);
				void onShow();

			private:
				QVBoxLayout *v_box;
				PageListView *page_list_view;
				bool pages_extracted = false;

				PagesTab(QWidget *parent);
				~PagesTab();
		};

		// Private variables
		bool unsaved_changes	=	false;
		bool finishing			=	false;// Make sure PageTab::onShow() isn't called on destruction
		static MultipleInfoPage *multiple_info_page;
		static PropertiesTab *properties_tab;
		static InfoTab *info_tab;
		static PlotTab *plot_tab;
		static PagesTab *pages_tab;
		QVBoxLayout *v_box;
		QTabWidget *tab_widget;
		QDialogButtonBox *button_box;
		ReferenceList<ComicFile> selected_list;

		// private methods
		ComicInfoDialog(QWidget *parent);
		~ComicInfoDialog();
		void initView();

	private slots:
		void onFinished(const int result);
};


extern ComicInfoDialog *comic_info_dialog;


#endif