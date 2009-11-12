#ifndef AVARLISTITEM_H
#define AVARLISTITEM_H

#include <QPair>
#include <QList>

class AvarListItem
{
public:
    AvarListItem(const QString& name, const QList<QPair<float, float> > &keyframes);
    ~AvarListItem();

    int columnCount() const;
    QVariant data(int column) const;
    const QString& name() const;
    QString& name();
    QList<QPair<float, float> >& keyframes();
    const QList<QPair<float, float> >& keyframes() const;

private:
    QString _name;
    QList<QPair<float, float> > _keyframes;
 };


#endif // AVARLISTITEM_H
