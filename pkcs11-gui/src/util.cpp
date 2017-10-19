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
#include "inc/util.h"
#include<string.h>
#include <QDebug>

char digits[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

unsigned char *parseId(int id){
    unsigned char *c=new unsigned char[4];
    c[0] = id & 0xFF;
    c[1] = (id>>8) & 0xFF;
    c[2] = (id>>16) & 0xFF;
    c[3] = (id>>24) & 0xFF;
    return c;
}

int getIdLength(int id){
    return strlen((char*)parseId(id));
}


QString convertId(unsigned char *rawId, size_t size){
    int id=0;
    for (unsigned int i = 0; i<size; i++){
        id|=rawId[i]<<(i*8);
    }
    return QString::number(id);
}


