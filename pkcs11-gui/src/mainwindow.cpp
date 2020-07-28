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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "changepindialog.h"
#include "keypairgeneratordialog.h"
#include "settingsdialog.h"
#include <licensedialog.h>

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    ui=new Ui::MainWindow();

    ui->setupUi(this);

    userAskPwdDlg = new AskPINDialog(this);
    soAskPwdDlg = new AskPINDialog(this);
    importP12Dialog = new ImportP12Dialog(this,&(this->currentSmartCardReader));
    auto keyPairDialog = new KeyPairGeneratorDialog(this);
    LicenseDialog *licenseDlg = new LicenseDialog(this);

    this->refreshDrivers();

    connect (ui->actionAbout_QT,&QAction::triggered,[this](){
        QApplication::aboutQt();
    });

    connect (ui->actionLicense,&QAction::triggered,[this,licenseDlg](){
        licenseDlg->show();
    });

    connect (ui->connectBtn,SIGNAL(clicked()),this,SLOT(initialize()));

    connect(ui->cmbReader,SIGNAL(currentIndexChanged(int)),this,SLOT(readerChanged(int)));

    connect(ui->userLoginBtn,SIGNAL(clicked()),userAskPwdDlg,SLOT(show()));
    connect(ui->soLoginBtn,SIGNAL(clicked()),soAskPwdDlg,SLOT(show()));

    connect(userAskPwdDlg,SIGNAL(passwordInserted(QString)),this,SLOT(userLogin(QString)));
    connect(soAskPwdDlg,SIGNAL(passwordInserted(QString)),this,SLOT(soLogin(QString)));

    connect(ui->actionRefresh,SIGNAL(triggered()),this,SLOT(refresh()));

    connect (ui->actionSettings,&QAction::triggered,[this](auto){
       SettingsDialog* dlg = new SettingsDialog(this);
       dlg->exec();
       if (this->p11Core==nullptr){
           this->refreshDrivers();
       }
    });

    connect(ui->actionImport_p12,&QAction::triggered,[this](){
        if (this->currentSmartCardReader!=nullptr && this->currentSmartCardReader->isLogged()){
            this->importP12Dialog->show();
        }else{
            QMessageBox::critical(this,tr("Import P12"),tr("Please login to import p12"));
        }
    });

    connect(ui->actiongenerateKeyPair,&QAction::triggered,[this,keyPairDialog](){
        if (this->currentSmartCardReader!=nullptr && this->currentSmartCardReader->isLogged()){
            keyPairDialog->show();
        }else{
            QMessageBox::critical(this,tr("Generate Key Pair"),tr("Please login to generate key pair"));
        }
    });

    connect(keyPairDialog,SIGNAL(generateKeyPair(int,QString,long)),this,SLOT(generateKeyPairs(int,QString,long)));

    connect(importP12Dialog,SIGNAL(imported()),this,SLOT(refresh()));

    connect(ui->smartcardContent,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this, SLOT(elementSelected(QTableWidgetItem*,QTableWidgetItem*)));

    connect (ui->deleteBtn,SIGNAL(clicked()),this,SLOT(deleteElement()));
    connect (ui->actionchangePin,SIGNAL(triggered()),this,SLOT(changePin()));
    connect (ui->saveBtn,SIGNAL(clicked()),this,SLOT(saveElement()));
    this->refreshDrivers();
}

void MainWindow::refreshDrivers(){
    ui->driverCmb->clear();
    QStringList drivers = this->settings.value("drivers").toStringList();
    ui->driverCmb->addItems(drivers);
}

void MainWindow::initialize(){
    if (!ui->driverCmb->currentText().isEmpty()){
        this->p11Core = new P11Core(this);

        this->p11Core->init(ui->driverCmb->currentText()).rightMap([this](auto){
            this->ui->cmbReader->setEnabled(true);
            this->ui->connectBtn->setEnabled(false);
            this->ui->driverCmb->setEnabled(false);
            ui->userLoginBtn->setEnabled(true);
            ui->soLoginBtn->setEnabled(true);
            ui->actionchangePin->setEnabled(true);
            ui->actiongenerateKeyPair->setEnabled(true);
            ui->actionImport_p12->setEnabled(true);
            this->refreshReaders();
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,tr("initialization error"), error);
            this->p11Core=nullptr;
            return 0;
        });
    }
}

void MainWindow::refreshReaders(){
    ui->cmbReader->clear();
    this->p11Core->loadSmartCardReaders().rightMap([this](auto readers){
        for (int i=0;i<readers.size();i++){
            SmartCardReader* smartCardReader = readers.at(i);
            QString present;
            if (smartCardReader->hasSmartCard()){
                present= tr("(with token)");
            }else{
                present= tr("(without token)");
            }
            this->ui->cmbReader->insertItem(i, QString(smartCardReader->getDescription())+present);
        }
        return 0;
    }).leftMap([this](auto error){
        QMessageBox::critical(this,tr("Error getting readers"),error);
        return 0;
    });


}

void MainWindow::readerChanged(int index){
    elementSelected(nullptr,nullptr);

    if (index>-1){
        SmartCardReader* selectedSmartCardReader = this->p11Core->getSmartCardReaders().at(index);
        qDebug()<<"Selected slot"<<selectedSmartCardReader->getDescription();
        this->currentSmartCardReader=selectedSmartCardReader;
        if (this->currentSmartCardReader->hasSmartCard()){
            this->currentSmartCard = this->currentSmartCardReader->getSmartCard();
        }else{
            this->currentSmartCard = nullptr;
        }
    }else{
        this->currentSmartCardReader=nullptr;
        this->currentSmartCard = nullptr;
    }
    this->refresh();
}

void MainWindow::lockWithMessage(QString message,bool waitingCursor){
    setEnabled(false);
    if (waitingCursor){
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    statusBar()->showMessage(message);
}

void MainWindow::unlockWithMessage(QString message, bool restoreCursor){
    setEnabled(true);
    if (restoreCursor){
        QApplication::restoreOverrideCursor();
    }
    statusBar()->showMessage(message);
}


void MainWindow::refresh(){
    lockWithMessage("Refreshing");
    ui->smartcardContent->clearContents();
    ui->smartcardContent->setRowCount(0);
    this->selectedElement.clear();
    if (this->currentSmartCardReader!=nullptr){
        refreshPublicKeys();
        refreshCertificates();
        if(this->currentSmartCardReader->isLogged()){
            loadPrivateKeys();
        }
    }
    unlockWithMessage("");
}

void MainWindow::refreshPublicKeys(){
    if (this->currentSmartCard!=NULL){
        this->currentSmartCard->getPublicKeys().rightMap([this](auto publicKeys){
            int currRows=ui->smartcardContent->rowCount();
            ui->smartcardContent->setRowCount(currRows+publicKeys.size());
            for (int i=0;i<publicKeys.size();i++){
                Key* key=publicKeys.at(i);
                QTableWidgetItem *item=new QTableWidgetItem(QIcon(":/icons/pub_key"),QString("Public Key"));
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,0,item);
                item=new QTableWidgetItem(key->getId());
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,1,item);
                item = new QTableWidgetItem(key->getLabel());
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,2,item);
            }
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,error,error);
            return 0;
        });
    }
}

void MainWindow::refreshCertificates(){
    if (this->currentSmartCard!=NULL){
        this->currentSmartCard->getCertificates().rightMap([this](auto certificates){
            int currRows=ui->smartcardContent->rowCount();
            ui->smartcardContent->setRowCount(currRows+certificates.size());
            for (int i=0;i<certificates.size();i++){
                Certificate* cert=certificates.at(i);
                QTableWidgetItem *item=new QTableWidgetItem(QIcon(":/icons/certificate"),QString("Certificate"));
                item->setData(Qt::UserRole,qVariantFromValue(cert));
                ui->smartcardContent->setItem(currRows+i,0,item);
                item=new QTableWidgetItem(cert->getId());
                item->setData(Qt::UserRole,qVariantFromValue(cert));
                ui->smartcardContent->setItem(currRows+i,1,item);
                item = new QTableWidgetItem(cert->getLabel());
                item->setData(Qt::UserRole,qVariantFromValue(cert));
                ui->smartcardContent->setItem(currRows+i,2,item);
            }
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,error,error);
            return 0;
        });
    }
}

void MainWindow::loadPrivateKeys(){
    if (this->currentSmartCard!=nullptr){
        this->currentSmartCard->getPrivateKeys().rightMap([this](auto privKeys){
            int currRows=ui->smartcardContent->rowCount();
            ui->smartcardContent->setRowCount(currRows+privKeys.size());
            for (int i=0;i<privKeys.size();i++){
                Key* key=privKeys.at(i);
                QTableWidgetItem *item=new QTableWidgetItem(QIcon(":/icons/priv_key"),QString("Private Key"));
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,0,item);
                item=new QTableWidgetItem(key->getId());
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,1,item);
                item = new QTableWidgetItem(key->getLabel());
                item->setData(Qt::UserRole,qVariantFromValue(key));
                ui->smartcardContent->setItem(currRows+i,2,item);
            }
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,error,error);
            return 0;
        });
    }
}

void MainWindow::userLogin(QString password){
    if (this->currentSmartCardReader!=NULL && this->currentSmartCardReader->hasSmartCard()){
        if(!this->currentSmartCardReader->isUserLogged()){
            if (this->currentSmartCardReader->userLogin(password)){
                ui->userLoginBtn->setIcon(QIcon(":/icons/unlock"));
                ui->userLoginBtn->setText("User Logout");
                ui->soLoginBtn->setEnabled(false);
                disconnect(ui->userLoginBtn,SIGNAL(clicked()),userAskPwdDlg,SLOT(show()));
                disconnect(ui->soLoginBtn,SIGNAL(clicked()),soAskPwdDlg,SLOT(show()));
                connect(ui->userLoginBtn,SIGNAL(clicked()),this,SLOT(logout()));
                refresh();
            }else{
                QMessageBox::critical(this,"Error during login","Login failed");
            }
        }
    }
}

void MainWindow::soLogin(QString password){
    if (this->currentSmartCardReader!=NULL && this->currentSmartCardReader->hasSmartCard() ){
        if (!this->currentSmartCardReader->isSoLogged()){
            if (this->currentSmartCardReader->soLogin(password)){
                ui->soLoginBtn->setIcon(QIcon(":/icons/unlock"));
                ui->soLoginBtn->setText("SO Logout");
                ui->userLoginBtn->setEnabled(false);
                disconnect(ui->userLoginBtn,SIGNAL(clicked()),userAskPwdDlg,SLOT(show()));
                disconnect(ui->soLoginBtn,SIGNAL(clicked()),soAskPwdDlg,SLOT(show()));
                connect(ui->soLoginBtn,SIGNAL(clicked()),this,SLOT(logout()));
                refresh();
            }else{
                QMessageBox::critical(this,"Error during login","Login failed");
            }
        }
    }
}

void MainWindow::logout(){
    this->currentSmartCardReader->logout();
    ui->userLoginBtn->setEnabled(true);
    ui->soLoginBtn->setIcon(QIcon(":/icons/lock"));
    ui->soLoginBtn->setText("SO Login");

    ui->soLoginBtn->setEnabled(true);
    ui->userLoginBtn->setIcon(QIcon(":/icons/lock"));
    ui->userLoginBtn->setText("User Login");
    connect(ui->userLoginBtn,SIGNAL(clicked()),userAskPwdDlg,SLOT(show()));
    connect(ui->soLoginBtn,SIGNAL(clicked()),soAskPwdDlg,SLOT(show()));
    refresh();
}

void MainWindow::elementSelected(QTableWidgetItem* newItem ,QTableWidgetItem*){
    ui->deleteBtn->setEnabled(false);
    ui->deleteBtn->setText(tr("Delete"));
    ui->saveBtn->setEnabled(false);
    ui->saveBtn->setText(tr("Save"));
    ui->detailText->setText("");
    if (newItem!=nullptr){
        QVariant itemData = newItem->data(Qt::UserRole);
        this->selectedElement = itemData;
        if (itemData.isValid() && !itemData.isNull()){
            if (itemData.canConvert<Key*>()==true){
                ui->deleteBtn->setEnabled(this->currentSmartCardReader->isLogged());
                ui->deleteBtn->setText("Delete Key");
                ui->saveBtn->setText(tr("Save Key"));
                ui->saveBtn->setEnabled(true);
                itemData.value<Key*>()->getDetails().rightMap([this](auto certDesc){
                    ui->detailText->setText(certDesc);
                    return 0;
                }).leftMap([this](auto error){
                    QMessageBox::critical(this,tr("Cannot read details"),error);
                    return 0;
                });
            }
            if (itemData.canConvert<Certificate*>()==true){
                ui->deleteBtn->setEnabled(this->currentSmartCardReader->isLogged());
                ui->deleteBtn->setText("Delete Certificate");
                ui->saveBtn->setText(tr("Save Certificate"));
                ui->saveBtn->setEnabled(true);
                itemData.value<Certificate*>()->getDetails().rightMap([this](auto certDesc){
                    ui->detailText->setText(certDesc);
                    return 0;
                }).leftMap([this](auto error){
                    QMessageBox::critical(this,tr("Cannot read details"),error);
                    return 0;
                });
            }
        }
    }else{
        this->selectedElement.clear();
    }
}

void MainWindow::deleteElement(){
    if (selectedElement.canConvert<Key*>()){
        auto response = QMessageBox::question(this,tr("Delete Key"),tr("Are you sure to delete the selected key?"),QMessageBox::Yes,QMessageBox::Cancel);

        if (response == QMessageBox::Yes){
            lockWithMessage("Deleting key");
            selectedElement.value<Key*>()->deleteKey().rightMap([this](auto){
                this->currentSmartCardReader->relogin();
                this->refresh();
                return 0;
            }).leftMap([this](auto error){
                QMessageBox::critical(this,"Error deleting key",error);
                return 0;
            });
        }
    }
    if (selectedElement.canConvert<Certificate*>()){
        auto response = QMessageBox::question(this,tr("Delete Certificate"),tr("Are you sure to delete the selected certificate?"),QMessageBox::Yes,QMessageBox::Cancel);

        if (response == QMessageBox::Yes){
            lockWithMessage("Deleting certificate");
            selectedElement.value<Certificate*>()->deleteCertificate().rightMap([this](auto){
                this->currentSmartCardReader->relogin();
                this->refresh();
                return 0;
            }).leftMap([this](auto error){
                QMessageBox::critical(this,"Error deleting certificate",error);
                return 0;
            });
        }
    }
    unlockWithMessage("");
}

void MainWindow::saveElement(){
    if (selectedElement.canConvert<Certificate*>()){
        QString pemFilePath=QFileDialog::getSaveFileName(this,tr("Save Certificate"),
                                                         QDir::homePath(),
                                                         tr("Certificate(*.cert *.crt)"));
        if (!pemFilePath.isEmpty()){
            selectedElement.value<Certificate*>()->savePEM(pemFilePath).rightMap([this,pemFilePath](auto){
                this->statusBar()->showMessage(tr("Certificate saved %1").arg(pemFilePath));
                return 0;
            }).leftMap([this](auto error){
                QMessageBox::critical(this,"Error saving certificate",error);
                return 0;
            });
        }
    }
    if (selectedElement.canConvert<Key*>()){
        Key* key=selectedElement.value<Key*>();
        QString pemFilePath;
        if (key->isPrivate()){
            pemFilePath=QFileDialog::getSaveFileName(this,tr("Save Private Key"),
                                                     QDir::homePath(),
                                                     tr("Private Key(*.key)"));
        }else{
            pemFilePath=QFileDialog::getSaveFileName(this,tr("Save Public Key"),
                                                     QDir::homePath(),
                                                     tr("Public Key(*.key)"));
        }
        if (!pemFilePath.isEmpty()){
            key->savePEM(pemFilePath).rightMap([this,pemFilePath](auto){
                this->statusBar()->showMessage(tr("Key saved %1").arg(pemFilePath));
                return 0;
            }).leftMap([this](auto error){
                QMessageBox::critical(this,"Error saving key",error);
                return 0;
            });
        }
    }
}

void MainWindow::changePin(){
    if (this->currentSmartCardReader==nullptr){
        QMessageBox::critical(this,tr("ChangePin failed"),tr("Cannot find any smartcardReader"));
        return;
    }
    if (!this->currentSmartCardReader->hasSmartCard()){
        QMessageBox::critical(this,tr("ChangePin failed"),tr("No smartcard found in the reader"));
        return;
    }
    if (!this->currentSmartCardReader->isLogged()){
        QMessageBox::critical(this,tr("ChangePin failed"),tr("Not logged"));
        return;
    }
    auto changePinDlg = new ChangePinDialog(this,this->currentSmartCardReader);
    changePinDlg->show();
}

void MainWindow::generateKeyPairs(int id, QString label, long modulus){
    if (this->currentSmartCard!=nullptr){
        lockWithMessage("Generating key pair, will take a while...");
        this->currentSmartCard->generateKeyPairs(id,label,modulus).rightMap([this](auto){
            this->refresh();
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,"Error saving key",error);
            return 0;
        });
        unlockWithMessage("");
    }
}

MainWindow::~MainWindow(){
    delete ui;
}
