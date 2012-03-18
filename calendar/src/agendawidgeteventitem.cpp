/*
    Akonadi google calendar plasmoid - agendawidgeteventitem.cpp
    Copyright (C) 2012  Jan Grulich <grulja@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "agendawidgeteventitem.h"

AgendaWidgetEventItem::AgendaWidgetEventItem(const Akonadi::Item & item, QGraphicsItem * parent)
    : QGraphicsWidget(parent)
{
    m_item = item;
    
    m_event = item.payload<KCalCore::Event::Ptr>();
    
    m_mainLayout = new QGraphicsLinearLayout(this);
    m_textLayout = new QGraphicsLinearLayout(Qt::Vertical,m_mainLayout);
    
    m_icon = new Plasma::IconWidget(this);
    m_icon->setOrientation(Qt::Horizontal);
    m_icon->setMinimumWidth(50);
    m_icon->setMaximumHeight(15);
    m_icon->setText(m_event->summary());
    
    QGraphicsWidget * line = new QGraphicsWidget();
    line->setMaximumHeight(20);
    line->setMaximumWidth(5);
    
    line->setAutoFillBackground(true);
    
    QColor color = Qt::green;
    color.setAlphaF(0.3);
    QPalette palette;
    palette = line->palette();
    palette.setColor(QPalette::Window,color);
    line->setPalette(palette);
    
    m_textLayout->addItem(m_icon);
    
    m_mainLayout->addItem(line);
    m_mainLayout->addItem(m_textLayout);
    
}
