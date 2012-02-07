#ifndef PLASMA_CONTACTS_HEADER
#define PLASMA_CONTACTS_HEADER

#include <KIcon>
 
#include <Plasma/Applet>
 
 
class PlasmaContacts : public Plasma::Applet
{
    Q_OBJECT
    public:
	
        PlasmaContacts(QObject *parent, const QVariantList &args);
        ~PlasmaContacts();
 
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
	void init();
 
    private:
        KIcon m_icon;
};
 
K_EXPORT_PLASMA_APPLET(plasma_contacts, PlasmaContacts)


#endif 
