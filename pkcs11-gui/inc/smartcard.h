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
#ifndef SMARTCARD_H
#define SMARTCARD_H

#include <QObject>
#include <neither/either.hpp>
#include <libp11.h>
#include <key.h>
#include <certificate.h>
#include <QFile>

using namespace neither;

class SmartCardReader;

class SmartCard : public QObject
{
    Q_OBJECT
public:
    explicit SmartCard(SmartCardReader *parent, PKCS11_TOKEN *token);
    QString getLabel();
    QString getManufacturer();
    QString getModel();
    QString getSerial();
    Either<QString,QList<Key*>> getPublicKeys();
    Either<QString,QList<Key*>> getPrivateKeys();
    Either<QString,QList<Certificate*>> getCertificates();
    Either<QString, bool> storeP12(QString path, QString password, QString label, int id);
    Either<QString, bool> setPin(QString pin);
    Either<QString, bool> generateKeyPairs(int id, QString label, unsigned long modulus);
signals:

public slots:
private:
    SmartCardReader *smartCardReader;
    PKCS11_TOKEN *token;
};

#endif // SMARTCARD_H
