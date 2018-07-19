// ############################################################################
// argcheck.hpp
// ============
// Author : Sepand KASHANI [sep@zurich.ibm.com]
// ############################################################################

/*
 * Helper functions to ease argument checking.
 */

#ifndef PYPELINE_UTIL_ARGCHECK_HPP
#define PYPELINE_UTIL_ARGCHECK_HPP

namespace pypeline::util::argcheck {
    /*
     * Return true if array has specified rank.
     *
     * Parameters
     * ----------
     * x : xt::xexpression
     *     Array to test.
     * rank : size_t
     *     Desired rank.
     *
     * Returns
     * -------
     * has_rank : bool
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include "xtensor/xtensor.hpp"
     *    #include "pypeline/util/argcheck.hpp"
     *
     *    auto x = xt::ones<double>({3, 3});
     *
     *    namespace argcheck = pypeline::util::argcheck;
     *    argcheck::has_rank(x, 1);  // false
     *    argcheck::has_rank(x, 2);  // true
     */
    template<typename E>
    bool has_rank(E &&x, const size_t rank);

    /*
     * Return true if array has specified shape.
     *
     * Parameters
     * ----------
     * x : xt::xexpression
     *     Array to test.
     * shape : std::array<size_t>
     *     Desired shape.
     *
     * Returns
     * -------
     * has_shape : bool
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include <array>
     *    #include "xtensor/xtensor.hpp"
     *    #include "pypeline/util/argcheck.hpp"
     *
     *    auto x = xt::ones<double>({3, 3});
     *    auto shape1 = std::array<size_t, 2> {3, 3};
     *    auto shape2 = std::array<size_t, 1> {3};
     *
     *    namespace argcheck = pypeline::util::argcheck;
     *    argcheck::has_shape(x, shape1);  // true
     *    argcheck::has_shape(x, shape2);  // false
     */
    template<typename E, size_t rank>
    bool has_shape(E &&x, const std::array <size_t, rank> &shape);

    /*
     * Return true if array contains [float, double, long double].
     *
     * Parameters
     * ----------
     * x : xt::xexpression
     *     Array to test.
     *
     * Returns
     * -------
     * has_floats : bool
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include <complex>
     *    #include "xtensor/xtensor.hpp"
     *    #include "pypeline/util/argcheck.hpp"
     *
     *    using cfloat = std::complex<float>;
     *    using cdouble = std::complex<double>;
     *    using cldouble = std::complex<long double>;
     *
     *    namespace argcheck = pypeline::util::argcheck;
     *    argcheck::has_floats(xt::ones<int        >({3, 3}));  // false
     *    argcheck::has_floats(xt::ones<float      >({3, 3}));  // true
     *    argcheck::has_floats(xt::ones<double     >({3, 3}));  // true
     *    argcheck::has_floats(xt::ones<long double>({3, 3}));  // true
     *    argcheck::has_floats(xt::ones<cfloat     >({3, 3}));  // false
     *    argcheck::has_floats(xt::ones<cdouble    >({3, 3}));  // false
     *    argcheck::has_floats(xt::ones<cldouble   >({3, 3}));  // false
     */
    template<typename E>
    bool has_floats(E &&x);

    /*
     * Return true if array contains std::complex<[float, double, long double]>.
     *
     * Parameters
     * ----------
     * x : xt::xexpression
     *     Array to test.
     *
     * Returns
     * -------
     * has_complex : bool
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include <complex>
     *    #include "xtensor/xtensor.hpp"
     *    #include "pypeline/util/argcheck.hpp"
     *
     *    using cfloat = std::complex<float>;
     *    using cdouble = std::complex<double>;
     *    using cldouble = std::complex<long double>;
     *
     *    namespace argcheck = pypeline::util::argcheck;
     *    argcheck::has_complex(xt::ones<int        >({3, 3}));  // false
     *    argcheck::has_complex(xt::ones<float      >({3, 3}));  // false
     *    argcheck::has_complex(xt::ones<double     >({3, 3}));  // false
     *    argcheck::has_complex(xt::ones<long double>({3, 3}));  // false
     *    argcheck::has_complex(xt::ones<cfloat     >({3, 3}));  // true
     *    argcheck::has_complex(xt::ones<cdouble    >({3, 3}));  // true
     *    argcheck::has_complex(xt::ones<cldouble   >({3, 3}));  // true
     */
    template<typename E>
    bool has_complex(E &&x);
}

#include "_argcheck.tpp"

#endif //PYPELINE_UTIL_ARGCHECK_HPP