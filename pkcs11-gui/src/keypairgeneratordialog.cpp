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
#include "inc/keypairgeneratordialog.h"
#include "ui_keypairgeneratordialog.h"

KeyPairGeneratorDialog::KeyPairGeneratorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyPairGeneratorDialog){
    ui->setupUi(this);
    ui->idEdt->setValidator(new QIntValidator);
    connect(ui->actions, &QDialogButtonBox::accepted, [this](){
       emit(this->generateKeyPair(ui->idEdt->text().toInt(),ui->labelEdt->text(),ui->modulusCmb->currentText().toLong()));
    });
}

KeyPairGeneratorDialog::~KeyPairGeneratorDialog()
{
    delete ui;
}
