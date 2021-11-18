#pragma once

#include <functional>
#include <random>
#include <boost/mpl/for_each.hpp>

#include "profiler.h"
#include "test_runner.h"
#include "stress_tests.h"

namespace testing
{
    using test_type = std::function<void(void)>;


	template <typename Lambda, typename InnerTypes>
	void typed_test_template(Lambda lambda)
	{
		boost::mpl::for_each<InnerTypes>([lambda]<typename Type>(Type type)
		{
			lambda(type);
		});
	}

	template <typename Lambda, template<typename ...arg> typename OuterTypes , typename InnerTypes>
	void type_combined_test_template(Lambda lambda)
	{
		boost::mpl::for_each<InnerTypes>([lambda]<typename Type>(Type type)
		{
			boost::mpl::for_each<OuterTypes<Type>>([lambda, type]<typename Container>(Container cont)
			{
				lambda(type, cont);
			});
		});
	}

	template <typename Lambda, typename OuterTypes, typename InnerTypes>
	void type_double_combined_test_template(Lambda lambda)
	{
		boost::mpl::for_each<InnerTypes>([lambda]<typename Type>(Type type)
		{
			boost::mpl::for_each<OuterTypes<Type>>([lambda, type]<typename Container>(Container cont)
			{
				boost::mpl::for_each<OuterTypes<Type>>([lambda, type, cont]<typename Container2>(Container2 cont2)
				{
					lambda(type, cont, cont2);
				});
			});
		});
	}

	template <typename T>
	struct random_generator_impl
	{
		std::uniform_int_distribution<> range;
		std::mt19937 gen;
		auto random()
		{
			return range(gen);
		}

		random_generator_impl()
			: range(), gen(time(nullptr)) {}

		T operator()()
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				return std::to_string(random());
			} else
			{
				return static_cast<T>(random());
			}
		}
	};


	template <typename T, typename U>
	struct random_generator_impl<std::pair<T, U>> {
		random_generator_impl<T> genT;
		random_generator_impl<U> genU;
		std::pair<T, U> operator()()
		{
			return std::pair{ genT(), genU() };
		}
	};


	template <typename T>
	using random_generator = random_generator_impl<T>;

}
