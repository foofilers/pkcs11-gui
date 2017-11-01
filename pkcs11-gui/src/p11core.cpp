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
#include "p11core.h"
#include <openssl/pem.h>
#include <QDebug>
#include <p11errors.h>

P11Core::P11Core(QObject* parent): QObject(parent){

}

Either<QString,bool> P11Core::init(QString driver){
    using ret = Either<QString,bool>;
    OpenSSL_add_all_algorithms();

    this->p11Context = PKCS11_CTX_new();

    /* load pkcs #11 module */
    int rc = PKCS11_CTX_load(this->p11Context, driver.toLatin1().data());
    qDebug()<<"Initialization context rc:"<<rc;

    if (rc!=0){
        ret::leftOf(tr("Error initializing pkcs11 module:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}


Either<QString,QList<SmartCardReader*>> P11Core::loadSmartCardReaders(){
    using ret = Either<QString,QList<SmartCardReader*>>;
    unsigned int nSlots=0;
    PKCS11_slot_st* availableSlots;
    QList<SmartCardReader*> result;

    int rc=PKCS11_enumerate_slots(this->p11Context,&availableSlots,&nSlots);
    if (rc==0){
        for (unsigned int i=0;i<nSlots;i++){
            result << new SmartCardReader(this,availableSlots);
            availableSlots++;
        }
    }else{
        return ret::leftOf(tr("Error loading slots:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    this->slotList = result;
    return ret::rightOf(this->slotList);
}

QList<SmartCardReader*> P11Core::getSmartCardReaders(){
    return this->slotList;
}



P11Core::~P11Core(){
    if (this->p11Context) {
        PKCS11_CTX_unload(this->p11Context);
        PKCS11_CTX_free(this->p11Context);
    }
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    this->p11Context = NULL;

    qDebug()<<"SmartCardReader destroyed";
}
