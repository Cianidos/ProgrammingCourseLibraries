#pragma once
#include "linked_list.h"


namespace collections
{
    namespace sequence_companion
    {
        template <typename T>
        void concat_containers_mut(linked_list<T>& lhs, linked_list<T> rhs)
        {
            lhs.concat(std::move(rhs));
        }

        template<typename T>
        std::pair<linked_list<T>, linked_list<T>> cut_container_mut
        (typename linked_list<T>::iterator index, linked_list<T>& cont)
        {
            return cont.cut(index);
        }
    }
}

#include "sequence.h"

namespace collections
{

    template<typename T>
    using list_sequence = collections::sequence<linked_list<T>>;

}
