#include <QtGui/QDragEnterEvent>
#include <QUrl>
#include "QLabelWithDrag.h"

QLabelWithDrag::QLabelWithDrag(QWidget *parent,Qt::WFlags flags):QLabel(parent,flags)
{
	setAcceptDrops(true);
}

QLabelWithDrag::~QLabelWithDrag()
{
}

void QLabelWithDrag::dragEnterEvent(QDragEnterEvent* de)
{
	QList<QUrl> urls = de->mimeData()->urls();
	foreach(QUrl u,urls)
		emit open(u.toLocalFile());
}