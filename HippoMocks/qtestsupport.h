#ifndef HIPPOMOCKS_QTESTSUPPORT_H
#define HIPPOMOCKS_QTESTSUPPORT_H

#include <QtTest/QTest>
#include "hippomocks.h"

namespace HippoMocks {

inline HippoMocks::Reporter *qTestReporter() {
    static struct QTestReporter : HippoMocks::Reporter {
        QTestReporter() : latentException([]{}) {}
        std::function<void()> latentException;

        void CallMissing(Call& call, MockRepository& repo) override {
            (void)call;
            std::stringstream text;
            text << "Function with expectation not called!" << std::endl;
            text << repo;
            QTest::qFail(text.str().c_str(), call.fileName, call.lineno);
        }
        void ExpectationExceeded(Call& call, MockRepository& repo, const std::string& args, const char* funcName) override {
            (void)call;
            std::stringstream text;
            text << "Function " << funcName << args << " called with mismatching expectation!" << std::endl;
            text << repo;
            QTest::qFail(text.str().c_str(), call.fileName, call.lineno);
        }
        void FunctionCallToZombie(MockRepository& repo, const std::string& args) override {
            (void)args;
            std::stringstream text;
            text << "Function called on mock that has already been destroyed!" << std::endl;
            text << repo;
            QFAIL(text.str().c_str());
        }
        void InvalidBaseOffset(size_t baseOffset, MockRepository& repo) override {
            (void)baseOffset;
            (void)repo;
            std::terminate();
        }
        void InvalidFuncIndex(size_t funcIndex, MockRepository& repo) override {
            (void)funcIndex;
            (void)repo;
            std::terminate();
        }
        void NoExpectationMatches(MockRepository& repo, const std::string& args, const char* funcName) override {
            std::stringstream text;
            text << "Function " << funcName << args << " called with mismatching expectation!" << std::endl;
            text << repo;
            QFAIL(text.str().c_str());
        }
        void NoResultSetUp(Call& call, MockRepository& repo, const std::string& args, const char* funcName) override {
            (void)call;
            std::stringstream text;
            text << "No result set up on call to " << funcName << args << std::endl << repo;
            QFAIL(text.str().c_str());
        }
        void UnknownFunction(MockRepository& repo) override {
            std::stringstream text;
            text << "Function called without expectation!" << std::endl;
            text << repo;
            QFAIL(text.str().c_str());
        }
        void TestStarted() override {
            latentException = []{};
        }
        void TestFinished() override {
#if __cpp_lib_uncaught_exceptions >= 201411L
            if (!std::uncaught_exceptions() && latentException) {
#else

            if (!std::uncaught_exception() && latentException) {
#endif
                latentException();
            }
        }
    } defaultReporter;
    return &defaultReporter;
}

}

#endif  // HIPPOMOCKS_QTESTSUPPORT_H
