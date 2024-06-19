#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open XML File"), "", tr("XML Files (*.xml)"));
    if (!fileName.isEmpty()) {
        loadXmlFile(fileName);
    }
}

void MainWindow::loadXmlFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            ui->filePathLineEdit->setText(fileName);
            ui->fileContentTextEdit->setPlainText(formatDomElement(doc.documentElement()));

            // Загрузка списка PartNumber из выбранного файла
            QDomNodeList items = doc.elementsByTagName("Item");
            ui->partNumberComboBox->clear();
            for (int i = 0; i < items.size(); i++) {
                QDomElement item = items.at(i).toElement();
                QString partNumber = item.attribute("PartNumber");
                ui->partNumberComboBox->addItem(partNumber);
            }
            filename_ = fileName;
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Invalid XML file"));
        }
        file.close();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
    }
}

QString MainWindow::formatDomElement(const QDomElement &element, int indent)
{
    QString result;
    QString indentString(indent, ' ');
    result += indentString + "<" + element.tagName();
    QDomNamedNodeMap attributes = element.attributes();
    for (int i = 0; i < attributes.count(); ++i) {
        QDomAttr attr = attributes.item(i).toAttr();
        result += " " + attr.name() + "=\"" + attr.value() + "\"";
    }
    result += ">\n";
    QDomNodeList children = element.childNodes();
    for (int i = 0; i < children.count(); ++i) {
        QDomNode child = children.at(i);
        if (child.isElement()) {
            result += formatDomElement(child.toElement(), indent + 2);
        } else if (child.isText()) {
            result += indentString + "  " + child.toText().data() + "\n";
        }
    }
    result += indentString + "</" + element.tagName() + ">\n";
    return result;
}

void MainWindow::on_deleteItemButton_clicked()
{
    QString partNumber = ui->partNumberComboBox->currentText();
    if (!partNumber.isEmpty()) {
        QDomElement root = doc.documentElement();
        QDomNodeList items = root.elementsByTagName("Item");
        for (int i = 0; i < items.size(); i++) {
            QDomElement item = items.at(i).toElement();
            if (item.attribute("PartNumber") == partNumber) {
                item.parentNode().removeChild(item);
                break;
            }
        }

        ui->fileContentTextEdit->setPlainText(formatDomElement(doc.documentElement()));
        ui->partNumberComboBox->removeItem(ui->partNumberComboBox->currentIndex());
    }
}

void MainWindow::on_addItemButton_clicked()
{
    QString partNumber = QInputDialog::getText(this, tr("Add Item"), tr("Enter PartNumber:"));
    if (!partNumber.isEmpty()) {
        QFileInfo fileInfo(filename_);
        QString fileDirPath = fileInfo.dir().absolutePath();
        QString libraryFilePath = fileDirPath + "/ItemLibrary.xml";
        QFile libraryFile(libraryFilePath);
        if (libraryFile.open(QIODevice::ReadOnly)) {
            QXmlStreamReader reader(&libraryFile);
            bool itemFound = false;
            QDomElement itemElement;
            while (!reader.atEnd() && !reader.hasError()) {
                reader.readNext();
                if (reader.isStartElement() && reader.name() == QString("Item")) {
                    if (reader.attributes().value("PartNumber").toString() == partNumber) {
                        itemElement = doc.createElement("Item");
                        itemElement.setAttribute("PartNumber", partNumber);
                        while (reader.readNextStartElement()) {
                            if (reader.name() == QString("ProductName") || reader.name() == QString("Quantity") || reader.name() == QString("USPrice")) {
                                QDomElement element = doc.createElement(reader.name().toString());
                                element.appendChild(doc.createTextNode(reader.readElementText()));
                                itemElement.appendChild(element);
                            } else {
                                reader.skipCurrentElement();
                            }
                        }
                        itemFound = true;
                        break;
                    }
                }
            }
            libraryFile.close();

            if (itemFound) {
                QDomElement root = doc.documentElement();
                root.appendChild(itemElement);
                ui->fileContentTextEdit->setPlainText(formatDomElement(doc.documentElement()));
                ui->partNumberComboBox->addItem(partNumber);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Item with PartNumber not found in library file"));
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open library file: ") + libraryFile.errorString());
        }
    }
}

void MainWindow::on_saveFileButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save XML File"), "", tr("XML Files (*.xml)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << doc.toString();
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
        }
    }
}
