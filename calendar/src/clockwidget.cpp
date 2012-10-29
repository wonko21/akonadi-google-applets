/*
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


#include "clockwidget.h"

#include <Plasma/DataEngineManager>

ClockWidget::ClockWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags):
    QGraphicsWidget(parent, wFlags),
    m_layout(new QGraphicsLinearLayout(Qt::Vertical, this)),
    m_timeLabel(new Plasma::Label(this)),
    m_dateLabel(new Plasma::Label(this))
{
    m_engine = Plasma::DataEngineManager::self()->loadEngine("time");
    m_engine->connectSource("Local", this, 60000, Plasma::AlignToMinute);

    Plasma::DataEngine::Data data = m_engine->query("Local");
    m_date = data["Date"].toDate();
    m_time = data["Time"].toTime();

    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setAlignment(Qt::AlignCenter);

    updateLabels();

    m_layout->setItemSpacing(0,2);
    m_layout->setContentsMargins(0, 0, 0, 2);
    m_layout->addItem(m_timeLabel);
    m_layout->addItem(m_dateLabel);

    setLayout(m_layout);

}

void ClockWidget::updateLabels()
{
    QString time = QString::number(m_time.hour()) + ":" + QString::number(m_time.minute());
    m_timeLabel->setText(time);
    QString date = QString::number(m_date.day()) + "/" + QString::number(m_date.month());
    m_dateLabel->setText(date);

    update();
}

void ClockWidget::updateSize(const QSize & size, const Plasma::FormFactor factor)
{
    int timeSize = font().pixelSize();
    int dateSize = font().pixelSize();

    if (factor == Plasma::Horizontal) {
	timeSize = ((size.height()/3) * 2) - 2;
	dateSize = (size.height()/3) - 1;
    } else if (factor == Plasma::Vertical) {
	timeSize = ((size.width()/6) * 2) - 2;
	dateSize = (size.width()/6) - 1;
    }

    QFont timeFont = m_timeLabel->font();
    timeFont.setPixelSize(timeSize);
    m_timeLabel->setFont(timeFont);
    m_timeLabel->setMaximumHeight(timeSize);

    QFont dateFont = m_dateLabel->font();
    dateFont.setPixelSize(dateSize);
    m_dateLabel->setFont(dateFont);
    m_dateLabel->setMaximumHeight(dateSize);

    update();
}

void ClockWidget::dataUpdated(const QString& name, const Plasma::DataEngine::Data& data)
{
    qDebug() << "updated";
    Q_UNUSED(name);

    m_date = data["Date"].toDate();
    m_time = data["Time"].toTime();

    updateLabels();
}

ClockWidget::~ClockWidget()
{ }

