#define BOOST_TEST_MODULE VectorCreate
#include <boost/test/unit_test.hpp>
#include "context_setup.hpp"

BOOST_AUTO_TEST_CASE(empty)
{
    vex::vector<double> x;

    BOOST_CHECK(0U == x.size());
    BOOST_CHECK(x.end() - x.begin() == 0);
}

BOOST_AUTO_TEST_CASE(size)
{
    const size_t N = 1024;
    vex::vector<double> x(ctx, N);

    BOOST_CHECK(x.size() == N);
    BOOST_CHECK(x.end() == x.begin() + N);
}

BOOST_AUTO_TEST_CASE(std_vector)
{
    const size_t N = 1024;

    std::vector<double> x = random_vector<double>(N);
    vex::vector<double> X(ctx, x);

    BOOST_CHECK(X.size() == x.size());

    std::vector<double> y(N);
    copy(X, y);

    check_sample(x, y, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(host_pointer)
{
    const size_t N = 1024;

    std::vector<double> x = random_vector<double>(N);
    vex::vector<double> X(ctx, N, x.data());

    BOOST_CHECK(X.size() == x.size());

    std::vector<double> y(N);
    copy(X, y);

    check_sample(x, y, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(copy_constructor)
{
    const size_t N = 1024;

    vex::vector<double> x1;
    vex::vector<double> x2(x1);

    BOOST_CHECK(x1.size() == 0U);
    BOOST_CHECK(x1.size() == x2.size());

    vex::vector<double> y1(ctx, random_vector<double>(N));
    vex::vector<double> y2(y1);

    BOOST_CHECK(y1.size() == N);
    BOOST_CHECK(y1.size() == y2.size());

    check_sample(y1, y2, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    const size_t N = 1024;

    vex::vector<double> x(ctx, N);
    x = 42;

    vex::vector<double> y = std::move(x);

    BOOST_CHECK(x.size() == 0U);
    BOOST_CHECK(y.size() == N);

    check_sample(y, [](size_t, double a) { BOOST_CHECK(a == 42); });
}

BOOST_AUTO_TEST_CASE(move_assign)
{
    const size_t N = 1024;
    std::vector<double> x = random_vector<double>(N);
    vex::vector<double> X(ctx, x);

    vex::vector<double> Y;
    Y = std::move(X);

    BOOST_CHECK(Y.size() == x.size());

    check_sample(Y, x, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(vector_swap)
{
    const size_t N = 1024;
    const size_t M = 512;

    vex::vector<double> x(ctx, N);
    vex::vector<double> y(ctx, M);

    x = 42;
    y = 67;

    swap(x, y);

    BOOST_CHECK(y.size() == N);
    BOOST_CHECK(x.size() == M);

    check_sample(x, [](size_t, double a) { BOOST_CHECK(a == 67); });
    check_sample(y, [](size_t, double a) { BOOST_CHECK(a == 42); });
}

BOOST_AUTO_TEST_CASE(vector_resize_to_std_vector)
{
    const size_t N = 1024;

    std::vector<double> x = random_vector<double>(N);
    vex::vector<double> X;

    X.resize(ctx, x);
    BOOST_CHECK(X.size() == x.size());

    check_sample(X, x, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(vector_resize_to_vex_vector)
{
    const size_t N = 1024;

    vex::vector<double> x(ctx, N);
    x = 42;

    vex::vector<double> y;
    y.resize(x);

    BOOST_CHECK(y.size() == x.size());

    check_sample(x, y, [](size_t, double a, double b) { BOOST_CHECK(a == b); });
}

BOOST_AUTO_TEST_CASE(stl_container_of_vex_vector)
{
    const size_t N = 1024;
    const size_t M = 16;

    std::vector< vex::vector<unsigned> > x;

    for(size_t i = 0; i < M; ++i) {
        x.push_back( vex::vector<unsigned>(ctx, N) );
        x.back() = i;
    }

    for(size_t j = 0; j < M; ++j)
        check_sample(x[j], [j](size_t, unsigned a) { BOOST_CHECK(a == j); });
}

BOOST_AUTO_TEST_SUITE_END()
