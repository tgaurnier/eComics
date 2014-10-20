#include "FixedGridLayout.hpp"

#include <QDebug>
#include <QWidget>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								FIXEDGRIDLAYOUT PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


FixedGridLayout::FixedGridLayout(int col_width, int row_height)
	: width(col_width), height(row_height) {}


FixedGridLayout::FixedGridLayout(QWidget *parent, int col_width, int row_height)
		: QGridLayout(parent), width(col_width), height(row_height) {}


FixedGridLayout::~FixedGridLayout() {}


void FixedGridLayout::addWidget(QWidget *widget, int row, int col, Qt::Alignment alignment) {
	addWidget(widget, row, col, 1, 1, alignment);
}


void FixedGridLayout::addWidget(QWidget *widget, int row, int col, int row_span, int col_span,
		Qt::Alignment alignment) {
	// First set fixed width based on col_span and horizontal spacing
	widget->setFixedWidth((width * col_span) + (horizontalSpacing() * (col_span - 1)));

	// Next set fixed height based on row_span and vertical spacing, but only if height != -1
	if(height != -1) {
		widget->setFixedHeight((height * row_span) + (verticalSpacing() * (row_span - 1)));
	}

	// Last actually add widget to grid view
	QGridLayout::addWidget(widget, row, col, row_span, col_span);
}