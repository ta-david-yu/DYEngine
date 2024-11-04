#include <algorithm>
#include <iterator>

namespace DYE::Algorithm
{
    template<class Iterator>
    void InsertionSort(Iterator first, Iterator last)
    {
        for (Iterator it = first; it != last; ++it)
        {
            std::rotate(std::upper_bound(first, it, *it), it, std::next(it));
        }
    }

    template<class Iterator, class Compare>
    void InsertionSort(Iterator first, Iterator last, Compare compare)
    {
        for (Iterator it = first; it != last; ++it)
        {
            std::rotate(std::upper_bound(first, it, *it, compare), it, std::next(it));
        }
    }
}