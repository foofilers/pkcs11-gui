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
#ifndef IMPORTP12DIALOG_H
#define IMPORTP12DIALOG_H

#include <QDialog>
#include <smartcardreader.h>

namespace Ui {
class ImportP12Dialog;
}

class ImportP12Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportP12Dialog(QWidget *parent, SmartCardReader **smartCardReader);
    ~ImportP12Dialog();

public slots:
    void browseFile();
    void importP12();
private:
    Ui::ImportP12Dialog *ui;
    SmartCardReader **smartCardReader;
signals:
    void imported();


};

#endif // IMPORTP12DIALOG_H
