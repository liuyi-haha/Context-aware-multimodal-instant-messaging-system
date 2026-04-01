#pragma once

#include <gtest/gtest.h>
#include <string>

namespace tests::utils
{
    template <typename ExceptionType, typename Func>
    void expectThrowWithMessage(Func&& func, const QString& expectedMessage)
    {
        try
        {
            func();
            FAIL() << "Expected exception was not thrown.";
        }
        catch (const ExceptionType& ex)
        {
            EXPECT_STREQ(ex.what(), expectedMessage.toUtf8());
        }
        catch (...)
        {
            FAIL() << "Unexpected exception type was thrown.";
        }
    }
}

