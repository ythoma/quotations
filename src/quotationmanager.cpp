

#include <QMessageBox>
#include <QRandomGenerator>

#include "quotationmanager.h"


/*! \brief Opens the XML file containing the citations
 *  It opens the file corresponding to the one of the settings.
*/
bool QuotationManager::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot open the quotation XML file"));
        return false;
    }

    if (!m_xmlDoc->setContent(&file)) {
        QMessageBox::warning(
                nullptr, QObject::tr("Error"), QObject::tr("Cannot retrieve quotations from the XML file"));
        file.close();
        return false;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = m_xmlDoc->documentElement();

    QDomNode n = docElem.firstChild();
    m_nbQuotations = 0;

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull()) {
            m_nbQuotations++;
        }
        n = n.nextSibling();
    }
    return true;
}


bool QuotationManager::saveFile(const QString& fileName)
{
    if (QFile::exists(fileName)
        && QMessageBox::question(
                   nullptr,
                   QObject::tr("Overwrite File? -- Quotation manager"),
                   QObject::tr("A file called %1 already exists. "
                               "Do you want to overwrite it?")
                           .arg(fileName),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No)
                   != QMessageBox::Yes)
        return false;


    QFile fileo(fileName);
    if (QFile::exists(fileName + ".bak")) {
        QFile::remove(fileName + ".bak");
    }
    if (!fileo.copy(fileName + ".bak")) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("No backup of the file could be created"));
    }
    if (!fileo.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot create the quotations file"));
        return false;
    }
    fileo.write(m_xmlDoc->toString().toLatin1());

    fileo.close();
    m_modified = false;
    return true;
}

bool QuotationManager::getQuote(int index, Quotation* quote) const
{
    if (quote == nullptr)
        return false;
    if (index >= m_nbQuotations)
        return false;
    if (index < 0)
        return false;
    // Index of the new quotation
    int num = index;

    QDomElement docElem = m_xmlDoc->documentElement();

    QDomNode n = docElem.firstChild();

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if ((num == 0) && (!e.isNull()))
            return quoteFromNode(n, quote);
        n = n.nextSibling();
        num--;
    }
    return false;
}

bool QuotationManager::quoteFromNode(const QDomNode& node, Quotation* quote) const
{
    QDomNode n1 = node.firstChild();
    QString q;
    QString author;
    QString book;
    bool authorOk = false, textOk = false;
    while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (e1.tagName().compare("text") == 0) {
            textOk = true;
            q = e1.text();
        }
        else if (e1.tagName().compare("author") == 0) {
            authorOk = true;
            author = e1.text();
        }
        else if (e1.tagName().compare("book") == 0) {
            book = e1.text();
        }
        n1 = n1.nextSibling();
    }
    if (textOk && authorOk) {
        quote->m_text = q;
        quote->m_author = author;
        quote->m_book = book;
        return true;
    }
    return false;
}

int QuotationManager::randomQuote(Quotation* quote) const
{
    if (quote == nullptr)
        return -1;
    if (m_nbQuotations == 0)
        return -1;
    // Index of the new quotation
    int num = QRandomGenerator::global()->bounded(m_nbQuotations);
    int index = num;

    QDomElement docElem = m_xmlDoc->documentElement();

    QDomNode n = docElem.firstChild();

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if ((num == 0) && (!e.isNull()))
            return quoteFromNode(n, quote) ? index : -1;
        n = n.nextSibling();
        num--;
    }
    return -1;
}


int QuotationManager::replaceQuotation(int index, const Quotation& quote)
{
    removeQuotation(index);
    return addQuotation(quote);
}


int QuotationManager::addQuotation(const Quotation& quote)
{
    QDomElement newnode = m_xmlDoc->createElement("citation");

    QDomElement q = m_xmlDoc->createElement("text");
    q.appendChild(m_xmlDoc->createTextNode(quote.m_text));
    newnode.appendChild(q);

    QDomElement a = m_xmlDoc->createElement("author");
    a.appendChild(m_xmlDoc->createTextNode(quote.m_author));
    newnode.appendChild(a);

    if (!quote.m_book.isEmpty()) {
        QDomElement b = m_xmlDoc->createElement("book");
        b.appendChild(m_xmlDoc->createTextNode(quote.m_book));
        newnode.appendChild(b);
    }
    m_xmlDoc->documentElement().appendChild(newnode);
    m_nbQuotations++;
    m_modified = true;
    return m_nbQuotations - 1;
}


void QuotationManager::removeQuotation(int index)
{
    if (index >= m_nbQuotations)
        return;
    if (index < 0)
        return;

    QDomElement docElem = m_xmlDoc->documentElement();

    QDomNode n = docElem.firstChild();
    int num = 0;

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (num == index) {
            m_xmlDoc->documentElement().removeChild(n);
            m_nbQuotations--;
            m_modified = true;
            return;
        }
        num++;
        n = n.nextSibling();
    }
}




bool QuotationManager::isModified() const
{
    return m_modified;
}




QuotationManager::QuotationManager()
{
    m_xmlDoc = new QDomDocument("citations");
    m_modified = false;
}

QuotationManager::~QuotationManager()
{
    delete m_xmlDoc;
}
