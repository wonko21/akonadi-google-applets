/*
    Akonadi google calendar plasmoid - calendarwidget.cpp
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


#include "calendarwidget.h"

#include <Akonadi/Entity>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemFetchJob>

#include <KCalCore/Event>

#include <KDateTime>
#include <KIntSpinBox>

CalendarWidget::CalendarWidget(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags),
    m_mainLayout(0),
    m_changeLayout(0),
    m_daysLayout(0),
    m_spin(0),
    m_combo(0),
    m_firstDay(1)
{
    m_mainLayout = new QGraphicsLinearLayout(Qt::Vertical,this);
    m_daysLayout = new QGraphicsGridLayout(m_mainLayout);
    m_changeLayout = new QGraphicsLinearLayout(m_mainLayout);
    
    m_spin = new Plasma::SpinBox(this);
    m_spin->nativeWidget()->setMaximum(2099);
    
    connect(m_spin,SIGNAL(valueChanged(int)),SLOT(yearChanged(int)));
    
    m_combo = new Plasma::ComboBox(this);
    m_combo->addItem(i18n("January"));
    m_combo->addItem(i18n("February"));
    m_combo->addItem(i18n("March"));
    m_combo->addItem(i18n("April"));
    m_combo->addItem(i18n("May"));
    m_combo->addItem(i18n("June"));
    m_combo->addItem(i18n("July"));
    m_combo->addItem(i18n("August"));
    m_combo->addItem(i18n("September"));
    m_combo->addItem(i18n("October"));
    m_combo->addItem(i18n("November"));
    m_combo->addItem(i18n("December"));
    
    connect(m_combo,SIGNAL(currentIndexChanged(int)),SLOT(monthChanged(int)));
    
    m_changeLayout->addItem(m_combo);
    m_changeLayout->addItem(m_spin);

    
    QFont fnt = font();
    fnt.setPointSize(fnt.pointSize()-3);
    
    Plasma::IconWidget * m_weekDay;         
             
    for (int i = 0; i < 7; i++) {
                
        m_weekDay = new Plasma::IconWidget(this);
        m_weekDay->setMinimumSize(10,10);
        m_weekDay->setFont(fnt);
               
        m_daysLayout->addItem(m_weekDay,0,i);
        
    }
             
    CalendarWidgetDayItem * m_dayItem;
    
    for (int i = 0; i < 7; i++) {
        
        for (int j = 1; j < 7; j++) {
            
            m_dayItem = new CalendarWidgetDayItem(this);
            
            m_daysLayout->addItem(m_dayItem,j,i);
            connect(m_dayItem,SIGNAL(clicked(QDate)),SLOT(setDate(QDate)));
            
        }
        
    }
    
    m_mainLayout->addItem(m_changeLayout);
    m_mainLayout->addItem(m_daysLayout);
    
    setLayout(m_mainLayout);
    
    setDate(QDate::currentDate());
}

void CalendarWidget::setCollections(QList< Akonadi::Entity::Id > ids)
{  
    clearEvents();
    
    m_idList = ids;
    
    if (!m_idList.isEmpty()) {

        fetchCollections();
        
    }
    
}

void CalendarWidget::fetchCollections()
{
    Akonadi::CollectionFetchJob * job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this);

    job->fetchScope();

    connect(job, SIGNAL(result(KJob *)), SLOT(fetchCollectionsFinished(KJob *)));
}

void CalendarWidget::fetchCollectionsFinished(KJob * job)
{
    if (job->error()) {

        qDebug() << "fetchCollections failed";

        return;
    }

    Akonadi::CollectionFetchJob * fetchJob = qobject_cast<Akonadi::CollectionFetchJob *>(job);

    const Akonadi::Collection::List collections = fetchJob->collections();

    foreach (const Akonadi::Collection & collection, collections) {

        if (m_idList.contains(collection.id())) {
            
            fetchItems(collection);

        }

    }
    
}

void CalendarWidget::fetchItems(const Akonadi::Collection & collection)
{
    Akonadi::ItemFetchJob * job = new Akonadi::ItemFetchJob(collection);

    connect(job, SIGNAL(result(KJob *)), SLOT(fetchItemsFinished(KJob *)));

    job->fetchScope().fetchFullPayload(true);
    
}

void CalendarWidget::fetchItemsFinished(KJob * job)
{
    if (job->error()) {

        qDebug() << "fetchItems failed";

        return;
    }

    Akonadi::ItemFetchJob * fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);

    const Akonadi::Item::List items = fetchJob->items();

    foreach (const Akonadi::Item & item, items) {

        if (item.hasPayload<KCalCore::Event::Ptr>()) {
        
            addItem(item);
            
        }

    }
    
}

void CalendarWidget::addItem(const Akonadi::Item & item)
{
    QDate min = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(1,0))->date();
    QDate max = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(6,6))->date();
        
    KCalCore::Event::Ptr event = item.payload<KCalCore::Event::Ptr>();
    
    QDate dateStart = event->dtStart().date(); 
    QDate dateEnd = event->dtEnd().date();
    QDate date = dateStart;
    
    int daysTo = dateStart.daysTo(dateEnd );
            
    if (dateStart > max) {

        return;
        
    } else if (dateEnd == min && !event->allDay()) {
        
        return;
        
    } else if (dateStart < min && dateEnd < min && !event->recurs()) {
        
        return;
        
    } else if (dateStart < min && event->recurs()) {

        date = event->recurrence()->getPreviousDateTime(KDateTime(min)).date();

        if (date.addDays(daysTo) < min) {
        
            date = event->recurrence()->getNextDateTime(KDateTime(date)).date();
        
        }
        
        if (date.addDays(daysTo) < min || date > max) {

            return;
            
        }
        
    } 
                              
    for (int i = 1; i < daysTo; i++) {
        
        if (date.addDays(i) >= min  && date.addDays(i) <= max ) {

            setColored(date.addDays(i));
            
        }
    }
    
    if (dateStart == dateEnd ) {

        setColored(date);
        
        return;
    }
    
    if (date >= min ) { 
        
         setColored(date);
        
    } 

    if (dateEnd <= max ) {
    
        setColored(date.addDays(daysTo));
        
    }
    
}

void CalendarWidget::setColored(QDate date)
{    
    CalendarWidgetDayItem * dayItem;
    
    for (int i = 0; i < 7; i++) {
        
        for (int j = 1; j < 7; j++) {
            
            dayItem = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(j,i));
            
            if (dayItem->date() == date && date != m_date) {
            
                if (m_date.month() == date.month()) {
                
                    dayItem->setEvent(true);
                    
                } else {
                    
                    dayItem->setEvent(false);
                    
                }
                
            }
            
        }
        
    }
}


void CalendarWidget::clearEvents()
{
    CalendarWidgetDayItem * dayItem;
    
    for (int i = 0; i < 7; i++) {
        
        for (int j = 1; j < 7; j++) {
            
            dayItem = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(j,i));
            
            if (dayItem->date() == m_date) {
                
                dayItem->setActualDay();
                
            } else if (dayItem->date().month() == m_date.month()) {
                
                dayItem->setActualMonth(true);
                
            } else {
                
                dayItem->setActualMonth(false);
               
            }
            
                       
        }
        
    }
    
}

void CalendarWidget::setFirstDay(int day)
{
    m_firstDay = day;

    QStringList daysList;
    
    if (m_firstDay == 1) {
        
        daysList << i18n("Mon");
        
    } else {
        
        daysList << i18n("Sun");
        daysList << i18n("Mon");
        
    }
    
    daysList << i18n("Tue") << i18n("Wed") << i18n("Thu")
             << i18n("Fri") << i18n("Sat");
             
    if (m_firstDay != 7) {
        
        daysList << i18n("Sun");
        
    } 
    
    CalendarWidgetDayItem * dayItem;
    
    for (int i = 0; i < 7; i++) {
        
        dayItem = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(0,i));
        dayItem->setText(daysList.at(i));
        
    }
    
    setDate(m_date);
    
}

void CalendarWidget::setDate(QDate date)
{ 
    if (!date.isValid()) {
        
        return;
        
    }
        
    m_date = date;
    
    m_spin->setValue(m_date.year());
    m_combo->setCurrentIndex(m_date.month()-1);
    
    QDate firstDate(m_date.year(),m_date.month(),1);
    int weekDay = firstDate.dayOfWeek()-1;
    
    if (m_firstDay == 1) {
    
        if (weekDay == 0) {
        
            firstDate = firstDate.addDays(-7);
        
        } else {
        
            firstDate = firstDate.addDays(-weekDay);
        
        }
        
    } else {
        
        if (weekDay == 6) {
        
            firstDate = firstDate.addDays(-7);
        
        } else {
        
            firstDate = firstDate.addDays(-weekDay-1);
        
        }
        
    }
    
    CalendarWidgetDayItem * dayItem;
    
    for (int week = 1; week < 7; week++) {
        
        for (int day = 0; day < 7; day++) {
            
            dayItem = static_cast<CalendarWidgetDayItem*>(m_daysLayout->itemAt(week,day));
            dayItem->setDay(firstDate);
            
            if (firstDate == m_date) {
                
                dayItem->setActualDay();
                
            } else {
            
                if (firstDate.month() == m_date.month()) {
                
                    dayItem->setActualMonth(true);
                
                } else {
            
                    dayItem->setActualMonth(false);
                
                }
                
            }
        
            firstDate = firstDate.addDays(1);
            
        }
        
    }
    
    setCollections(m_idList);
    
}

void CalendarWidget::yearChanged(int year)
{
    QDate dt(year,m_date.month(),m_date.day());
    
    if (dt.isValid()) {
        
        setDate(dt);
        
    } else {
        
        dt.setDate(year,m_date.month(),1);
        setDate(dt);
        
    }
}

void CalendarWidget::monthChanged(int month)
{
    QDate dt(m_date.year(),month+1,m_date.day());
    
    if (dt.isValid()) {
        
        setDate(dt);
        
    } else {
        
        dt.setDate(m_date.year(),month+1,1);
        setDate(dt);
        
    }
}
