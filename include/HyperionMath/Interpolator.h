#pragma once

#include <gsl/gsl>
#include <type_traits>

#include "Exponentials.h"
#include "HyperionUtils/Concepts.h"
#include "HyperionUtils/Macros.h"
#include "HyperionUtils/OptionAndResult.h"

namespace hyperion::math {
	using utils::None;
	using utils::Option;
	using utils::Some;
	using utils::concepts::FloatingPoint;
	/// @brief The possible kinds of value interpolation possible by `Interpolator`
	enum class InterpolationType
	{
		Linear = 0,
		Logarithmic,
		Exponential
	};

	IGNORE_PADDING_START
	/// @brief Interpolates from a starting value to a final value in the way prescribed by `Type`
	/// @note `T` must be a floating point type
	/// @note The start value must be in [0, target value] and the target value must be > 0
	///
	/// @tparam T - The floating point type to perform operations in
	/// @tparam Type - The type of interpolation
	template<FloatingPoint T, InterpolationType Type = InterpolationType::Linear>
	class Interpolator {
	  public:
		static constexpr T DEFAULT_TARGET_VALUE = gsl::narrow_cast<T>(1.0);
		static constexpr T DEFAULT_INITIAL_VALUE = gsl::narrow_cast<T>(0.0);
		static constexpr T DEFAULT_TRANSITION_LENGTH = gsl::narrow_cast<T>(0.1);
		static constexpr size_t DEFAULT_SAMPLE_RATE = 44100ULL;

		/// @brief Creates a default `Interpolator`
		constexpr Interpolator() noexcept = default;

		/// @brief Creates an `Interpolator` with the given target value
		///
		/// @param targetValue - The target final value to interpolate to
		constexpr explicit Interpolator(T targetValue) noexcept
			: m_target_value(targetValue),
			  mLinearTransitionStep(
				  (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds)),
			  m_samples_to_transition(get_samples_to_transition()) {
		}

		/// @brief Creates an `Interpolator` with the given target value and sample rate
		///
		/// @param targetValue - The target final value to interpolate to
		/// @param sampleRate - The sample rate to process at
		constexpr explicit Interpolator(T targetValue, size_t sampleRate) noexcept
			: m_sample_rate(sampleRate), m_target_value(targetValue),
			  mLinearTransitionStep(
				  (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds)),
			  m_samples_to_transition(get_samples_to_transition()) {
		}

		/// @brief Creates an `Interpolator` with the given target and initial values, and sample
		/// rate
		///
		/// @param targetValue - The target final value to interpolate to
		/// @param initial_value - The initial value to start interpolation from
		/// @param sampleRate - The sample rate to process at
		constexpr Interpolator(T targetValue, T initial_value, size_t sampleRate) noexcept
			: m_sample_rate(sampleRate), m_target_value(targetValue),
			  m_current_value(initial_value), m_initial_value(m_current_value),
			  mLinearTransitionStep(
				  (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds)),
			  m_samples_to_transition(get_samples_to_transition()) {
		}

		/// @brief Creates an `Interpolator` with the given target and initial values, time to
		/// interpolate over, and sample rate
		///
		/// @param targetValue - The target final value to interpolate to
		/// @param initial_value - The initial value to start interpolation from
		/// @param transitionLengthSeconds - The transition time to interpolate over
		/// @param sampleRate - The sample rate to process at
		constexpr Interpolator(T targetValue,
							   T initial_value,
							   T transitionLengthSeconds,
							   size_t sampleRate) noexcept
			: m_sample_rate(sampleRate), m_target_value(targetValue),
			  m_current_value(initial_value), m_initial_value(m_current_value),
			  m_transition_lengthSeconds(transitionLengthSeconds),
			  mLinearTransitionStep(
				  (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds)),
			  m_samples_to_transition(get_samples_to_transition()) {
		}
		constexpr Interpolator(const Interpolator& interpolator) noexcept = default;
		constexpr Interpolator(Interpolator&& interpolator) noexcept = default;
		~Interpolator() noexcept = default;

		/// @brief Gets the next value in the interpolation sequence
		/// If `currentSample` is given, gets the value in the sequence for the given sample index
		/// @note This is only available for `Interpolator`s of type
		/// `InterpolationType::Logarithmic` or `InterpolationType::Exponential`
		/// @param currentSample - The (optional) sample in the sequence to get the value for
		///
		/// @return - The interpolation value
		inline auto get_next_value(Option<size_t> currentSample = None()) noexcept -> T {
			if(currentSample.is_some()) {
				m_current_transition_sample = currentSample.unwrap();
			}

			if(m_current_transition_sample <= m_samples_to_transition) {
				m_current_value = interpolate(m_current_transition_sample);
				m_current_transition_sample++;
			}

			return m_current_value;
		}

		/// @brief Resets the `Interpolator`.
		/// - If `initial_value` is given, the `Interpolator` will start at the given one instead of
		/// `DEFAULT_INITIAL_VALUE`
		/// - If `transitionLengthSeconds` is also given, the `Interpolator` will interpolate over
		/// that length of time instead of the current one
		///
		/// @param initial_value - The initial value to start interpolation from
		/// @param transitionLengthSeconds - The transition time to interpolate over
		inline auto
		reset(Option<T> initial_value, Option<T> transitionLengthSeconds) noexcept -> void {
			if(initial_value.is_some()) {
				m_current_value = initial_value.unwrap();
				m_initial_value = m_current_value;
			}
			else {
				m_current_value = DEFAULT_INITIAL_VALUE;
				m_initial_value = m_current_value;
			}

			if(transitionLengthSeconds.is_some()) {
				m_transition_lengthSeconds = transitionLengthSeconds.unwrap();
			}

			mLinearTransitionStep
				= (m_target_value - m_current_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds);
			m_samples_to_transition = get_samples_to_transition();
			m_current_transition_sample = 0ULL;
		}

		/// @brief Sets the target value to interpolate to, to the given one
		///
		/// @param targetValue - The value to interpolate to
		constexpr inline auto set_target(T targetValue) noexcept -> void {
			m_target_value = targetValue;
			m_initial_value = m_current_value;
			mLinearTransitionStep
				= (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds);
			m_samples_to_transition = get_samples_to_transition();
		}

		/// @brief Sets the sample rate to use for interpolation
		///
		/// @param sampleRate - The sample rate to use for interpolation
		inline auto set_sample_rate(size_t sampleRate) noexcept -> void {
			m_sample_rate = sampleRate;
			mLinearTransitionStep
				= (m_target_value - m_initial_value)
				  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds);
			m_samples_to_transition = get_samples_to_transition();
			m_current_transition_sample = 0ULL;
		}

		constexpr auto
		operator=(const Interpolator& interpolator) noexcept -> Interpolator& = default;
		constexpr auto operator=(Interpolator&& interpolator) noexcept -> Interpolator& = default;

	  private:
		static constexpr T LOG_TIME_FACTOR = gsl::narrow_cast<T>(5.0);
		static constexpr T EXP_TIME_FACTOR = gsl::narrow_cast<T>(0.693);

		size_t m_sample_rate = DEFAULT_SAMPLE_RATE;
		T m_target_value = DEFAULT_TARGET_VALUE;
		T m_current_value = DEFAULT_INITIAL_VALUE;
		T m_initial_value = DEFAULT_INITIAL_VALUE;
		T m_transition_lengthSeconds = DEFAULT_TRANSITION_LENGTH;
		T mLinearTransitionStep
			= (m_target_value - m_initial_value)
			  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds);
		size_t m_samples_to_transition = get_samples_to_transition();
		size_t m_current_transition_sample = 0;

		/// @brief Gets the interpolated value for the given sample
		///
		/// @param sample - The sample to get the interpolated value for
		///
		/// @return The interpolated value
		inline auto interpolate(size_t sample) noexcept -> T {
			if constexpr(Type == InterpolationType::Linear) {
				return linear_interpolation(sample);
			}
			else if constexpr(Type == InterpolationType::Logarithmic) {
				return log_interpolation(sample);
			}
			else {
				return exp_interpolation(sample);
			}
		}

		inline auto linear_interpolation(size_t sample) noexcept -> T {
			return m_initial_value + (mLinearTransitionStep * gsl::narrow_cast<T>(sample));
		}

		/// @brief Performs "logarithmic" interpolation.
		/// In reality, this is a pseudo logarithmic shape:
		///
		/// y = transition_factor *
		/// [1 + (peak_factor - 1) * (-e^{1 - t / (transtionLength / time_factor) } ) ]
		///
		/// where t = sample / sampleRate
		///
		/// which has a  horizontal asymptote at  ~ the target value and more pronouced slope
		///
		/// @param sample - The sample to get the interpolation value for
		///
		/// @return - The interpolated value
		inline auto log_interpolation(size_t sample) noexcept -> T {
			return (m_initial_value - m_target_value)
					   * math::Exponentials::exp(-gsl::narrow_cast<T>(sample)
												 / (gsl::narrow_cast<T>(m_sample_rate)
													* m_transition_lengthSeconds / LOG_TIME_FACTOR))
				   + m_target_value;
		}

		/// @brief Performs exponential interpolation:
		///
		/// y = e^{t / (transitionLength / time_factor) } - 1
		///
		/// where t = sample / sampleRate
		///
		/// @param sample The sample to get the interpolation value for
		///
		/// @return - The interpolated value
		inline auto exp_interpolation(size_t sample) noexcept -> T {
			return (m_target_value - m_initial_value)
					   * (math::Exponentials::exp(
							  gsl::narrow_cast<T>(sample)
							  / (gsl::narrow_cast<T>(m_sample_rate) * m_transition_lengthSeconds
								 / EXP_TIME_FACTOR))
						  - gsl::narrow_cast<T>(1.0))
				   + m_initial_value;
		}

		/// @brief Gets the number of samples necessary to fully complete the interpolation sequence
		///
		/// @return The number of samples in the interpolation sequence
		inline auto get_samples_to_transition() noexcept -> size_t {
			return gsl::narrow_cast<size_t>(gsl::narrow_cast<T>(m_sample_rate)
											* m_transition_lengthSeconds);
		}
	};
	IGNORE_PADDING_STOP

} // namespace hyperion::math
