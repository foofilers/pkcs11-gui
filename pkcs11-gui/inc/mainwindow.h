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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <neither/either.hpp>
#include <p11core.h>
#include <QTableWidgetItem>
#include <QSettings>
#include "askpindialog.h"
#include "importp12dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void refreshReaders();
    void readerChanged(int index);
    void refreshPublicKeys();
    void loadPrivateKeys();
    void refreshCertificates();
    void userLogin(QString);
    void soLogin(QString);
    void refresh();
    void elementSelected(QTableWidgetItem*, QTableWidgetItem*);
    void deleteElement();
    void changePin();
    void logout();
    void saveElement();
    void generateKeyPairs(int, QString, long);
    void refreshDrivers();
    void initialize();
private:
    Ui::MainWindow *ui;
    P11Core *p11Core=nullptr;
    SmartCardReader *currentSmartCardReader;
    SmartCard *currentSmartCard;
    QVariant selectedElement;
    AskPINDialog* userAskPwdDlg;
    AskPINDialog* soAskPwdDlg;
    ImportP12Dialog *importP12Dialog;
    QSettings settings;
    void lockWithMessage(QString message, bool waitingCursor=true);
    void unlockWithMessage(QString message, bool restoreCursor=true);
};

#endif // MAINWINDOW_H
