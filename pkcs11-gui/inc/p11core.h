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
#ifndef P11CORE_H
#define P11CORE_H

#include <libp11.h>
#include <neither/either.hpp>
#include <QObject>
#include <QList>
#include <smartcardreader.h>
using namespace neither;

class P11Core : public QObject
{
    Q_OBJECT
public:
    P11Core(QObject* parent);
     ~P11Core();
    Either<QString,QList<SmartCardReader*>> loadSmartCardReaders();
    QList<SmartCardReader*> getSmartCardReaders();
    Either<QString,bool> init();
private:
    PKCS11_CTX *p11Context=NULL;
    QList<SmartCardReader*> slotList;

};

#endif // SMARTCARDREADER_H
