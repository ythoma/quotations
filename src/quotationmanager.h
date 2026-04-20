#ifndef QUOTATIONMANAGER_H_
#define QUOTATIONMANAGER_H_


#include <QObject>
#include <QString>
#include <QtXml>


struct Quotation
{
    QString m_text;   //!< Text of the quotation
    QString m_author; //!< Author of the quotation
    QString m_book;   //!< Book of the quotation
};

class QuotationManager
{

public:
    QuotationManager();
    virtual ~QuotationManager();

    bool isModified() const;

    int addQuotation(const Quotation& quote);
    void removeQuotation(int index);
    int replaceQuotation(int index, const Quotation& quote);

    int randomQuote(Quotation* quote) const;
    bool getQuote(int index, Quotation* quote) const;

    bool loadFile(const QString& fileName);
    bool saveFile(const QString& fileName);


protected:
    QDomDocument* m_xmlDoc{nullptr}; //!< Xml document containing the quotations list
    int m_nbQuotations{0};           //!< Number of quotations
    bool m_modified{false};          //!< Has the list been modified?

    bool quoteFromNode(const QDomNode& node, Quotation* quote) const;
};


#endif // QUOTATIONMANAGER_H_
