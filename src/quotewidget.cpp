#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include "quotewidget.h"

RandomImageManager::RandomImageManager()
{
    currentDir = "";
}

QString RandomImageManager::getFileImage(const QString& dir)
{
    if (dir != currentDir) {
        list.clear();
        addFiles(dir);
        currentDir = dir;
    }
    int size = list.size();
    if (size == 0) {
        return "";
    }
    int pos = QRandomGenerator::global()->bounded(size);
    return list.at(pos);
}

void RandomImageManager::addFiles(const QString& dir)
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();

    QStringList files = QDir(dir).entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    foreach (QString f, files) {
        QFileInfo info(dir + "/" + f);
        if (info.isDir()) {
            addFiles(dir + "/" + f);
        }
        else if (info.isFile()) {
            QString theext = info.completeSuffix();
            foreach (QByteArray ext, formats)
                if (theext.compare(ext) == 0)
                    list.append(dir + "/" + f);
        }
    }
}

/*! Randomly changes the current quotation
 * This function uses the random number generator and updates the current 
 * quotation with the randomly generated.
*/
void Quote::changeQuotation(bool)
{
    Quotation quote;
    if ((m_curIndex = m_manager->randomQuote(&quote)) == -1) {
        return;
    }
    QObject::disconnect(curText->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
    curText->setPlainText(quote.m_text);
    QObject::connect(curText->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
    curAuthor->setText(quote.m_author);
    curBook->setText(quote.m_book);
    changebutton->show();
    cancelbutton->hide();
}


void Quote::addQuotation(bool)
{
    Quotation quote;
    quote.m_text = curText->toPlainText();
    quote.m_author = curAuthor->text();
    quote.m_book = curBook->text();
    m_curIndex = m_manager->addQuotation(quote);
    setModified(true);

    if (advanced->checkState() == Qt::Checked) {
        m_state = STATE_ADVANCED;
    }
    else {
        m_state = STATE_NORMAL;
    }
    updateMenu();
}

void Quote::newQuotation(bool)
{
    QObject::disconnect(curText->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
    curText->clear();
    QObject::connect(curText->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
    curAuthor->clear();
    curBook->clear();

    m_state = STATE_NEW;
    updateMenu();
}


void Quote::removeQuotation(bool)
{
    if (m_curIndex != -1) {
        if (QMessageBox::question(
                    this,
                    tr("Warning"),
                    tr("Are you sure you want to remove this quote?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No)
            == QMessageBox::Yes) {
            m_manager->removeQuotation(m_curIndex);
            setModified(true);
        }
    }
    changeQuotation();
}


void Quote::saveSignature(bool)
{
    QFile file(settings->temp_name);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open the template file"));
        return;
    }
    QString s = file.readAll();
    file.close();
    s.replace("{quote}", curText->toPlainText());
    s.replace("{author}", curAuthor->text());
    QString b;
    if (!curBook->text().isEmpty()) {
        b = tr("in ");
        b += curBook->text();
    }
    s.replace("{book}", b);

    QFile fileo(settings->sign_name);
    if (!fileo.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create the signature file"));
        return;
    }
    fileo.write(s.toUtf8());

    fileo.close();
}

void Quote::cancelUpdate(bool)
{
    if (advanced->checkState() == Qt::Checked) {
        m_state = STATE_ADVANCED;
    }
    else {
        m_state = STATE_NORMAL;
    }
    changeQuotation();
    updateMenu();
}

void Quote::setModified(bool modified)
{
    if (modified) {
        setWindowTitle(tr("Quotation manager*"));
    }
    else {
        setWindowTitle(tr("Quotation manager"));
    }
}

void Quote::textEdited(const QString& /*text*/)
{
    textChanged();
}

void Quote::textChanged()
{
    setModified(true);
    if (m_state != STATE_NEW) {
        m_state = STATE_UPDATE;
    }
    updateMenu();
}

void Quote::updateQuotation(bool)
{
    if (advanced->checkState() == Qt::Checked) {
        m_state = STATE_ADVANCED;
    }
    else {
        m_state = STATE_NORMAL;
    }

    Quotation quote;
    quote.m_text = curText->toPlainText();
    quote.m_author = curAuthor->text();
    quote.m_book = curBook->text();
    m_curIndex = m_manager->replaceQuotation(m_curIndex, quote);


    updateMenu();
}

void Quote::updateMenu()
{
    switch (m_state) {
    case STATE_NORMAL:

        updatebutton->hide();
        cancelbutton->hide();
        removebutton->hide();
        addbutton->hide();
        newbutton->hide();
        savequotesbutton->hide();
        changebutton->show();
        savebutton->show();

        break;
    case STATE_UPDATE:
        updatebutton->show();
        cancelbutton->show();
        removebutton->hide();
        addbutton->hide();
        newbutton->hide();
        savequotesbutton->hide();
        changebutton->hide();
        savebutton->hide();
        break;
    case STATE_NEW:
        cancelbutton->show();
        addbutton->show();
        updatebutton->hide();
        removebutton->hide();
        newbutton->hide();
        savequotesbutton->hide();
        changebutton->hide();
        savebutton->hide();

        break;
    case STATE_ADVANCED:

        updatebutton->hide();
        cancelbutton->hide();
        removebutton->show();
        addbutton->hide();
        newbutton->show();
        savequotesbutton->show();
        changebutton->show();
        savebutton->show();

        break;
    }
}

Quote::Quote()
{
    currentImage = nullptr;
    m_state = STATE_NORMAL;
    setModified(false);
    QWidget* standard = new QWidget();
    tabImage = new QGraphicsView();
    tabImage->setScene(new QGraphicsScene(this));
    settings = new QuoteSettingsWidget();
    m_manager = new QuotationManager();
    curText = new QTextEdit();
    curAuthor = new QLineEdit();
    curBook = new QLineEdit();

    QHBoxLayout* first = new QHBoxLayout(standard);

    QGridLayout* grid1 = new QGridLayout();
    QVBoxLayout* grid2 = new QVBoxLayout();

    first->addLayout(grid1);
    first->addLayout(grid2);
    first->addWidget(tabImage);

    grid1->addWidget(new QLabel(tr("Quotation")), 0, 0);
    grid1->addWidget(new QLabel(tr("Author")), 1, 0);
    grid1->addWidget(new QLabel(tr("Book")), 2, 0);
    grid1->addWidget(curText, 0, 1);
    grid1->addWidget(curAuthor, 1, 1);
    grid1->addWidget(curBook, 2, 1);
    curText->setMinimumWidth(200);

    QObject::connect(curText->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
    QObject::connect(curAuthor, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
    QObject::connect(curBook, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));

    savebutton = new QPushButton(tr("Update signature"));
    grid2->addWidget(savebutton);
    QObject::connect(savebutton, SIGNAL(clicked(bool)), this, SLOT(saveSignature(bool)));

    changebutton = new QPushButton(tr("Another quotation"));
    grid2->addWidget(changebutton);
    QObject::connect(changebutton, SIGNAL(clicked(bool)), this, SLOT(changeQuotation(bool)));

    savequotesbutton = new QPushButton(tr("Save quotations file"));
    grid2->addWidget(savequotesbutton);
    QObject::connect(savequotesbutton, SIGNAL(clicked(bool)), this, SLOT(saveQuotations(bool)));

    newbutton = new QPushButton(tr("New quotation"));
    grid2->addWidget(newbutton);
    QObject::connect(newbutton, SIGNAL(clicked(bool)), this, SLOT(newQuotation(bool)));

    addbutton = new QPushButton(tr("Add quotation"));
    grid2->addWidget(addbutton);
    QObject::connect(addbutton, SIGNAL(clicked(bool)), this, SLOT(addQuotation(bool)));

    updatebutton = new QPushButton(tr("Update quotation"));
    grid2->addWidget(updatebutton);
    QObject::connect(updatebutton, SIGNAL(clicked(bool)), this, SLOT(updateQuotation(bool)));

    cancelbutton = new QPushButton(tr("Cancel"));
    grid2->addWidget(cancelbutton);
    QObject::connect(cancelbutton, SIGNAL(clicked(bool)), this, SLOT(cancelUpdate(bool)));

    removebutton = new QPushButton(tr("Remove quotation"));
    grid2->addWidget(removebutton);
    QObject::connect(removebutton, SIGNAL(clicked(bool)), this, SLOT(removeQuotation(bool)));

    advanced = new QCheckBox(tr("Advanced"));
    grid2->addWidget(advanced);
    QObject::connect(advanced, SIGNAL(stateChanged(int)), this, SLOT(changeAdvanced(int)));

    newImage = new QPushButton(tr("New image"));
    grid2->addWidget(newImage);
    QObject::connect(newImage, SIGNAL(clicked(bool)), this, SLOT(loadImage(bool)));

    addTab(standard, tr("Quotations"));
    addTab(settings, tr("Settings"));
    tabImage->setMinimumWidth(620);

    m_imageManager = new RandomImageManager();

    loadImage();

    if (m_manager->loadFile(settings->cite_name)) {
        changeQuotation();
    }
    updateMenu();
}

Quote::~Quote()
{
    delete m_imageManager;
    delete m_manager;
}

void Quote::loadImage(bool)
{
    if (!settings->picturesDir_name.isEmpty()) {
        QString fileImage = m_imageManager->getFileImage(settings->picturesDir_name);

        // Using a QImageReader allows to deal with the EXIF rotation
        // thanks to auto transform
        QImageReader reader(fileImage);
        reader.setAutoTransform(true);

        QPixmap pix = QPixmap::fromImageReader(&reader);
        QPixmap p = pix.scaledToWidth(std::min(pix.width(), 600));
        QPixmap p2 = p.scaledToHeight(std::min(400, p.height()));

        if (currentImage)
            tabImage->scene()->removeItem(currentImage);
        currentImage = tabImage->scene()->addPixmap(p2);
    }
}
/*! \brief This function allows to display or not the advanced options
 *  If the check box "advanced" is checked, then the following buttons
 *  are displayed:
 *      - To remove the current quotations
 *      - To create a new quotation
 *      - To add the newly created quotation to the list
 *      - To save the quotation file
 *  If the check box is unchecked, then the buttons are hidden
*/
void Quote::changeAdvanced(int state)
{
    if (state == Qt::Checked) {
        m_state = STATE_ADVANCED;
    }
    else {
        m_state = STATE_NORMAL;
    }
    updateMenu();
}

void Quote::saveQuotations(bool)
{
    m_manager->saveFile(settings->cite_name);
    setModified(false);
}


void Quote::closeEvent(QCloseEvent* event)
{
    if (m_manager->isModified()) {
        if (QMessageBox::question(
                    this,
                    QObject::tr("Save file"),
                    QObject::tr("The quotation list has been modified."
                                "Do you want to save the modifications?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes)
            == QMessageBox::Yes) {
            saveQuotations();
            event->accept();
        }
        else {
            if (QMessageBox::question(
                        this,
                        QObject::tr("Warning"),
                        QObject::tr("Are you sure you want to quit without saving the file?"),
                        QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::Yes)
                == QMessageBox::Yes) {
                event->accept();
                return;
            }
            else {
                event->ignore();
                return;
            }
        }
    }
    event->accept();
}

















QuoteSettingsWidget::QuoteSettingsWidget()
{
    QGridLayout* layout = new QGridLayout(this);
    temp = new QLineEdit();
    sign = new QLineEdit();
    cite = new QLineEdit();
    picturesDir = new QLineEdit();
    layout->addWidget(temp, 0, 1);
    layout->addWidget(sign, 1, 1);
    layout->addWidget(cite, 2, 1);
    layout->addWidget(picturesDir, 3, 1);

    layout->addWidget(new QLabel(tr("Template file")), 0, 0, Qt::AlignRight);
    layout->addWidget(new QLabel(tr("Signature file")), 1, 0, Qt::AlignRight);
    layout->addWidget(new QLabel(tr("Quotations file")), 2, 0, Qt::AlignRight);
    layout->addWidget(new QLabel(tr("Pictures dir")), 3, 0, Qt::AlignRight);

    validatebutton = new QPushButton(tr("Save settings"));
    layout->addWidget(validatebutton, 4, 0);

    QObject::connect(validatebutton, SIGNAL(pressed()), this, SLOT(validate()));

    QPushButton* browse0 = new QPushButton(tr("Browse"));
    layout->addWidget(browse0, 0, 2);
    QObject::connect(browse0, SIGNAL(pressed()), this, SLOT(browseTemplate()));
    QPushButton* browse1 = new QPushButton(tr("Browse"));
    layout->addWidget(browse1, 1, 2);
    QObject::connect(browse1, SIGNAL(pressed()), this, SLOT(browseSign()));
    QPushButton* browse2 = new QPushButton(tr("Browse"));
    layout->addWidget(browse2, 2, 2);
    QObject::connect(browse2, SIGNAL(pressed()), this, SLOT(browseQuote()));

    QPushButton* browse3 = new QPushButton(tr("Browse"));
    layout->addWidget(browse3, 3, 2);
    QObject::connect(browse3, SIGNAL(pressed()), this, SLOT(browsePicturesDir()));

    readSettings();

    temp->setText(temp_name);
    sign->setText(sign_name);
    cite->setText(cite_name);
    picturesDir->setText(picturesDir_name);
}

/*! \brief Opens a dialog to choose the template signature file
 * If the selected file is validated, then the edit field of the
 * current widget is modified with the value.
*/
void QuoteSettingsWidget::browseTemplate()
{
    QString s = QFileDialog::getOpenFileName(this, tr("Choose a template file"), temp_name, tr("Text (*.txt)"));
    if (!s.isEmpty()) {
        temp->setText(s);
    }
}

void QuoteSettingsWidget::browseSign()
{
    QString s = QFileDialog::getSaveFileName(this, tr("Choose a signature file"), sign_name, tr("Text (*.txt)"));
    if (!s.isEmpty()) {
        sign->setText(s);
    }
}

void QuoteSettingsWidget::browseQuote()
{
    QString s = QFileDialog::getOpenFileName(
            this, tr("Choose a quotation file"), cite_name, tr("Text or xml (*.txt *.xml)"));
    if (!s.isEmpty()) {
        cite->setText(s);
    }
}

void QuoteSettingsWidget::browsePicturesDir()
{
    QString s = QFileDialog::getExistingDirectory(
            this,
            tr("Choose a directory"),
            picturesDir_name,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!s.isEmpty()) {
        picturesDir->setText(s);
    }
}

void QuoteSettingsWidget::validate()
{
    temp_name = temp->text();
    sign_name = sign->text();
    cite_name = cite->text();
    picturesDir_name = picturesDir->text();

    writeSettings();
}

void QuoteSettingsWidget::readSettings()
{
    QSettings settings("Thoma_soft", "Citations");
    temp_name = settings.value("template_file", "").toString();
    sign_name = settings.value("signature_file", "").toString();
    cite_name = settings.value("quotation_file", "").toString();
    picturesDir_name = settings.value("pictures_dir", "").toString();
}

void QuoteSettingsWidget::writeSettings()
{
    QMessageBox::information(this, tr("write settings"), tr("writing settings"));
    QSettings settings("Thoma_soft", "Citations");
    settings.setValue("template_file", temp_name);
    settings.setValue("signature_file", sign_name);
    settings.setValue("quotation_file", cite_name);
    settings.setValue("pictures_dir", picturesDir_name);
}
