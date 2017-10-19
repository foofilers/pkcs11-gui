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
#ifndef RASC_KEY_H
#define RASC_KEY_H

#include <QObject>
#include <libp11.h>
#include <qmetatype.h>
#include <neither/either.hpp>

using namespace neither;

class Key : public QObject
{
    Q_OBJECT
public:
    explicit Key(QObject *parent, PKCS11_TOKEN *token, PKCS11_KEY* key, bool privateKey);
    Key();
    Key(const Key &other);
    QString getLabel();
    QString getId();
    Either<QString,bool> deleteKey();
    Either<QString, QString> getDetails();
    bool isPrivate();
    EVP_PKEY *getEVPKey();
    Either<QString, bool> savePEM(QString pemPath);
private:
    bool privateKey;
protected:
    PKCS11_TOKEN *token = nullptr;
    PKCS11_KEY *key = nullptr;
};

Q_DECLARE_METATYPE(Key)

#endif // RASC_KEY_H
