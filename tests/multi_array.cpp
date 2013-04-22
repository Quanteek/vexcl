#define VEXCL_SHOW_KERNELS
#define BOOST_TEST_MODULE MultiArray
#include <boost/test/unit_test.hpp>
#include <vexcl/multi_array.hpp>
#include "context_setup.hpp"

BOOST_AUTO_TEST_CASE(create)
{
    vex::multi_array<double, 2> x(ctx, {1024, 1024});
    BOOST_CHECK(x.size() == 1024 * 1024);
}

BOOST_AUTO_TEST_CASE(arithmetics)
{
    vex::multi_array<double, 3> y(ctx, {32, 32, 32});
    vex::vector<double> x(ctx, y.size()); // works.
    //vex::multi_array<double, 3> x(ctx, {32, 32, 32}); // doesn't

    x = 2 * M_PI * vex::element_index();
    y = pow(sin(x), 2.0) + pow(cos(x), 2.0); 

    check_sample(y, [](size_t, double v) { BOOST_CHECK_CLOSE(v, 1, 1e-8); });
}

BOOST_AUTO_TEST_SUITE_END()
