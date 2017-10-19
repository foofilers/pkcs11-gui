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
#include "inc/changepindialog.h"
#include "ui_changepindialog.h"
#include <QMessageBox>
#include <QIntValidator>

ChangePinDialog::ChangePinDialog(QWidget *parent, SmartCardReader *smartCardReader) :
    QDialog(parent),
    ui(new Ui::ChangePinDialog){

    ui->setupUi(this);

    this->smartCardReader = smartCardReader;
    if (!this->smartCardReader->isUserLogged()){
        ui->currentPinEdt->setEnabled(false);
    }
    connect(ui->actions,SIGNAL(accepted()),this,SLOT(changePin()));
}

void ChangePinDialog::changePin(){
    if (ui->newPinEdt->text()!=ui->newPinConfirmEdt->text()){
        QMessageBox::critical(this,tr("ChangePin failed"),tr("The verify pin doesn't match"));
        return;
    }
    if (this->smartCardReader->isUserLogged()){
        this->smartCardReader->changePin(ui->currentPinEdt->text(), ui->newPinEdt->text()).rightMap([this](auto){
            QMessageBox::information(this,tr("ChangePin"),tr("Pin changed"));
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,tr("ChangePin failed"),error);
            return 0;
        });
    }else{
        this->smartCardReader->getSmartCard()->setPin(ui->newPinEdt->text()).rightMap([this](auto){
            QMessageBox::information(this,tr("ChangePin"),tr("Pin changed"));
            return 0;
        }).leftMap([this](auto error){
            QMessageBox::critical(this,tr("ChangePin failed"),error);
            return 0;
        });
    }
}

ChangePinDialog::~ChangePinDialog(){
    delete ui;
}
