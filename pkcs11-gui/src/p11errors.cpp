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
#include "p11errors.h"
#include <openssl/err.h>
#include <QString>

P11Error::P11Error(unsigned long errorCode, QString errorMessage){
    this->errorCode=errorCode;
    this->errorMessage=errorMessage;
}

P11Error P11Error::getOpensslLastError(){
    unsigned long errorCode = ERR_peek_last_error();
    if (errorCode) {
        char* errorMsg = ERR_error_string(errorCode, NULL);
        return P11Error(errorCode,QString(errorMsg));
    }
    return P11Error(0,QString());
}

QString P11Error::getErrorMessage(){
    return errorMessage;
}

unsigned long P11Error::getErrorCode(){
    return this->errorCode;
}
