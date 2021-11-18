#pragma once
#include <algorithm>
#include <numeric>
#include <vector>
#include <iterator>
#include <concepts>
#include <functional>
#include <random>


namespace sorting
{
	template <typename  Iterator>
	using iter_value = typename std::iterator_traits<Iterator>::value_type;

	template <typename Func, typename  Iterator>
	concept iter_compare = std::relation<Func, const iter_value<Iterator>&, const iter_value<Iterator>&>;



	template <std::bidirectional_iterator RandomIt,
	iter_compare<RandomIt> Comparator = std::less<iter_value<RandomIt>>>
	void MergeSort(RandomIt begin_it, RandomIt end_it, Comparator cmp = std::less<iter_value<RandomIt>>{})
	{
		using namespace std;
		const int range_length = distance(begin_it, end_it);
		if (range_length < 2) {
			return;
		}

		auto one_third = next(begin_it, range_length / 3);
		auto two_third = next(begin_it, range_length * 2 / 3);

		MergeSort(begin_it, one_third);
		MergeSort(one_third, two_third);
		MergeSort(two_third, end_it);

		inplace_merge( begin_it, one_third, two_third, cmp); 
		inplace_merge(begin_it, two_third, end_it, cmp);
	}



	template<std::bidirectional_iterator Iterator,
	iter_compare<Iterator> Comparator = std::less<iter_value<Iterator>>>
	void QuickSort(Iterator begin_it, Iterator end_it, Comparator cmp = std::less<iter_value<Iterator>>{})
	{
		using namespace std;
		if (distance(begin_it, end_it) > 1)
		{
			Iterator pivot = begin_it, middle_it = next(begin_it);

			for (Iterator i = middle_it; i != end_it; ++i) 
				if (cmp(*i, *pivot)) 
					iter_swap(i, middle_it++);

			iter_swap(begin_it, prev(middle_it));

			QuickSort(begin_it, prev(middle_it));
			QuickSort(middle_it, end_it);
		}
	}


	template<std::bidirectional_iterator Iterator,
	iter_compare<Iterator> Comparator = std::less<iter_value<Iterator>>>
	void BubbleSort(Iterator begin_it, Iterator end_it, Comparator cmp = std::less<iter_value<Iterator>>{})
	{
		for (Iterator i = begin_it; i != end_it; ++i)
			for (Iterator j = begin_it; j != i; ++j)
				if (cmp(*i, *j))
					std::iter_swap(i, j);
	}

	template<std::bidirectional_iterator Iterator,
	iter_compare<Iterator> Comparator = std::less<iter_value<Iterator>>>
	void SelectionSort (Iterator begin_it, Iterator end_it, Comparator cmp = std::less<iter_value<Iterator>>{})
	{
		for (Iterator i = begin_it; i != end_it; ++i)
			std::iter_swap(i, std::min_element(i, end_it, cmp));
	}

	template<std::random_access_iterator Iterator,
	iter_compare<Iterator> Comparator = std::less<iter_value<Iterator>>>
	void RandomSort(Iterator begin_it, Iterator end_it, Comparator cmp = std::less<iter_value<Iterator>>{})
	{
		std::random_device rd;
		std::mt19937 g(rd());

		while (!std::is_sorted(begin_it, end_it, cmp))
			std::shuffle(begin_it, end_it, g);
	}

}
