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

#ifndef TASKWIDGETITEM_H
#define TASKWIDGETITEM_H

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <Plasma/Frame>

#include <KCalCore/Todo>
#include <Akonadi/Item>

#include "taskeditor.h"
#include "taskwidgetitemdate.h"
#include "taskwidgetiteminfo.h"

namespace Plasma
{
    class CheckBox;
    class Label;
    class IconWidget;
}

class KJob;

class TaskWidgetItem : public Plasma::Frame
{
    Q_OBJECT

public:

    TaskWidgetItem(const Akonadi::Item & item, QGraphicsWidget * parent = 0);

    int indent() const {
        return m_indent;
    }

    QString relatedTo() const {
        return m_todo->relatedTo(KCalCore::Incidence::RelTypeParent);
    }

    QString uid() const {
        return m_todo->uid();
    }

    bool isCompleted() const {
        return m_todo->isCompleted();
    }

    Akonadi::Item item() const {
        return m_item;
    }

    void setRelated(TaskWidgetItem * item);
    void setUnrelated();

    void updateTask(const Akonadi::Item & item);

    bool operator<(const TaskWidgetItem * item);
    bool operator<<(const TaskWidgetItem * item);
    bool operator==(const Akonadi::Item & item);

public slots:
    void setCompleted();
    void editTask();
    void saveTask();

private slots:
    void modifyFinished(KJob * job);

private:
    void setItemInfo();
    void setColorForDate();

    bool orderByName(const TaskWidgetItem * item, const bool & completedFirst = false);
    bool orderByDate(const TaskWidgetItem * item, const bool & completedFirst = false);

    TaskEditor * m_editor;

    QGraphicsLinearLayout * m_layout;

    TaskWidgetItemDate * m_date;
    TaskWidgetItemInfo * m_name;

    Akonadi::Item m_item;
    KCalCore::Todo::Ptr m_todo;

    int m_indent;

};

#endif // TASKWIDGETITEM_H
