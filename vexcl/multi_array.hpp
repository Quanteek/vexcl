#ifndef VEXCL_MULTI_ARRAY_HPP
#define VEXCL_MULTI_ARRAY_HPP

/*
The MIT License

Copyright (c) 2012-2013 Denis Demidov <ddemidov@ksu.ru>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
 * \file   vexcl/multi_array.hpp
 * \author Denis Demidov <ddemidov@ksu.ru>
 * \brief  An analog of boost::multi_array
 */

#include <vector>
#include <numeric>
#include <vexcl/vector.hpp>

namespace vex {

template <typename T, size_t NDIM>
class multi_array : public vex::vector<T> {
    typedef vex::vector<T> Base;

    public:
        multi_array(
                const std::vector<cl::CommandQueue> &queue,
                std::vector<size_t> lengths
                ) :
            /* NOTE: this simply allocates an accordingly sized vex::vector for
             * now. The things that should be taken into account:
             * 1. Pitching (padding the fastest dimension so that row-wise
             *    acceses are coalesced). This is not as relevant on newer
             *    harware as it used to be though.
             * 2. Partitioning across devices. With the current approach
             *    arithmetic operations between equally sized multi_array will
             *    just work. But if we need to support dense matrix-vector
             *    products, then partitioning should happen across the slowest
             *    dimension.
             */
            Base(queue,
                    std::accumulate(
                        lengths.begin(), lengths.end(),
                        static_cast<size_t>(1), std::multiplies<size_t>()
                        )
                )
        {
            assert(lengths.size() == NDIM);
        }

        /* This currently only accepts normal vector expressions. As soon as
         * expression includes a term of vex::multi_array type, there is
         * runtime error "unsupported type in kernel".
         *
         * Creating separate Boost.Proto grammar for multi_arrays and only
         * delegating suitable operations to vex::vectors would probably be a
         * better idea anyway.
         */
        template <class Expr>
        const multi_array& operator=(const Expr &expr) {
            this->Base::operator=(expr);
            return *this;
        }
};

}

#endif
