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
#include "smartcard.h"
#include "smartcardreader.h"
#include <QDebug>
#include <QMessageBox>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <p11errors.h>
#include <util.h>
#include <libp11.h>

SmartCard::SmartCard(SmartCardReader *parent, PKCS11_TOKEN *token) : QObject(parent){
    this->token=token;
    this->smartCardReader=parent;
}

QString SmartCard::getLabel(){
    return QString(this->token->label);
}

QString SmartCard::getManufacturer(){
    return QString(this->token->manufacturer);
}

QString SmartCard::getModel(){
    return QString(this->token->model);
}

QString SmartCard::getSerial(){
    return QString(this->token->serialnr);
}

Either<QString,QList<Key*>> SmartCard::getPublicKeys(){
    using ret = Either<QString,QList<Key*>>;
    PKCS11_KEY *keys;
    unsigned int nKeys=0;
    QList<Key*> result;
    int rc= PKCS11_enumerate_public_keys(this->token,&keys,&nKeys);
    if (rc!=0){
        return ret::leftOf(tr("PKCS11_enumerate_public_keys errors rc:"));
    }else{
        for (unsigned int i=0;i<nKeys;i++){
            result<<new Key(this,this->token,keys,false);
            keys++;
        }
    }
    return ret::rightOf(result);
}

Either<QString,QList<Key*>> SmartCard::getPrivateKeys(){
    using ret = Either<QString,QList<Key*>>;
    PKCS11_KEY *keys;
    unsigned int nKeys=0;

    QList<Key*> result;
    if (this->token->loginRequired && !this->smartCardReader->isSoLogged() && !this->smartCardReader->isUserLogged() ){
        return ret::leftOf(tr("Login required to retrieve the private keys"));
    }
    int rc = PKCS11_enumerate_keys(this->token,&keys,&nKeys);
    if (rc!=0){
        return ret::leftOf(tr("PKCS11_enumerate_keys errors rc:"));
    }else{
        for (unsigned int i=0;i<nKeys;i++){
            if (PKCS11_get_key_type(keys)==EVP_PKEY_RSA){
                result<<new Key(this,this->token,keys,true);
            }
            keys++;
        }
    }
    return ret::rightOf(result);
}

Either<QString,QList<Certificate*>> SmartCard::getCertificates(){
    using ret = Either<QString,QList<Certificate*>>;
    PKCS11_CERT* certs;
    unsigned int nCerts;
    QList<Certificate*> result;
    int rc = PKCS11_enumerate_certs(this->token,&certs,&nCerts);
    if (rc!=0){
        return ret::leftOf(tr("Error during certificate gets rc"));
    }else{
        for (unsigned int i=0;i<nCerts;i++){
            result<<new Certificate(this,certs);
            certs++;
        }
    }
    return ret::rightOf(result);
}

Either<QString,bool> SmartCard::storeP12(QString path, QString password,QString label,int id){
    using ret =  Either<QString,bool>;
    STACK_OF(X509) *ca = NULL;
    PKCS12 *p12;
    EVP_PKEY *pkey;
    X509 *cert;
    QString errorDetail;

    if (!this->smartCardReader->isLogged()){
        return ret::leftOf(tr("Login required "));
    }

    qDebug()<<"opening p12"<<path.toUtf8().data();
    FILE* p12File = fopen(path.toUtf8().data(),"rb");
    p12 = d2i_PKCS12_fp(p12File, NULL);
    fclose(p12File);

    if (!p12) {
        return ret::leftOf(tr("Cannot store the private key:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }

    if (!PKCS12_parse(p12,password.toLatin1().data(), &pkey, &cert, &ca)) {
        PKCS12_free(p12);
        return ret::leftOf(tr("Cannot parse p12 file :%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    PKCS12_free(p12);

    if (pkey) {
        qDebug()<<"adding private key";
        int rc = PKCS11_store_private_key(this->token,pkey,label.toLatin1().data(),parseId(id),getIdLength(id));
        qDebug()<<"RC:"<<rc;
        if (rc!=0){
            EVP_PKEY_free(pkey);
            return ret::leftOf(tr("Cannot store the private key:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }

    if (cert){
        qDebug()<<"adding certificate";
        int rc = PKCS11_store_certificate(this->token,cert,label.toLatin1().data(),parseId(id),getIdLength(id),NULL);
        qDebug()<<"RC:"<<rc;
        if (rc!=0){
            X509_free(cert);
            return ret::leftOf(tr("Cannot store the certificate:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }
    EVP_PKEY_free(pkey);
    X509_free(cert);
    return ret::rightOf(true);
}

Either<QString,bool> SmartCard::generateKeyPairs(int id, QString label, unsigned long modulus){
    using ret = Either<QString,bool>;
    if (PKCS11_generate_key(this->token,label.toLatin1().data(),parseId(id),getIdLength(id),modulus)){
        return ret::leftOf(tr("Cannot generate key pairs:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}

Either<QString, bool> SmartCard::setPin(QString pin){
    using ret =  Either<QString,bool>;
    if (!this->smartCardReader->isSoLogged()){
        return ret::leftOf(tr("Security officer Login required "));
    }
    if(PKCS11_init_pin(this->token,pin.toLatin1().data())!=0){
        return ret::leftOf(tr("Cannot set pin:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}


