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
#include "askpindialog.h"
#include "ui_askpindialog.h"

AskPINDialog::AskPINDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskPINDialog)
{
    ui->setupUi(this);
    connect(ui->actionBtns,SIGNAL(accepted()), this, SLOT(accepted()));
    connect(ui->actionBtns,SIGNAL(rejected()), this, SLOT(close()));
}

void AskPINDialog::accepted(){
    emit(this->passwordInserted(ui->passwordEdt->text()));
    this->close();
}

AskPINDialog::~AskPINDialog(){
    delete ui;
}
