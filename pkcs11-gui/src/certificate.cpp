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
#include "certificate.h"
#include <util.h>
#include <p11errors.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <QDebug>

Certificate::Certificate(QObject *parent, PKCS11_CERT* cert) : QObject(parent){
    this->cert = cert;
}

Certificate::Certificate():QObject(nullptr){

}

Certificate::Certificate(const Certificate &other):QObject(other.parent()){
    this->cert = other.cert;
}


QString Certificate::getLabel()
{
    return QString(this->cert->label);
}

QString Certificate::getId(){
    return convertId(this->cert->id,this->cert->id_len);
}

Either<QString, bool> Certificate::deleteCertificate(){
    using ret = Either<QString, bool>;
    int rc=PKCS11_remove_certificate(this->cert);
    if (rc!=0){
        return ret::leftOf(tr("Error deleting certificate:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}

Either<QString,QString> Certificate::getDetails(){
    using ret = Either<QString,QString>;
    BIO *bioMem = BIO_new(BIO_s_mem());
    if (NULL == bioMem) {
        return ret::leftOf("Error allocating BIO structure");
    }

    if(X509_print_ex(bioMem, this->cert->x509, XN_FLAG_COMPAT, X509_FLAG_COMPAT)==0){
        BIO_free(bioMem);
        return ret::leftOf(tr("Cannot print Certificate detail:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    char* certDetail = (char *) malloc(bioMem->num_write + 1);
    memset(certDetail, 0, bioMem->num_write + 1);
    BIO_read(bioMem, certDetail, bioMem->num_write);
    BIO_free(bioMem);
    QString result = QString(certDetail);
    free(certDetail);
    return ret::rightOf(result);
}

Either<QString,bool> Certificate::savePEM(QString pemPath){
    using ret=Either<QString,bool>;
    FILE* destFile = fopen(pemPath.toLatin1().data(),"wb");
    if (PEM_write_X509(destFile,this->cert->x509)==0){
        fclose(destFile);
        return ret::leftOf(tr("Error saving Certificate:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    fclose(destFile);
    return ret::rightOf(true);
}
