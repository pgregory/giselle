#ifndef DRAGGABLESPINBOX_H
#define DRAGGABLESPINBOX_H

#include <QDoubleSpinBox>
#include <QPoint>

class DraggableSpinbox : public QDoubleSpinBox
{
public:
    DraggableSpinbox(QWidget* parent = 0);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    bool    m_dragging;
    QPoint  m_startPoint;
    double  m_startValue;
};

#endif // DRAGGABLESPINBOX_H
