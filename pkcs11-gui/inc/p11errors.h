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
#ifndef P11ERRORS_H
#define P11ERRORS_H

#include <QString>

class P11Error{

public:
    explicit P11Error(unsigned long errorCode, QString errorMessage);
    static P11Error getOpensslLastError();
    QString getErrorMessage();
    unsigned long getErrorCode();
private:
    QString errorMessage;
    unsigned long errorCode;
};




#endif // P11ERRORS_H
