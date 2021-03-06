// ############################################################################
// linalg.hpp
// ==========
// Author : Sepand KASHANI [sep@zurich.ibm.com]
// ############################################################################

/*
 * Linear algebra routines.
 */

#ifndef PYPELINE_UTIL_MATH_LINALG_HPP
#define PYPELINE_UTIL_MATH_LINALG_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <string>

#include "xtensor/xtensor.hpp"
#include "xtensor/xnorm.hpp"

#include "pypeline/util/argcheck.hpp"

namespace pypeline { namespace util { namespace math { namespace linalg {
    /*
     * Determine rotation angle from Z-axis rotation matrix.
     *
     * Parameters
     * ----------
     * R : xt::xexpression
     *     (3, 3) rotation matrix around the Z-axis;
     *
     * Returns
     * -------
     * angle : double
     *     Signed rotation angle [rad].
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include "xtensor/xarray.hpp"
     *    #include "pypeline/util/math/linalg.hpp"
     *
     *    auto R = xt::xarray<double> {{0, -1, 0},
     *                                 {1,  0, 0},
     *                                 {0,  0, 1}};
     *
     *    namespace linalg = pypeline::util::math::linalg;
     *    double angle = linalg::z_rot2angle(R);
     */
    template <typename E>
    double z_rot2angle(E &&R) {
        namespace argcheck = pypeline::util::argcheck;

        if (!argcheck::has_floats(R)) {
            std::string msg = "Parameter[R] must contain real values.";
            throw std::runtime_error(msg);
        }
        if (!argcheck::has_shape(R, std::array<size_t, 2> {3, 3})) {
            std::string msg = "Parameter[R] must have shape (3, 3).";
            throw std::runtime_error(msg);
        }
        xt::xtensor<double, 1> border_cells {R(0, 2), R(1, 2),
                                             R(2, 2),
                                             R(2, 0), R(2, 1)};
        xt::xtensor<double, 1> valid_border_cells {0, 0, 1, 0, 0};
        if (!xt::allclose(border_cells, valid_border_cells)) {
            std::string msg = "Parameter[R] is not a rotation matrix around the Z-axis.";
            throw std::runtime_error(msg);
        }

        /*
         * Replace [sin,cos]_angle as given below when moving to C++17.
         * double cos_angle = std::clamp<double>(R(0, 0), -1, 1);
         * double sin_angle = std::clamp<double>(R(1, 0), -1, 1);
         */
        double cos_angle = (R(0, 0) > 1) ? 1 : ((R(0, 0) < -1) ? -1 : R(0, 0));
        double sin_angle = (R(1, 0) > 1) ? 1 : ((R(1, 0) < -1) ? -1 : R(1, 0));

        double angle = 0;
        if (sin_angle >= 0) {  // In quadrants I or II
            angle = std::acos(cos_angle);
        } else {  // In quadrants III or IV
            angle = -std::acos(cos_angle);
        }

        return angle;
    }

    /*
     * 3D rotation matrix.
     *
     * Parameters
     * ----------
     * axis : xt::xexpression
     *     (3,) rotation axis.
     * angle : double
     *     Signed rotation angle [rad].
     *
     * Returns
     * -------
     * R : xt::xtensor<double, 2>
     *     (3, 3) rotation matrix.
     *
     * Examples
     * --------
     * .. literal_block::
     *
     *    #include <cmath>
     *    #include "xtensor/xtensor.hpp"
     *    #include "pypeline/util/math/linalg.hpp"
     *
     *    xt::xtensor<double, 1> axis {1, 1, 1};
     *    double angle = M_PI / 2.0;
     *
     *    namespace linalg = pypeline::util::math::linalg;
     *    auto R = linalg::rot(axis, angle);
     */
    template <typename E>
    xt::xtensor<double, 2> rot(E &&axis, const double angle) {
        namespace argcheck = pypeline::util::argcheck;

        if (!argcheck::has_floats(axis)) {
            std::string msg = "Parameter[axis] must contain real values.";
            throw std::runtime_error(msg);
        }
        if (!argcheck::has_shape(axis, std::array<size_t, 1> {3})) {
            std::string msg = "Parameter[axis] must have shape (3,).";
            throw std::runtime_error(msg);
        }
        if (xt::allclose(axis, 0)) {
            std::string msg = "Cannot rotate around null-vector.";
            throw std::runtime_error(msg);
        }

        double norm_axis = xt::norm_l2(axis, { 0 }, xt::evaluation_strategy::immediate())(0);
        double a = axis(0) / norm_axis;
        double b = axis(1) / norm_axis;
        double c = axis(2) / norm_axis;
        double cos_angle = std::cos(angle);
        double sin_angle = std::sin(angle);

        double p00 = (a * a) + ((b * b) + (c * c)) * cos_angle;
        double p11 = (b * b) + ((a * a) + (c * c)) * cos_angle;
        double p22 = (c * c) + ((a * a) + (b * b)) * cos_angle;
        double p01 = (a * b * (1 - cos_angle)) - (c * sin_angle);
        double p10 = (a * b * (1 - cos_angle)) + (c * sin_angle);
        double p12 = (b * c * (1 - cos_angle)) - (a * sin_angle);
        double p21 = (b * c * (1 - cos_angle)) + (a * sin_angle);
        double p20 = (a * c * (1 - cos_angle)) - (b * sin_angle);
        double p02 = (a * c * (1 - cos_angle)) + (b * sin_angle);

        xt::xtensor<double, 2> R {{p00, p01, p02},
                                  {p10, p11, p12},
                                  {p20, p21, p22}};
        return R;
    }
}}}}

#endif //PYPELINE_UTIL_MATH_LINALG_HPP
