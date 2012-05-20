oclutil
=======

oclutil is header-only template library created for ease of C++ based OpenCL
development. Vector arithmetic and multidevice computation is supported.
See Doxygen-generated documentation at http://ddemidov.github.com/oclutil.

Selection of compute devices
----------------------------

You can select any number of available compute devices, which satisfy provided
filters. Filter is a functor returning bool and acting on a `cl::Device`
parameter. Several standard filters are provided, such as device type or name
filter, double precision support etc. Filters can be combined with logical
operators. In the example below all devices with names matching "Radeon" and
supporting double precision are selected:
```C++
#include <iostream>
#include <oclutil/oclutil.hpp>
using namespace clu;
int main() {
    auto device = device_list(
        Filter::Name("Radeon") && Filter::DoublePrecision()
        );
    std::cout << device.size() << " GPUs found:" << std::endl;
    for(auto &d : device)
        std::cout << "\t" << d.getInfo<CL_DEVICE_NAME>() << std::endl;
}
```

Often you want not just device list, but initialized OpenCL context with
command queue on each available device. This may be achieved with `queue_list`
function:
```C++
cl::Context context;
std::vector<cl::CommandQueue> queue;
// Select no more than 2 NVIDIA GPUs:
std::tie(context, queue) = queue_list(
    [](const cl::Device &d) {
        return d.getInfo<CL_DEVICE_VENDOR>() == "NVIDIA Corporation";
    } && Filter::Count(2)
    );
```

Memory allocation and vector arithmetic
---------------------------------------

Once you got queue list, you can allocate OpenCL buffers on the associated
devices. `clu::vector` constructor accepts `std::vector` of `cl::CommandQueue`.
The contents of the created vector will be equally partitioned between each
queue (presumably, each of the provided queues is linked with separate device). 
```C++
const uint n = 1 << 20;
std::vector<double> x(n);
std::generate(x.begin(), x.end(), [](){ return (double)rand() / RAND_MAX; });

cl::Context context;
std::vector<cl::CommandQueue> queue;
std::tie(context, queue) = queue_list(Filter::Type(CL_DEVICE_TYPE_GPU));

clu::vector<double> X(queue, CL_MEM_READ_ONLY,  x);
clu::vector<double> Y(queue, CL_MEM_READ_WRITE, n);
clu::vector<double> Z(queue, CL_MEM_READ_WRITE, n);
```

You can now use simple vector arithmetic with device vectors. For every
expression you use, appropriate kernel is compiled (first time it is
encountered in your program) and called automagically.

Vectors are processed in parallel across all devices they were allocated on:
```C++
Y = Const(42);
Z = Sqrt(Const(2) * X) + Cos(Y);
```

You can copy the result back to host or you can use `vector::operator[]` to
read (or write) vector elements diectly. Though latter technique is very
ineffective and should be used for debugging purposes only.
```C++
copy(Z, x);
assert(x[42] == Z[42]);
```

Another frequently performed operation is reduction of a vector expresion to
single value, such as summation. This can be done with `Reductor` class:
```C++
Reductor<double,SUM> sum(queue);
Reductor<double,MAX> max(queue);

std::cout << max(Abs(X) - Const(0.5)) << std::endl;
std::cout << sum(Sqrt(Const(2) * X) + Cos(Y)) << std::endl;
```

Using custom kernels
--------------------

Custom kernels are of course possible as well. `vector::operator(uint)` returns
`cl::Buffer` object for a specified device:
```C++
cl::Context context;
std::vector<cl::CommandQueue> queue;
std::tie(context, queue) = queue_list(Filter::Type(CL_DEVICE_TYPE_GPU));

const uint n = 1 << 20;
clu::vector<float> x(queue, CL_MEM_WRITE_ONLY, n);

auto program = build_sources(context, std::string(
    "kernel void dummy(uint size, global float *x)\n"
    "{\n"
    "    uint i = get_global_id(0);\n"
    "    if (i < size) x[i] = 4.2;\n"
    "}\n"
    ));

for(uint d = 0; d < queue.size(); d++) {
    auto dummy = cl::Kernel(program, "dummy").bind(queue[d], alignup(n, 256), 256);
    dummy((uint)x.part_size(d), x(d));
}

std::cout << sum(x) << std::endl;
```
