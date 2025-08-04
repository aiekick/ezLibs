#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezSingleton is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/* its a singleton desing who not cause memeory leak in your leak checker

macro to add just after your class definition
ex : 
class Toto {
	IMPLEMENT_SINGLETON(Toto)
};

then you need to init via : Toto::initSingleton()
then you call singleton via Toto::ref().any_methods
then you need to unit via : Toto::unitSingleton()

IMPLEMENT_SINGLETON is using unique_ptr
IMPLEMENT_SHARED_SINGLETON is using shared_ptr

*/

#include <memory>

#define IMPLEMENT_SINGLETON(TTYPE)                           \
public:                                                      \
    static std::unique_ptr<TTYPE>& initSingleton() {         \
        static auto mp_instance = std::make_unique<TTYPE>(); \
        return mp_instance;                                  \
    }                                                        \
    static TTYPE& ref() {                                    \
        return *initSingleton().get();                       \
    }                                                        \
    static void unitSingleton() {                            \
        initSingleton().reset();                             \
    }

#define IMPLEMENT_SHARED_SINGLETON(TTYPE)                    \
public:                                                      \
    static std::shared_ptr<TTYPE> initSingleton() {         \
        static auto mp_instance = std::make_shared<TTYPE>(); \
        return mp_instance;                                  \
    }                                                        \
    static std::shared_ptr<TTYPE> ref() {                   \
        return initSingleton();                             \
    }                                                        \
    static void unitSingleton() {                            \
        initSingleton().reset();                             \
    }
