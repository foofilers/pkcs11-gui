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
#include "key.h"
#include <util.h>
#include <p11errors.h>
#include <QDebug>
#include<openssl/evp.h>
#include<openssl/pem.h>

Key::Key(QObject *parent, PKCS11_TOKEN *token, PKCS11_KEY *key, bool privateKey) : QObject(parent){
    this->key=key;
    this->token=token;
    this->privateKey=privateKey;
}

Key::Key():QObject(nullptr){

}

Key::Key(const Key &other): QObject(other.parent()){
    this->key=other.key;
    this->token=other.token;
}

QString Key::getLabel(){
    return QString(this->key->label);
}

QString Key::getId(){
    return convertId(this->key->id, this->key->id_len);
}

Either<QString, bool> Key::deleteKey(){
    using ret = Either<QString, bool>;
    int rc=PKCS11_remove_key(this->key);
    if (rc!=0){
        return ret::leftOf(tr("Error deleting key:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}

EVP_PKEY* Key::getEVPKey(){
    if (isPrivate()){
        return PKCS11_get_private_key(this->key);
    }else{
        return PKCS11_get_public_key(this->key);
    }
}

Either<QString,QString> Key::getDetails(){
    using ret = Either<QString,QString>;

    EVP_PKEY *evpKey=getEVPKey();
    if (evpKey==nullptr){
        return ret::leftOf(tr("EVP Key is null"));
    }

    BIO *bioMem = BIO_new(BIO_s_mem());
    if (NULL == bioMem) {
        return ret::leftOf("Error allocating BIO structure");
    }

    if (isPrivate()){
        if(EVP_PKEY_print_private(bioMem, evpKey , 1, nullptr)==0){
            BIO_free(bioMem);
            return ret::leftOf(tr("Cannot print Key detail:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }else{
        if(EVP_PKEY_print_public(bioMem, evpKey , 1, nullptr)==0){
            BIO_free(bioMem);
            return ret::leftOf(tr("Cannot print Key detail:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }
    uint64_t bioSize=BIO_number_written(bioMem);
    char* certDetail = (char *) malloc(bioSize + 1);
    memset(certDetail, 0, bioSize + 1);
    BIO_read(bioMem, certDetail, bioSize);
    BIO_free(bioMem);
    QString result = QString(certDetail);
    free(certDetail);
    return ret::rightOf(result);
}

Either<QString,bool> Key::savePEM(QString pemPath){
    using ret=Either<QString,bool>;

    EVP_PKEY *evpKey=getEVPKey();
    if (evpKey==nullptr){
        return ret::leftOf(tr("EVP Key is null"));
    }

    FILE* destFile = fopen(pemPath.toLatin1().data(),"wb");
    if (isPrivate()){
        if (PEM_write_PrivateKey(destFile,evpKey,nullptr,nullptr,0,0,nullptr)==0){
            fclose(destFile);
            return ret::leftOf(tr("Error saving private Key:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }else{
        if (PEM_write_PUBKEY(destFile,evpKey)==0){
            fclose(destFile);
            return ret::leftOf(tr("Error saving public Key:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
        }
    }
    fclose(destFile);
    return ret::rightOf(true);
}

bool Key::isPrivate(){
    return this->privateKey;
}


