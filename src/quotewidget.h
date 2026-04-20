#ifndef QUOTEWIDGET_H_
#define QUOTEWIDGET_H_

#include <QtGui>
#include <QtXml>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QCheckBox>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>


#include "quotationmanager.h"

class RandomImageManager
{
public:
    QString getFileImage(const QString &dir);
    RandomImageManager();
private:
    QStringList list;
    void addFiles(const QString &dir);

    QString currentDir;
};

class QuoteSettingsWidget: public QWidget
{
	Q_OBJECT

	public:
	QuoteSettingsWidget();
	QLineEdit *temp; //!< Edit field for the template file name
	QLineEdit *sign; //!< Edit field for the signature file name
	QLineEdit *cite; //!< Edit field for the citation file name
    QLineEdit *picturesDir; //!< Edit field for the pictures directory name
	
	QString temp_name; //!< Name of the template signature file
	QString sign_name; //!< Name of the signature file to be generated
	QString cite_name; //!< Name of the file containing the citations
    QString picturesDir_name; //!< Name of the directory containing the pictures
	QPushButton *validatebutton; //!< Validation button

	void readSettings();
	void writeSettings();
public slots:
	void validate();
	void browseTemplate();
	void browseSign();
	void browseQuote();
    void browsePicturesDir();
};

class Quote: public QTabWidget
{
	Q_OBJECT
	
	public:
	Quote();
    ~Quote() override;
	
protected:

    RandomImageManager *m_imageManager{nullptr};

    QuotationManager *m_manager{nullptr};
    int m_curIndex{-1};

	enum {
		STATE_NORMAL=1,
		STATE_ADVANCED=2,
		STATE_NEW=3,
		STATE_UPDATE=4
	} m_state;

	QuoteSettingsWidget *settings; //!< Widget for the settings
	QPushButton *changebutton; //!< Button to randomly change the current quotation
	QPushButton *savequotesbutton; //!< Button to save the quotation list
	QPushButton *newbutton; //!< Button to create a new quotation
	QPushButton *addbutton; //!< Button to add the quotation to the list
	QPushButton *savebutton; //!< Button to save the new signature
	QPushButton *removebutton; //!< Button to remove the current quotation
	QPushButton *cancelbutton; //!< Button to cancel the current quotation modifications
	QPushButton *updatebutton; //!< Button to update the current quotation
    QPushButton *newImage; //!< Button to load a new random image
	QCheckBox *advanced; //!< Check box to display the advanced features

	QTextEdit *curText; //!< Text of the current quotation
	QLineEdit *curAuthor; //!< Author of the current quotation
	QLineEdit *curBook; //!< Book of the current quotation

    QGraphicsView *tabImage;
    QGraphicsPixmapItem *currentImage;

    void closeEvent(QCloseEvent *event) override;

	void setModified(bool modified);

	void updateMenu();
	
	public slots:
    void textEdited(const QString &text);
    void textChanged();
	void addQuotation(bool checked=false);
	void removeQuotation(bool checked=false);
	void newQuotation(bool checked=false);
	void changeAdvanced(int);	
	void saveQuotations(bool checked=false);
	void cancelUpdate(bool checked=false);
	void changeQuotation(bool checked=false);
	void updateQuotation(bool checked=false);
	void saveSignature(bool checked=false);
    void loadImage(bool checked=false);
};
#endif // QUOTEWIDGET_H_

