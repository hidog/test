#include "vectorlist.h"
#include <QHeaderView>
#include <QTableWidget>
#include <iostream>

VectorList::VectorList(QWidget *parent) : QTableWidget(parent)
{
    rowHeight = 20;
    rows = 0;
    rowSelected = 0;
    previousRow = 0;
    setupConnections();
}

VectorList::~VectorList()
{

}

void VectorList::setupConnections()
{
    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(rowSelect(int,int)));
    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(rowDoubleClicked(int,int)));
}

void VectorList::setData(DataContainer *filedataPass)
{
    filedata = filedataPass;
}


void VectorList::highlightRow(int rowSelectedPass)
{
    rowSelected = rowSelectedPass;
    QFont font0, font1;
    font0 = this->item(rowSelected,0)->font();
    font0.setBold(true);
    this->item(rowSelected,0)->setFont(font0);
    font1 = this->item(rowSelected,1)->font();
    font1.setBold(true);
    this->item(rowSelected,1)->setFont(font1);
}

void VectorList::clearHighlight()
{
    QFont font0, font1;
    font0 = this->item(rowSelected,0)->font();
    font0.setBold(false);
    this->item(rowSelected,0)->setFont(font0);
    font1 = this->item(rowSelected,1)->font();
    font1.setBold(false);
    this->item(rowSelected,1)->setFont(font1);
}

void VectorList::rowSelect(int row, int column)
{
    this->selectRow(row);
    previousRow = row;
}

void VectorList::rowDoubleClicked(int row, int column)
{
    this->selectRow(row);
    emit(fileClicked(row));
}
