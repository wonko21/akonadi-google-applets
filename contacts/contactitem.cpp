#include "contactitem.h"

#include <KIcon>

#include <QIcon>

#include <KABC/Picture>
#include <KABC/PhoneNumber>

#include <Plasma/ToolTipManager>
#include <Plasma/ToolTipContent>


ContactItem::ContactItem(KABC::Addressee * addr, QGraphicsWidget* parent): Plasma::IconWidget(parent)
{        
    setOrientation(Qt::Horizontal);
    setDrawBackground(true);
    
    m_addressee = addr;
    
    if (m_addressee->photo().isEmpty()) {
		
	setIcon(KIcon("user-identity"));
	
    } else {

	QPixmap pixmap;
    
        pixmap.convertFromImage(addr->photo().data());
    
        setIcon(KIcon(QIcon(pixmap)));
	
    }
        
    setMaximumSize(250,50);
    setMinimumSize(250,50);
    
    setWidgetText();        
}

void ContactItem::setWidgetText()
{

    if (m_addressee->name().isEmpty()) {
    
        QStringList emails = m_addressee->emails();
	
	if (!emails.empty()) {
	 
	    setText(emails.first());
	    
	} else {
	 
	    setText("Contact without name");
	    
	}
	
    } else {
	
	setText(m_addressee->name());
	
    }
    
}

void ContactItem::setShowInfo(bool emails, bool numbers)
{
    QString main = text() + "<br/>";  
    
    // TODO: show more numbers 
    
    QString sub; 
    
    if (numbers) {
    
	if (!m_addressee->phoneNumber(KABC::PhoneNumber::Home).isEmpty()) {
	
	    sub += QString::fromUtf8("<strong>Telefon domů: </strong>") + m_addressee->phoneNumber(KABC::PhoneNumber::Home).number() + "<br/>";
	
	}
    
	if (!m_addressee->phoneNumber(KABC::PhoneNumber::Work).isEmpty() && numbers) {
	    
	    sub += QString::fromUtf8("<strong>Telefon do práce: </strong>") + m_addressee->phoneNumber(KABC::PhoneNumber::Work).number() + "<br/>";
	
	}
    
    // TODO: show more emails
    }
    
    if (emails) {
	
	if (!m_addressee->emails().isEmpty() && emails) {
	
	    sub += "<strong>Email: </strong> <a href=\"mailto:" + m_addressee->emails().first() + "\">" + m_addressee->emails().first() + "</a><br/>";
	
	}
	
    }
    
    // TODO: 
    
    if (sub.isEmpty()) {
	
	hideContact();
	
    }
    
    Plasma::ToolTipContent m_tooltip;
        
    m_tooltip.setMainText(main);
    m_tooltip.setImage(icon());
    m_tooltip.setSubText(sub);
    m_tooltip.setAutohide(false);
    
    m_tooltip.setAutohide(false);
    m_tooltip.setClickable(true);
    
    // TODO: capture linkActivated signal (KRun)
    
    Plasma::ToolTipManager::self()->setContent(this, m_tooltip);
           
        
}

bool ContactItem::containsString(const QString & string)
{

    return text().toLower().contains(string.toLower());
    
}

bool ContactItem::containsPhone()
{

    return (!m_addressee->phoneNumbers().isEmpty());    
    
}

bool ContactItem::containsMail()
{

    return (!m_addressee->emailLabel().isEmpty());
    
}

// TODO
void ContactItem::hideContact()
{

    hide();  
    
}

// TODO
void ContactItem::showContact()
{
    show();
    
}





