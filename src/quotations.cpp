#include <QApplication>

#include "quotewidget.h"



int main(int argc, char *argv[])
{
    
    // Create the application object
	QApplication app(argc, argv);
	
	// Get the local language
	QString locale = QLocale::system().name();

	// Apply translation, depending on the local language
	// Supported languages are english and french
    QTranslator translator;
    if (translator.load(QString("quotes_") + locale)) {
        app.installTranslator(&translator);
    }

	// Create and show the main widget
	Quote cite;
	cite.show();
	
	// Start the event loop
	return app.exec();
}
