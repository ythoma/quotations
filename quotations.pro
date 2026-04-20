
TEMPLATE = app
TARGET = quotations

# Input
HEADERS += src/quotewidget.h src/quotationmanager.h
SOURCES += src/quotewidget.cpp src/quotations.cpp src/quotationmanager.cpp
TRANSLATIONS += quotes_fr.ts

QT += xml gui widgets

# Force l'utilisation de tr()
# DEFINES += QT_NO_CAST_FROM_ASCII

