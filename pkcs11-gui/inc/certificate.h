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
#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <QObject>
#include <libp11.h>
#include <qvariant.h>
#include <neither/either.hpp>

using namespace neither;

class Certificate : public QObject
{
    Q_OBJECT
public:
    explicit Certificate(QObject *parent, PKCS11_CERT* cert);
    Certificate();
    Certificate(const Certificate &other);
    QString getLabel();
    QString getId();
    Either<QString,bool> deleteCertificate();
    Either<QString, QString> getDetails();
    Either<QString, bool> savePEM(QString pemPath);
signals:

public slots:
protected:
    PKCS11_CERT* cert=nullptr;
};

Q_DECLARE_METATYPE(Certificate)

#endif // CERTIFICATE_H
