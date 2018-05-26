// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. See License.txt in the project root for license information.

#ifndef __CF_RX_EXT_H__
#define __CF_RX_EXT_H__

#include <rxcpp/rx.hpp>
#include <rxcpp/subjects/rx-subject.hpp>

namespace System::RX
{
    //template<class T, class SourceOperator>
    //class Observable : rxcpp::observable<T, SourceOperator>
    //{
    //public:
    //
    //    Observable()
    //    {
    //    }
    //
    //    explicit Observable(const source_operator_type& o)
    //        : rxcpp::observable<T, SourceOperator>(o)
    //    {
    //    }
    //    explicit Observable(source_operator_type&& o)
    //        : rxcpp::observable<T, SourceOperator>(std::move(o))
    //    {
    //    }
    //};

    template<class T>
    class Subject
    {
        rxcpp::subjects::detail::multicast_observer<T> s;
        mutable rxcpp::observable<T, rxcpp::dynamic_observable<T>> o;

    public:

        typedef rxcpp::subscriber<T, rxcpp::observer<T, rxcpp::subjects::detail::multicast_observer<T>>> subscriber_type;
        typedef rxcpp::observable<T> observable_type;

        Subject()
            : s(rxcpp::composite_subscription())
            , o(rxcpp::dynamic_observable<T>([this](rxcpp::subscriber<T> const& o) {
                    s.add(s, std::move(o));
                }))
        {
        }
        explicit Subject(rxcpp::composite_subscription const& cs)
            : s(cs)
            , o(rxcpp::dynamic_observable<T>([this](rxcpp::subscriber<T> const& o) {
                    s.add(s, std::move(o));
                }))
        {
        }

        void Reset()
        {
            if (!s.get_subscriber().is_subscribed())
                s = rxcpp::subjects::detail::multicast_observer<T>(rxcpp::composite_subscription());
        }

        bool has_observers() const
        {
            return s.has_observers();
        }

        rxcpp::composite_subscription& get_subscription() const
        {
            return s.get_ref_subscription();
        }

        //subscriber_type get_subscriber() const {
        //    return s.get_subscriber();
        //}

        rxcpp::subjects::detail::multicast_observer<T>& get_observer() { return s; }

        rxcpp::observable<T, rxcpp::dynamic_observable<T>>& get_observable() const { return o; }
    };
}

#endif //! __CF_RX_EXT_H__