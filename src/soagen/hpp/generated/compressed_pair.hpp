//#---------------------------------------------------------------------------------------------------------------------
//#     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//#---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../meta.hpp"
#include "../header_start.hpp"

/// @cond

namespace soagen
{
	namespace detail
	{
#define SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(T, name, expression)                                                       \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr T& name()& noexcept                                                                                      \
	{                                                                                                                  \
		return static_cast<T&>(expression);                                                                            \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr const T& name() const& noexcept                                                                          \
	{                                                                                                                  \
		return static_cast<const T&>(expression);                                                                      \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr T&& name()&& noexcept                                                                                    \
	{                                                                                                                  \
		return static_cast<T&&>(expression);                                                                           \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr const T&& name() const&& noexcept                                                                        \
	{                                                                                                                  \
		return static_cast<const T&&>(expression);                                                                     \
	}                                                                                                                  \
	static_assert(true)

		// primary template - neither element can be a base
		template <typename First,
				  typename Second,
				  bool FirstCanBeBase  = std::is_empty_v<First> && !std::is_final_v<First>,
				  bool SecondCanBeBase = std::is_empty_v<Second> && !std::is_final_v<Second>>
		class compressed_pair_base
		{
		  private:
			static_assert(!FirstCanBeBase);
			static_assert(!SecondCanBeBase);
			SOAGEN_NO_UNIQUE_ADDRESS First first_;
			SOAGEN_NO_UNIQUE_ADDRESS Second second_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: first_{ static_cast<F&&>(first_init) },
				  second_{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, first_);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, second_);
		};

		// secondary template - First is a base
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, true, false> //
			: private First
		{
		  private:
			SOAGEN_NO_UNIQUE_ADDRESS Second second_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: First{ static_cast<F&&>(first_init) },
				  second_{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, *this);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, second_);
		};

		// secondary template - Second is a base
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, false, true> //
			: private Second
		{
		  private:
			SOAGEN_NO_UNIQUE_ADDRESS First first_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: Second{ static_cast<S&&>(second_init) },
				  first_{ static_cast<F&&>(first_init) }

			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, first_);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, *this);
		};

		// secondary template - both are bases
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, true, true> //
			: private First, private Second
		{
		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: First{ static_cast<F&&>(first_init) },
				  Second{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, *this);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, *this);
		};

#undef SOAGEN_COMPRESSED_PAIR_BASE_DEFAULTS
#undef SOAGEN_COMPRESSED_PAIR_BASE_GETTERS
	}
	template <typename First, typename Second>
	class SOAGEN_EMPTY_BASES compressed_pair //
		: public detail::compressed_pair_base<First, Second>
	{
	  private:
		using base = detail::compressed_pair_base<First, Second>;

	  public:
		using first_type  = First;
		using second_type = Second;

		SOAGEN_NODISCARD_CTOR
		compressed_pair() = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(const compressed_pair&) = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(compressed_pair&&) = default;

		compressed_pair& operator=(const compressed_pair&) = default;

		compressed_pair& operator=(compressed_pair&&) = default;

#if SOAGEN_DOXYGEN
#else
		using detail::compressed_pair_base<First, Second>::compressed_pair_base; // inherit constructor
#endif

	  private:
		template <size_t I, typename T>
		SOAGEN_PURE_INLINE_GETTER
		static constexpr decltype(auto) do_get(T&& cp) noexcept
		{
			static_assert(I <= 1);
			if constexpr (I == 0)
				return static_cast<T&&>(cp).first();
			else
				return static_cast<T&&>(cp).second();
		}

	  public:
		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr auto& get() & noexcept
		{
			return do_get<I>(*this);
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr auto&& get() && noexcept
		{
			return do_get<I>(static_cast<compressed_pair&&>(*this));
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const auto& get() const& noexcept
		{
			return do_get<I>(*this);
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const auto&& get() const&& noexcept
		{
			return do_get<I>(static_cast<const compressed_pair&&>(*this));
		}

		SOAGEN_HIDDEN_CONSTRAINT(std::is_swappable_v<F>&& std::is_swappable_v<S>,
								 typename F = First,
								 typename S = Second)
		void swap(compressed_pair& other) noexcept(std::is_nothrow_swappable_v<F>&& std::is_nothrow_swappable_v<S>)
		{
			using std::swap;
			swap(base::first(), other.first());
			swap(base::second(), other.second());
		}
	};

	template <typename F, typename S>
	compressed_pair(F&&, S&&) -> compressed_pair<remove_cvref<F>, remove_cvref<S>>;

	SOAGEN_CONSTRAINED_TEMPLATE(std::is_swappable_v<F>&& std::is_swappable_v<S>, typename F, typename S)
	SOAGEN_ALWAYS_INLINE
	void swap(compressed_pair<F, S>& lhs,
			  compressed_pair<F, S>& rhs) noexcept(std::is_nothrow_swappable_v<F>&& std::is_nothrow_swappable_v<S>)
	{
		lhs.swap(rhs);
	}

}

namespace std
{
	template <typename First, typename Second>
	struct tuple_size<soagen::compressed_pair<First, Second>>
	{
		static constexpr size_t value = 2;
	};

	template <size_t I, typename First, typename Second>
	struct tuple_element<I, soagen::compressed_pair<First, Second>>
	{
		static_assert(I < 2);
		using type = std::conditional_t<I == 1, Second, First>;
	};

}

/// @endcond

#include "../header_end.hpp"
