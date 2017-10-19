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
#include "smartcardreader.h"
#include <QDebug>
#include <p11errors.h>

SmartCardReader::SmartCardReader(QObject *parent,PKCS11_slot_st* slot) : QObject(parent){
    this->slot = slot;
}

QString SmartCardReader::getDescription(){
    return QString(this->slot->description);
}

bool SmartCardReader::hasSmartCard(){
    return this->slot->token!=NULL;
}

SmartCard *SmartCardReader::getSmartCard(){
    if (this->smartCard==NULL && this->hasSmartCard()){
        this->smartCard = new SmartCard(this,this->slot->token);
    }
    return this->smartCard;
}

bool SmartCardReader::login (QString password, bool soLogin){
    int so=soLogin?1:0;
    qDebug()<<"smartcardReader login for SOUser:"<<so;

    int rc = PKCS11_open_session(this->slot,1);
    if (rc!=0){
        qWarning("Error opening session rc:%d",rc);
        return false;
    }

    rc = PKCS11_login(this->slot,so,password.toLatin1().data());
    if (rc!=0){
        qWarning("Error during login rc:%d",rc);
        userLogged=false;
        soLogged=false;
        return false;
    }
    this->currentPassword=password;
    if (soLogin){
       userLogged=false;
       soLogged=true;
    }else{
        userLogged=true;
        soLogged=false;
    }
    return true;
}

bool SmartCardReader::userLogin(QString password){
    return this->login(password,false);
}

bool SmartCardReader::soLogin(QString password){
    return this->login(password,true);
}

void SmartCardReader::logout(){
    qDebug()<<"logout";
    int rc = PKCS11_logout(this->slot);
    if (rc!=0){
        auto p11Error=P11Error::getOpensslLastError();
        if (p11Error.getErrorCode()==81075101){
            //already logged out;
            this->soLogged=false;
            this->userLogged=false;
        }else{
            qWarning("Logout failed %s",p11Error.getErrorMessage().toLatin1().data());
        }
    }else{
        this->soLogged=false;
        this->userLogged=false;
    }
}

bool SmartCardReader::isLogged(bool soLogin){
    if (soLogin){
        return soLogged;
    }else{
        return userLogged;
    }
}

bool SmartCardReader::isLogged(){
    return this->isSoLogged() || this->isUserLogged();
}

void SmartCardReader::relogin(){
    qDebug()<<"relogin";
    bool soLogged= this->soLogged;
    bool userLogged = this->userLogged;
    this->logout();
    if (soLogged){
        if(!this->login(this->currentPassword,true)){
            qWarning("Relogin failed");
        }
    }
    if (userLogged){
        if(!this->login(this->currentPassword,false)){
            qWarning("Relogin failed");
        }
    }
}

Either<QString, bool> SmartCardReader::changePin(QString oldPin,QString newPin){
    using ret = Either<QString, bool>;
    if (!this->isLogged()){
        return ret::leftOf(tr("Login required "));
    }
    if(PKCS11_change_pin(this->slot,oldPin.toLatin1().data(),newPin.toLatin1().data())!=0){
        return ret::leftOf(tr("Cannot change pin:%1").arg(P11Error::getOpensslLastError().getErrorMessage()));
    }
    return ret::rightOf(true);
}

bool SmartCardReader::isSoLogged(){
    return this->isLogged(true);
}

bool SmartCardReader::isUserLogged(){
    return this->isLogged(false);
}

