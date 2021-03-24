#pragma once

#include <cstdint>

#include "General.h"
#include "HyperionUtils/Concepts.h"

namespace hyperion::math {

#ifndef _MSC_VER
	using std::uint32_t;
	using std::uint64_t;
#endif

	using utils::concepts::FloatingPoint;

	/// @brief Collection of approximations of various exponential functions
	class Exponentials {
	  public:
		/// @brief Calculates the mantissa and exponent of `x`,
		/// in the representation x = mantissa * 2^exponent
		///
		/// @param x - The value to calculate the mantissa and exponent for
		/// @param e - The `int` to store the exponent in
		/// @return - The mantissa
		[[nodiscard]] inline static constexpr auto
		frexp(FloatingPoint auto x, int* e) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return frexpf_internal(x, e);
			}
			else {
				return frexp_internal(x, e);
			}
		}

		/// @brief Fast approximiation calculation of e^x
		///
		/// @param x - The exponent
		/// @return - e^x
		[[nodiscard]] inline static constexpr auto
		exp(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return expf_internal(x);
			}
			else {
				return exp_internal(x);
			}
		}

		/// @brief Fast approximation calculation of ln(x)
		///
		/// @param x - The input
		/// @return - ln(x)
		[[nodiscard]] inline static constexpr auto
		ln(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return lnf_internal(x);
			}
			else {
				return ln_internal(x);
			}
		}

		/// @brief Fast approximation calculation of log_2(x)
		///
		/// @param x - The input
		/// @return - log_2(x)
		[[nodiscard]] inline static constexpr auto
		log2(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return log2f_internal(x);
			}
			else {
				return log2_internal(x);
			}
		}

		/// @brief Fast approximation calculation of log_10(x)
		///
		/// @param x - The input
		/// @return - log_10(x)
		[[nodiscard]] inline static constexpr auto
		log10(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return log10f_internal(x);
			}
			else {
				return log10_internal(x);
			}
		}

		/// @brief Fast approximation calculation of 2^x
		///
		/// @param x - The exponent
		/// @return 2^x
		[[nodiscard]] inline static constexpr auto
		pow2(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return pow2f_internal(x);
			}
			else {
				return pow2_internal(x);
			}
		}

		/// @brief Fast approximation calculation of 10^x
		///
		/// @param x - The exponent
		/// @return 10^x
		[[nodiscard]] inline static constexpr auto
		pow10(FloatingPoint auto x) noexcept -> decltype(x) {
			if constexpr(std::is_same_v<decltype(x), float>) {
				return pow10f_internal(x);
			}
			else {
				return pow10_internal(x);
			}
		}

		/// @brief Fast approximation calculation of base^exponent
		///
		/// @param base - The base to use
		/// @param exponent - The exponent to use
		/// @return - base^exponent
		[[nodiscard]] inline static constexpr auto
		pow(FloatingPoint auto base, FloatingPoint auto exponent) noexcept
			-> decltype(base) requires(std::is_same_v<decltype(base), decltype(exponent)>) {
			if constexpr(std::is_same_v<decltype(base), float>) {
				return powf_internal(base, exponent);
			}
			else {
				return pow_internal(base, exponent);
			}
		}

	  private :
		  /// @brief Calculates the mantissa and exponent of `x`,
		  /// in the representation x = mantissa * 2^exponent
		  ///
		  /// This seems to be a common knowledge implementation and was found in several cstdlibs
		  ///
		  /// @param x - The value to calculate the mantissa and exponent for
		  /// @param e - The `int` to store the exponent in
		  /// @return - The mantissa
		  [[nodiscard]] inline static constexpr auto
		  frexpf_internal(float x, int* e) noexcept -> float {
			union {
				float f;
				uint32_t i;
			} y = {x};
			auto ee = static_cast<int>(y.i >> 23U & 0xffU); // NOLINT

			if(!static_cast<bool>(ee)) {
				if(static_cast<bool>(x)) {
					x = frexpf_internal(static_cast<float>(static_cast<double>(x) * 0x1p64), e);
					*e -= 64;
				}
				else {
					*e = 0;
				}
				return x;
			}
			else if(ee == 0xff) {
				return x;
			}

			*e = ee - 0x7e;
			y.i &= 0x807fffffUL; // NOLINT
			y.i |= 0x3f000000UL; // NOLINT
			return y.f;			 // NOLINT
		}

		/// @brief Helper function for `expf`; Don't use on its own
		///
		/// This is a Pade approximation accurate in (-6, 4)
		///
		/// @param x - the exponent
		/// @return - e^x
		[[nodiscard]] inline static constexpr auto exp_helperf(float x) noexcept -> float {
			auto numerator = 1680 + x * (840 + x * (180 + x * (20 + x)));
			auto denominator = 1680 + x * (-840 + x * (180 + x * (-20 + x)));
			return numerator / denominator;
		}

		/// @brief Fast approximiation calculation of e^x
		///
		/// @param x - The exponent
		/// @return - e^x
		[[nodiscard]] inline static constexpr auto expf_internal(float x) noexcept -> float {
			const auto condition = x < -3.0F || x > 2.0F;
			if(condition) {
				const auto multiplier
					= static_cast<float>(x < 0.0F)
						  * 0.1353352832366126918939994949724844034076315459095758814681588726F
					  + static_cast<float>(x >= 0.0F)
							* 7.3890560989306502272304274605750078131803155705518473240871278225F;
				const auto subtractor
					= static_cast<float>(x < 0.0F) * -2.0F + static_cast<float>(x > 0.0F) * 2.0F;
				return expf_internal(x - subtractor) * multiplier;
			}
			else {
				return exp_helperf(x);
			}
		}

		/// @brief Helper function to lnf. Performs ln(x + 1), accurate in -0.8 <= x <= 5
		///
		/// @param x The input
		/// @return - ln(x + 1)
		[[nodiscard]] inline static constexpr auto lnXPlus1f(float x) noexcept -> float {
			auto numerator = x * (7560 + x * (15120 + x * (9870 + x * (2310 + x * 137))));
			auto denominator = 7560 + x * (18900 + x * (16800 + x * (6300 + x * (900 + 30 * x))));
			return numerator / denominator;
		}

		/// @brief Fast approximation calculation of ln(x)
		///
		/// @param x - The input
		/// @return - ln(x)
		[[nodiscard]] inline static constexpr auto lnf_internal(float x) noexcept -> float {
			// subtract one, because we'll be using an ln(x + 1) approximation
			float in = x - 1.0F;
			// if we're outside the accurate range, we'll reduce to the accurate range
			auto condition = static_cast<float>(in >= 5.0F || in <= -0.5F);
			auto divVal = in >= 5.0F ? 5.0F : (in <= -0.5F ? -0.5F : 1.0F);
			// if outside the accurate range, reduce to the accurate range and then perform the
			// approximation
			return condition
					   * (lnXPlus1f(((in + 1.0F) / (divVal + 1.0F)) - 1.0F) + lnXPlus1f(divVal))
				   + (1.0F - condition) * lnXPlus1f(in);
		}

		/// @brief Fast approximation calculation of log_2(x)
		///
		/// @param x - The input
		/// @return - log_2(x)
		[[nodiscard]] inline static constexpr auto log2f_internal(float x) noexcept -> float {
			return lnf_internal(x)
				   / 0.6931471805599453094172321214581765680755001343602552541206800094F;
		}

		/// @brief Fast approximation calculation of log_10(x)
		///
		/// @param x - The input
		/// @return - log_10(x)
		[[nodiscard]] inline static constexpr auto log10f_internal(float x) noexcept -> float {
			return lnf_internal(x)
				   / 2.3025850929940456840179914546843642076011014886287729760333279009F;
		}

		/// @brief Fast approximation calculation of 2^x
		///
		/// @param x - The exponent
		/// @return 2^x
		[[nodiscard]] inline static constexpr auto pow2f_internal(float x) noexcept -> float {
			auto integer = static_cast<int>(x);
			if(x < 0.0F) {
				if(General::abs(static_cast<float>(integer) - x) > 0) {
					return expf_internal(
						x * 0.6931471805599453094172321214581765680755001343602552541206800094F);
				}
				else {
					auto out = 2;
					for(int i = 1; i < General::abs(integer); ++i) {
						out *= 2;
					}
					return 1.0F / static_cast<float>(out);
				}
			}
			else if(x - static_cast<float>(integer) > 0) {
				return expf_internal(
					x * 0.6931471805599453094172321214581765680755001343602552541206800094F);
			}
			else if(x == 0.0F) {
				return 1.0F;
			}
			else {
				auto out = 2;
				for(int i = 1; i < integer; ++i) {
					out *= 2;
				}

				return static_cast<float>(out);
			}
		}

		/// @brief Fast approximation calculation of 10^x
		///
		/// @param x - The exponent
		/// @return 10^x
		[[nodiscard]] inline static constexpr auto pow10f_internal(float x) noexcept -> float {
			auto integer = static_cast<int>(x);
			if(x < 0.0F) {
				if(x - static_cast<float>(integer) < 0) {
					return expf_internal(
						x * 2.3025850929940456840179914546843642076011014886287729760333279009F);
				}
				else {
					auto out = 10;
					for(int i = 1; i < -integer; ++i) {
						out *= 10;
					}
					return 1.0F / static_cast<float>(out);
				}
			}
			else if(x - static_cast<float>(integer) > 0) {
				return expf_internal(
					x * 2.3025850929940456840179914546843642076011014886287729760333279009F);
			}
			else if(x == 0.0F) {
				return 1.0F;
			}
			else {
				auto out = 10;
				for(int i = 1; i < integer; ++i) {
					out *= 10;
				}

				return static_cast<float>(out);
			}
		}

		/// @brief Fast approximation calculation of base^exponent
		///
		/// @param base - The base to use
		/// @param exponent - The exponent to use
		/// @return - base^exponent
		[[nodiscard]] inline static constexpr auto
		powf_internal(float base, float exponent) noexcept -> float {
			return pow2f_internal(exponent * log2f_internal(base));
		}

		/// @brief Calculates the mantissa and exponent of `x`,
		/// in the representation x = mantissa * 2^exponent
		///
		/// @param x - The value to calculate the mantissa and exponent for
		/// @param e - The `int` to store the exponent in
		/// @return - The mantissa
		[[nodiscard]] inline static constexpr auto
		frexp_internal(double x, int* e) noexcept -> double {
			union {
				double d;
				uint64_t i;
			} y = {x};
			auto ee = static_cast<int>(y.i >> 52U & 0x7ffU); // NOLINT

			if(!static_cast<bool>(ee)) {
				if(static_cast<bool>(x)) {
					x = frexp(x * 0x1p64, e);
					*e -= 64;
				}
				else {
					*e = 0;
				}
				return x;
			}
			else if(ee == 0x7ffU) {
				return x;
			}

			*e = ee - 0x3fe;
			y.i &= 0x800fffffffffffffULL; // NOLINT
			y.i |= 0x3fe0000000000000ULL; // NOLINT
			return y.d;					  // NOLINT
		}

		/// @brief Helper function for `exp`; Don't use on its own
		///
		/// @param x - The exponent
		/// @return - e^x
		[[nodiscard]] inline static constexpr auto exp_helper(double x) noexcept -> double {
			auto numerator = 1680 + x * (840 + x * (180 + x * (20 + x)));
			auto denominator = 1680 + x * (-840 + x * (180 + x * (-20 + x)));
			return numerator / denominator;
		}

		/// @brief Fast approximiation calculation of e^x
		///
		/// @param x - The exponent
		/// @return - e^x
		[[nodiscard]] inline static constexpr auto exp_internal(double x) noexcept -> double {
			const auto condition = x < -3.0 || x > 2.0;
			if(condition) {
				const auto multiplier
					= static_cast<double>(x < 0.0)
						  * 0.1353352832366126918939994949724844034076315459095758814681588726
					  + static_cast<double>(x >= 0.0)
							* 7.3890560989306502272304274605750078131803155705518473240871278225;
				const auto subtractor
					= static_cast<double>(x < 0.0) * -2.0 + static_cast<double>(x > 0.0) * 2.0;
				return exp_internal(x - subtractor) * multiplier;
			}
			else {
				return exp_helper(x);
			}
		}

		/// @brief Helper function to ln. Performs ln(x + 1), accurate in -0.8 <= x <= 5
		///
		/// @param x The input
		/// @return - ln(x + 1)
		[[nodiscard]] inline static constexpr auto lnXPlus1(double x) noexcept -> double {
			auto numerator = x * (7560 + x * (15120 + x * (9870 + x * (2310 + x * 137))));
			auto denominator = 7560 + x * (18900 + x * (16800 + x * (6300 + x * (900 + 30 * x))));
			return numerator / denominator;
		}

		/// @brief Fast approximation calculation of ln(x)
		///
		/// @param x - The input
		/// @return - ln(x)
		[[nodiscard]] inline static constexpr auto ln_internal(double x) noexcept -> double {
			// subtract one, because we'll be using an ln(x + 1) approximation
			double in = x - 1.0;
			// if we're outside the accurate range, we'll reduce to the accurate range
			auto condition = static_cast<double>(in >= 5.0 || in <= -0.5);
			auto divVal = in >= 5.0 ? 5.0 : (in <= -0.5 ? -0.5 : 1.0);
			// if outside the accurate range, reduce to the accurate range and then perform the
			// approximation, otherwise, just perform the approximation 2.2302585 = ln(10)
			return condition * (lnXPlus1(((in + 1.0) / (divVal + 1.0)) - 1.0) + lnXPlus1(divVal))
				   + (1.0 - condition) * lnXPlus1(in);
		}

		/// @brief Fast approximation calculation of log_2(x)
		///
		/// @param x - The input
		/// @return - log_2(x)
		[[nodiscard]] inline static constexpr auto log2_internal(double x) noexcept -> double {
			return ln_internal(x)
				   / 0.6931471805599453094172321214581765680755001343602552541206800094;
		}

		/// @brief Fast approximation calculation of log_10(x)
		///
		/// @param x - The input
		/// @return - log_10(x)
		[[nodiscard]] inline static constexpr auto log10_internal(double x) noexcept -> double {
			return ln_internal(x)
				   / 2.3025850929940456840179914546843642076011014886287729760333279009;
		}

		/// @brief Fast approximation calculation of 2^x
		///
		/// @param x - The exponent
		/// @return 2^x
		[[nodiscard]] inline static constexpr auto pow2_internal(double x) noexcept -> double {
			auto integer = static_cast<int>(x);
			if(x < 0.0) {
				if(x - static_cast<double>(integer) < 0) {
					return exp_internal(
						x * 0.6931471805599453094172321214581765680755001343602552541206800094);
				}
				else {
					auto out = 2;
					for(int i = 1; i < -integer; ++i) {
						out *= 2;
					}
					return 1.0 / static_cast<double>(out);
				}
			}
			else if(x - static_cast<double>(integer) > 0) {
				return exp_internal(
					x * 0.6931471805599453094172321214581765680755001343602552541206800094);
			}
			else if(x == 0.0) {
				return 1.0;
			}
			else {
				auto out = 2;
				for(int i = 1; i < integer; ++i) {
					out *= 2;
				}

				return static_cast<double>(out);
			}
		}

		/// @brief Fast approximation calculation of 10^x
		///
		/// @param x - The exponent
		/// @return 10^x
		[[nodiscard]] inline static constexpr auto pow10_internal(double x) noexcept -> double {
			auto integer = static_cast<int>(x);
			if(x < 0.0) {
				if(x - static_cast<double>(integer) < 0) {
					return exp_internal(
						x * 2.3025850929940456840179914546843642076011014886287729760333279009);
				}
				else {
					auto out = 10;
					for(int i = 1; i < -integer; ++i) {
						out *= 10;
					}
					return 1.0 / static_cast<double>(out);
				}
			}
			else if(x - static_cast<double>(integer) > 0) {
				return exp_internal(
					x * 2.3025850929940456840179914546843642076011014886287729760333279009);
			}
			else if(x == 0.0) {
				return 1.0;
			}
			else {
				auto out = 10;
				for(int i = 1; i < integer; ++i) {
					out *= 10;
				}

				return static_cast<double>(out);
			}
		}

		/// @brief Fast approximation calculation of base^exponent
		///
		/// @param base - The base to use
		/// @param exponent - The exponent to use
		/// @return - base^exponent
		[[nodiscard]] inline static constexpr auto
		pow_internal(double base, double exponent) noexcept -> double {
			return pow2_internal(exponent * log2_internal(base));
		}
	};
} // namespace hyperion::math
