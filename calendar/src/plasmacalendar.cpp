/*
    Akonadi google calendar plasmoid - plasmacalendar.cpp
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


#include "plasmacalendar.h"

#include <QListWidgetItem>

#include <KConfigDialog>

#include <Akonadi/EntityDisplayAttribute>

#include <Plasma/IconWidget>

PlasmaCalendar::PlasmaCalendar(QObject * parent, const QVariantList & args)
    : Plasma::PopupApplet(parent, args),
      m_widget(0),
      m_agenda(0)
{
    setConfigurationRequired(true);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);
    setPopupIcon(icon());
}

QGraphicsWidget * PlasmaCalendar::graphicsWidget()
{
     if (!m_widget) {
	 
        m_widget = new QGraphicsWidget(this);
        m_widget->setMinimumSize(300, 400);

	m_agenda = new AgendaWidget(m_widget);
	
        m_layout = new QGraphicsLinearLayout(Qt::Vertical,m_widget); 

	m_scroll = new Plasma::ScrollWidget(this);
	m_scroll->setWidget(m_agenda);
	
	m_tab = new Plasma::TabBar(m_widget);
	m_tab->addTab(i18n("Agenda"),m_scroll);
	m_tab->addTab(i18n("Calendar view"),new Plasma::IconWidget(m_widget));
        m_tab->setTabBarShown(false);
	
        m_layout->addItem(m_tab);
	
	m_widget->setLayout(m_layout);
    }

    return m_widget;
    
}

void PlasmaCalendar::configChanged()
{
    KConfigGroup conf = config();
    
    QMap<Akonadi::Entity::Id,QString> map;
    
    QList<Akonadi::Item::Id> list = conf.readEntry("collections", QList<Akonadi::Item::Id>());

    m_agenda->setDateColor(conf.readEntry("dateColor","#343E88"));
    m_agenda->setWeeks(conf.readEntry("weeks",1));
    
    if (list.isEmpty()) {

        setConfigurationRequired(true);

    } else {

        setConfigurationRequired(false);

    }

    m_agenda->setCollections(list);
    
    foreach (Akonadi::Entity::Id id, m_agenda->collectionsList()) {
        
        map.insert(id,conf.readEntry(QString::number(id),"#00C000"));
        
    }
    
    m_agenda->setCalendarsColors(map);

}

void PlasmaCalendar::createConfigurationInterface(KConfigDialog * parent)
{
    QWidget * widget = new QWidget(0);

    configDialog.setupUi(widget);

    KConfigGroup conf = config();

    configDialog.loadCollections->setIcon(KIcon("view-refresh"));

    connect(configDialog.loadCollections, SIGNAL(clicked(bool)), SLOT(fetchCollections()));

    parent->addPage(widget, i18n("General"), icon());
    
    agendaConfigDialog = new AgendaConfig(0);
    
    agendaConfigDialog->setCalendarsColors(m_agenda->calendarsColors());
    agendaConfigDialog->setDateColor(QColor(m_agenda->dateColor()));
    agendaConfigDialog->setWeeks(m_agenda->weeks()-1);
    
    parent->addPage(agendaConfigDialog, i18n("Agenda"), "view-calendar-agenda");

    connect(agendaConfigDialog, SIGNAL(updateCalendars()), SLOT(updateCalendars()));
    
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    
    fetchCollections();
}

void PlasmaCalendar::configAccepted()
{
    KConfigGroup conf = config();

    QList<Akonadi::Item::Id> list;

    for (int i = 0; i < configDialog.collectionsList->count(); i++) {

        if (configDialog.collectionsList->item(i)->checkState()) {

            list.push_back(configDialog.collectionsList->item(i)->data(Qt::UserRole).toInt());

        }

    }

    if (agendaConfigDialog->dateColor() != m_agenda->dateColor()) {

        conf.writeEntry("dateColor", agendaConfigDialog->dateColor());
    }
    
    if (agendaConfigDialog->weeks() != m_agenda->weeks()) {

        conf.writeEntry("weeks", agendaConfigDialog->weeks());
    }
    
    conf.writeEntry("collections", list);
    
    foreach (Akonadi::Entity::Id id, agendaConfigDialog->calendarsColors().keys()) {
        
        conf.writeEntry(QString::number(id),agendaConfigDialog->calendarsColors()[id]);
        
    }
    
    emit configNeedsSaving();
   
}

void PlasmaCalendar::fetchCollections()
{
    while (configDialog.collectionsList->count() != 0) {

        delete configDialog.collectionsList->item(0);

    }

    Akonadi::CollectionFetchJob * job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this);

    job->fetchScope();

    connect(job, SIGNAL(result(KJob *)), SLOT(fetchCollectionsFinished(KJob *)));
}

void PlasmaCalendar::updateCalendars()
{

    agendaConfigDialog->clear();
    
    for (int i = 0; i < configDialog.collectionsList->count(); i++) {
        
        if (configDialog.collectionsList->item(i)->checkState() == Qt::Checked) {
            
            agendaConfigDialog->addItem(configDialog.collectionsList->item(i)->text(), 
                                        configDialog.collectionsList->item(i)->data(Qt::UserRole).toInt());
            
        }
        
    }
    
}


void PlasmaCalendar::fetchCollectionsFinished(KJob * job)
{
    if (job->error()) {

        qDebug() << "fetchCollections failed";

        return;
    }

    Akonadi::CollectionFetchJob * fetchJob = qobject_cast<Akonadi::CollectionFetchJob *> (job);
    const Akonadi::Collection::List collections = fetchJob->collections();

    foreach (const Akonadi::Collection & collection, collections) {

#ifndef ALL_COLLECTIONS
        if (collection.resource().contains("akonadi_googlecalendar_resource")) {
#endif
            if (collection.contentMimeTypes().contains(KCalCore::Event::eventMimeType())) {

                Akonadi::EntityDisplayAttribute * attribute = collection.attribute< Akonadi::EntityDisplayAttribute > ();
	    
                QListWidgetItem * item = new QListWidgetItem();
	        
	        QString name;

	        if (collections.contains(collection.parentCollection())) {
		 
		    Akonadi::Collection col = collections.at(collections.indexOf(collection.parentCollection()));
		    Akonadi::EntityDisplayAttribute * attr = col.attribute< Akonadi::EntityDisplayAttribute > ();
		    
		    if (!attribute) {
			
			name = col.name();
			
		    } else {
			
			name = attr->displayName();
			
		    }
		    
		    name += " / ";
		}
	     
                if (!attribute) {

		    name += collection.name();
		    
                } else {

                   name += attribute->displayName();

		}
                
                item->setText(name);

                item->setData(Qt::UserRole, collection.id());
                item->setCheckState(Qt::Unchecked);

                configDialog.collectionsList->insertItem(configDialog.collectionsList->count(), item);

            }
#ifndef ALL_COLLECTIONS
        }
#endif
    }

    if (!m_agenda->collectionsList().isEmpty()) {

        for (int i = 0; i < m_agenda->collectionsList().count(); i++) {

            for (int j = 0; j < configDialog.collectionsList->count(); j++) {

                if (m_agenda->collectionsList().at(i) == configDialog.collectionsList->item(j)->data(Qt::UserRole).toInt()) {

                    configDialog.collectionsList->item(j)->setCheckState(Qt::Checked);

                }

            }

        }

    }
    
    updateCalendars();

}
 
#include "plasmacalendar.moc"

