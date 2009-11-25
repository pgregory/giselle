#include "draggablespinbox.h"

#include <QMouseEvent>

DraggableSpinbox::DraggableSpinbox(QWidget* parent) : QDoubleSpinBox(parent), m_dragging(false)
{
}


void DraggableSpinbox::mousePressEvent(QMouseEvent* event)
{
    m_dragging = true;
    m_startPoint = event->pos();
    m_startValue = value();
    grabMouse();
}

void DraggableSpinbox::mouseMoveEvent(QMouseEvent* event)
{
    if(m_dragging)
    {
        QPoint p = event->pos();
        double diff = p.y() - m_startPoint.y();
        setValue(m_startValue + diff);
    }
}

void DraggableSpinbox::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_dragging)
    {
        m_dragging = false;
        releaseMouse();
    }
}
