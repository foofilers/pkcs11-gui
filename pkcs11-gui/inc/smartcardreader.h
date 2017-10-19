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
#ifndef SMARTCARDREADER_H
#define SMARTCARDREADER_H

#include <QObject>
#include <neither/either.hpp>
#include <libp11.h>
#include <smartcard.h>

class SmartCardReader : public QObject
{
    Q_OBJECT
public:
    explicit SmartCardReader(QObject *parent, PKCS11_slot_st *slot);
    QString getDescription();
    bool hasSmartCard();
    SmartCard* getSmartCard();
    bool userLogin(QString password);
    bool soLogin(QString password);
    void logout();
    bool isUserLogged();
    bool isSoLogged();
    bool isLogged();
    void relogin();
    Either<QString,bool> changePin(QString oldPin, QString newPin);
signals:

private:
    SmartCard* smartCard = NULL;
    PKCS11_slot_st* slot;
    bool soLogged=false;
    bool userLogged=false;
    bool login(QString password, bool soLogin);
    bool isLogged(bool soLogin);
    QString currentPassword;
};

#endif // SMARTCARDREADER_H
