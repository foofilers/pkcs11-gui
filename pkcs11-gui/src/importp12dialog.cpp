/*
 * Copyright 2017 Igor Maculan <n3wtron@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <importp12dialog.h>
#include <ui_importp12dialog.h>
#include <QShowEvent>

#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

ImportP12Dialog::ImportP12Dialog(QWidget *parent, SmartCardReader **smartCardReader) :
    QDialog(parent),
    ui(new Ui::ImportP12Dialog)
{
    ui->setupUi(this);
    this->smartCardReader = smartCardReader;

    connect(ui->browseBtn,SIGNAL(clicked()),this,SLOT(browseFile()));
    connect(ui->importBtn,SIGNAL(clicked()),this,SLOT(importP12()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(close()));

}

void ImportP12Dialog::browseFile(){
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open PEM Private Key"));
    ui->privKeyFilePathEdt->setText(fileName);
}

void ImportP12Dialog::importP12(){
    this->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (*(this->smartCardReader)!=nullptr){
        SmartCard* smCard = (*this->smartCardReader)->getSmartCard();
        if (smCard!=NULL){
            if (ui->privKeyFilePathEdt->text().isEmpty()){
                QMessageBox::critical(this,tr("Import Private Key"),tr("Cannot import private key without a selected file"));
                this->setEnabled(true);
                QApplication::restoreOverrideCursor();
                return;
            }
            QFile privKeyFile(ui->privKeyFilePathEdt->text());
            if (!privKeyFile.exists()){
                QMessageBox::critical(this,tr("Import Private Key"),tr("Cannot import private key. file does not exist"));
                this->setEnabled(true);
                QApplication::restoreOverrideCursor();
                return;
            }
            Either<QString, bool> result = smCard->storeP12(ui->privKeyFilePathEdt->text(),ui->passwordEdt->text(),ui->labelEdt->text(),ui->idEdt->text().toInt());
            if (result.isLeft){
                QMessageBox::critical(this,tr("Import Private Key"),result.leftValue);
            }else{
                emit(this->imported());

            }
        }
    }
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
}


ImportP12Dialog::~ImportP12Dialog()
{
    delete ui;
}
