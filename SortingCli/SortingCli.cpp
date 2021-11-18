#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include <fstream>
#include <sstream>

#include "list_sequence.h"
#include "array_sequence.h"
#include "Sorting.h"
#include "utils.h"
#include "testing.h"
#include <boost/mpl/vector.hpp>

using namespace collections;
using namespace sorting;
using namespace testing;
namespace rng = std::ranges;


void test_sequence_simple()
{
	sequence ls = dynamic_array<int>(1000);
	srand(time(nullptr));
	rng::generate(ls, []() { return rand(); });
	MergeSort(ls.begin(), ls.end());
	assert(rng::is_sorted(ls), "not sorted");
}

void test_sequence_simple2()
{
	stress::code_iteration(test_sequence_simple, 1000)();
}

template<typename T>
using b = boost::mpl::vector<std::vector<T>, std::list<T>, linked_list<T>, dynamic_array<T>, array_sequence<T>, list_sequence<T>>;
template<typename T>
using c = boost::mpl::vector<std::vector<T>, std::deque<T>>;
using a = boost::mpl::vector<int, float, std::pair<int, int>>;

template<typename Lambda>
void type_combined_test(Lambda lambda)
{
	testing::type_combined_test_template<Lambda, b, a>(lambda);
}

template<typename Lambda>
void type_combined_test2(Lambda lambda)
{
	testing::type_combined_test_template<Lambda, c, a>(lambda);
}

void proof_of_work_bidirectional()
{
	type_combined_test([&]<typename T, typename C>(T t, C c)
		{
			std::vector sorts{
				MergeSort<typename C::iterator,	   std::less<T>>,
				QuickSort<typename C::iterator,	   std::less<T>>,
				BubbleSort<typename C::iterator,   std::less<T>>,
				SelectionSort<typename C::iterator,std::less<T>>
			};
			auto random = random_generator<T>();
			C container(static_cast<size_t>(100));
			for (const auto& sort : sorts)
			{
				std::generate(std::begin(container), std::end(container), random);
				sort(std::begin(container), std::end(container), std::less<T>{});
				testing::assert(std::is_sorted(std::begin(container), std::end(container)));
			}
		});
}

void proof_of_work_bidirectional_reversed()
{
	type_combined_test([&]<typename T, typename C>(T t, C c)
		{
			std::vector sorts{
				//MergeSort<typename C::iterator,	   std::greater_equal<T>>,
				QuickSort<typename C::iterator,	   std::greater_equal<T>>,
				BubbleSort<typename C::iterator,   std::greater_equal<T>>,
				SelectionSort<typename C::iterator,std::greater_equal<T>>
			};
			auto random = random_generator<T>();
			C container(static_cast<size_t>(100));
			for (const auto& sort : sorts)
			{
				std::generate(std::begin(container), std::end(container), random);
				sort(std::begin(container), std::end(container), std::greater_equal<T>{});
				testing::assert(std::is_sorted(std::begin(container), std::end(container), std::greater_equal<T>{}));
			}
		});
}

void proof_of_work_random_access()
{
	type_combined_test2([&]<typename T, typename C>(T t, C c)
		{
			std::vector sorts{ RandomSort<C::iterator>};
			auto random = random_generator<T>();
			C container(static_cast<size_t>(10));
			for (const auto& sort : sorts)
			{
				std::generate(std::begin(container), std::end(container), random);
				sort(std::begin(container), std::end(container), std::less<T>{});
				testing::assert(std::is_sorted(std::begin(container), std::end(container)));
			}
		});
}

// test_name;container_type;inner_type;elements;total_time
void profile_bubble_sort_with_100_int(std::ostream& file)
{
	std::vector<int> container(static_cast<size_t>(100));
	auto random = random_generator<int>();
	std::generate(std::begin(container), std::end(container), random);
	file << "vector" << "," << "int" << "," << "100" << ",";
	{
		profiler p("",std::cerr, [&file](long long ms) {file << ms; });
		BubbleSort(std::begin(container), std::end(container));
	}
	file << std::endl;
}

#define SORT_PROFILE_CSV(FILE, SORT, CONTAINER, TYPE, LENGTH)					\
{																				\
	CONTAINER<TYPE> container(static_cast<size_t>(LENGTH));						\
	auto random = random_generator<TYPE>();										\
	std::generate(std::begin(container), std::end(container), random);			\
	FILE << #SORT << "," << #CONTAINER << "," << #TYPE << "," << LENGTH << ","; \
	{																			\
		profiler p("",std::cerr, [&](long long ms) { FILE << ms; });			\
		SORT(std::begin(container), std::end(container));						\
	}																			\
	FILE << std::endl;															\
}



int main()
{

	using namespace std::chrono_literals;

	auto tr = test_runner();
	tr RUN_TEST(proof_of_work_bidirectional);

	if (true)
	{
		std::ofstream file(R"(C:\Users\Ariel\Desktop\profiling.csv)");
		file << "SortType,Container,InnerType,Elements,Time" << std::endl;

		for (int i : {10, 100, 500, 1000, 2000, 5000, 7000, 10000, 20000, 50000, 70000, 100000, 200000, 500000, 700000, 1000000, 2000000, 10000000})
		{
			if (i < 100000)
			{
				SORT_PROFILE_CSV(file, BubbleSort, std::vector, int, i)
				SORT_PROFILE_CSV(file, BubbleSort, std::list, int, i)
				SORT_PROFILE_CSV(file, BubbleSort, dynamic_array, int, i)
				SORT_PROFILE_CSV(file, BubbleSort, linked_list, int, i)
				SORT_PROFILE_CSV(file, BubbleSort, std::vector, std::string, i)
				SORT_PROFILE_CSV(file, BubbleSort, std::list, std::string, i)
				SORT_PROFILE_CSV(file, BubbleSort, dynamic_array, std::string, i)
				SORT_PROFILE_CSV(file, BubbleSort, linked_list, std::string, i)

				SORT_PROFILE_CSV(file, SelectionSort, std::vector, int, i)
				SORT_PROFILE_CSV(file, SelectionSort, std::list, int, i)
				SORT_PROFILE_CSV(file, SelectionSort, dynamic_array, int, i)
				SORT_PROFILE_CSV(file, SelectionSort, linked_list, int, i)
				SORT_PROFILE_CSV(file, SelectionSort, std::vector, std::string, i)
				SORT_PROFILE_CSV(file, SelectionSort, std::list, std::string, i)
				SORT_PROFILE_CSV(file, SelectionSort, dynamic_array, std::string, i)
				SORT_PROFILE_CSV(file, SelectionSort, linked_list, std::string, i)
			}

				SORT_PROFILE_CSV(file, MergeSort, std::vector, int, i)
				SORT_PROFILE_CSV(file, MergeSort, std::list, int, i)
				SORT_PROFILE_CSV(file, MergeSort, dynamic_array, int, i)
				SORT_PROFILE_CSV(file, MergeSort, linked_list, int, i)
				SORT_PROFILE_CSV(file, MergeSort, std::vector, std::string, i)
				SORT_PROFILE_CSV(file, MergeSort, std::list, std::string, i)
				SORT_PROFILE_CSV(file, MergeSort, dynamic_array, std::string, i)
				SORT_PROFILE_CSV(file, MergeSort, linked_list, std::string, i)

				SORT_PROFILE_CSV(file, QuickSort, std::vector, int, i)
				SORT_PROFILE_CSV(file, QuickSort, std::list, int, i)
				SORT_PROFILE_CSV(file, QuickSort, dynamic_array, int, i)
				SORT_PROFILE_CSV(file, QuickSort, linked_list, int, i)
				SORT_PROFILE_CSV(file, QuickSort, std::vector, std::string, i)
				SORT_PROFILE_CSV(file, QuickSort, std::list, std::string, i)
				SORT_PROFILE_CSV(file, QuickSort, dynamic_array, std::string, i)
				SORT_PROFILE_CSV(file, QuickSort, linked_list, std::string, i)

				SORT_PROFILE_CSV(file, std::sort, std::vector, int, i)
				//SORT_PROFILE_CSV(file, std::sort, std::list, int, i)
				SORT_PROFILE_CSV(file, std::sort, dynamic_array, int, i)
				//SORT_PROFILE_CSV(file, std::sort, linked_list, int, i)
				SORT_PROFILE_CSV(file, std::sort, std::vector, std::string, i)
				//SORT_PROFILE_CSV(file, std::sort, std::list, std::string, i)
				SORT_PROFILE_CSV(file, std::sort, dynamic_array, std::string, i)
				//SORT_PROFILE_CSV(file, std::sort, linked_list, std::string, i)
		}

		file.close();
	}
	return 0;
}
