#ifndef QLABEL_WITH_DRAG_H
#define QLABEL_WITH_DRAG_H
#include <QtGui/QLabel>

class QLabelWithDrag : public QLabel
{
    Q_OBJECT
public:
	QLabelWithDrag(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QLabelWithDrag();
signals:
	void open(const QString&);
private:
	void dragEnterEvent(QDragEnterEvent* de);
};

#endif