#include <stdio.h>
#include "gtest/gtest.h"
#include "MediaData.h"
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/bind.hpp>
#include <boost/thread.hpp>
//#include <boost/thread/locks.hpp>
//#include <boost/thread/condition_variable.hpp>
//#include <boost/circular_buffer.hpp>

TEST(MediaData_test, constructor)
{
    zMedia::MediaBuffer buf;
    EXPECT_EQ(NULL, buf.data());
}

int main(int argc, char** argv)
{
    zMedia::MediaBuffer buf;
    printf("hello\n");
    boost::regex pat("^Subject: (Re: |Aw:)*(.*)");
    return 0;
}
