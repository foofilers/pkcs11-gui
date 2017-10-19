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
#ifndef CHANGEPINDIALOG_H
#define CHANGEPINDIALOG_H

#include <QDialog>
#include <smartcardreader.h>

namespace Ui {
class ChangePinDialog;
}

class ChangePinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePinDialog(QWidget *, SmartCardReader *smartCardReader);
    ~ChangePinDialog();

public slots:
    void changePin();
private:
    Ui::ChangePinDialog *ui;
    SmartCardReader *smartCardReader;

};

#endif // CHANGEPINDIALOG_H
