#include <plasma-contacts.h>
#include <Plasma/Theme>
#include <QPainter>

PlasmaContacts::PlasmaContacts(QObject *parent, const QVariantList &args): Plasma::Applet(parent, args)
{
    m_icon = KIcon("kde");
     
    setBackgroundHints(DefaultBackground);

}

PlasmaContacts::~PlasmaContacts()
{

}

 
void PlasmaContacts::init()
{

} 
 
void PlasmaContacts::paintInterface(QPainter *painter,
				     const QStyleOptionGraphicsItem *option, 
				     const QRect &contentsRect)
{
    Q_UNUSED(option);
    
    painter->setPen(Qt::white);
    painter->drawText(contentsRect,
                      Qt::AlignCenter | Qt::AlignHCenter,
                      "Contacts plasmoid");
    painter->restore();
}
 
#include "plasma-contacts.moc"

 
