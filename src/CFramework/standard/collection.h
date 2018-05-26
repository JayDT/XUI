/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __Collection_H__
#define __Collection_H__

#include <Reflection/Runtime/Object.h>
#include <Reflection/Runtime/Variant.h>
#include <Reflection/Runtime/Type.h>
#include <standard/misc.h>
#include <standard/events.h>

namespace System
{
    namespace Collection
    {
        //
        // Summary:
        //     Describes the action that caused a System.Collections.Specialized.INotifyCollectionChanged.CollectionChanged
        //     event.
        enum class NotifyCollectionChangedAction
        {
            //
            // Summary:
            //     One or more items were added to the collection.
            Add = 0,
            //
            // Summary:
            //     One or more items were removed from the collection.
            Remove = 1,
            //
            // Summary:
            //     One or more items were replaced in the collection.
            Replace = 2,
            //
            // Summary:
            //     One or more items were moved within the collection.
            Move = 3,
            //
            // Summary:
            //     The content of the collection changed dramatically.
            Reset = 4,

            AfterRemove = 5
        };

        struct NotifyCollectionChangedEventArgs : System::Events::EventArg
        {
            NotifyCollectionChangedAction Action;
            int NewStartingIndex = 0;
            int OldStartingIndex = 0;

			// Currently Use Map
			System::Reflection::Variant NewValue;
			System::Reflection::Variant OldValue;
		};

        namespace Generic
        {
            struct IEnumerator : public virtual System::Reflection::meta::Object
            {
                virtual ~IEnumerator() = default;
                virtual handle Current() const = 0;
                virtual void Next() = 0;

                virtual bool operator==(IEnumerator const& right) const = 0;
                virtual bool operator!=(IEnumerator const& right) const = 0;
            };

            struct IEnumerable : public virtual System::Reflection::meta::Object
            {
                virtual ~IEnumerable() = default;
                typedef std::shared_ptr<IEnumerator> generic_iterator;
                virtual generic_iterator gbegin() = 0;
                virtual generic_iterator gend() = 0;
            };

            struct Iterator
                : System::Reflection::Object
                , System::EnableSharedFromThisEx<Iterator>
            {
                virtual ~Iterator() = default;
                virtual System::Reflection::Variant Current() const = 0;

                virtual void operator++() = 0;
                virtual void operator--() = 0;
                virtual void StepForward() = 0;
                virtual void StepBackward() = 0;
                virtual bool MoveNext() = 0;
                virtual void Reset() = 0;
                virtual bool IsFinished() = 0;

                virtual bool operator==(Iterator const& right) const = 0;
                virtual bool operator!=(Iterator const& right) const = 0;
            };

            struct IContainer : System::Reflection::Object
            {
                typedef std::shared_ptr<Iterator> iiterator;
                virtual iiterator GetGenericEnumerator() = 0;
                virtual iiterator ifind(System::Reflection::Variant const&) = 0;
                virtual size_t size() const = 0;
                virtual bool empty() const = 0;
                virtual void Clear() = 0;
                virtual void Add(System::Reflection::Variant const&) = 0;
                virtual void Add(System::Reflection::Variant const& key, System::Reflection::Variant const& value) = 0;
                virtual void Insert(int index, System::Reflection::Variant const& value) = 0;
                virtual void Remove(System::Reflection::Variant const&) = 0;
                virtual const System::Reflection::Type* GetValueType() const = 0;
                virtual size_t IndexOf(const System::Reflection::Variant&) const = 0;
                virtual System::Reflection::Variant ElementAt(int index) const = 0;
                virtual void UpdateAt(int index, const System::Reflection::Variant&) const = 0;
                virtual void RemoveAt(int index) const = 0;
                virtual void Replace(int oldindex, int index) const = 0;
                virtual bool Contains(System::Reflection::Variant const&) const = 0;
                virtual void QuickSort(std::function<bool(System::Reflection::Variant const&, System::Reflection::Variant const&)> const& func)
                {
                    throw System::NotImplementedException();
                }

                virtual ~IContainer() = default;

                mutable System::Events::EventHandler<NotifyCollectionChangedEventArgs> CollectionChanged;

            protected:
                /// <summary>
                /// Raises the <see cref="CollectionChanged"/> event with an add action.
                /// </summary>
                /// <param name="t">The items that were added.</param>
                /// <param name="index">The starting index.</param>
                void NotifyAdd(int index, System::Reflection::Variant const& value) const
                {
                    if (CollectionChanged)
                    {
                        NotifyCollectionChangedEventArgs e;
                        e.Action = NotifyCollectionChangedAction::Add;
                        e.NewStartingIndex = index;
                        e.NewValue = value;
                        CollectionChanged(const_cast<IContainer*>(this), e);
                    }
                }

                void NotifyReplace(int oldindex, int index) const
                {
                    if (CollectionChanged)
                    {
                        NotifyCollectionChangedEventArgs e;
                        e.Action = NotifyCollectionChangedAction::Replace;
                        e.OldStartingIndex = oldindex;
                        e.NewStartingIndex = index;
                        CollectionChanged(const_cast<IContainer*>(this), e);
                    }
                }

                /// <summary>
                /// Raises the <see cref="CollectionChanged"/> event with a remove action.
                /// </summary>
                /// <param name="t">The items that were removed.</param>
                /// <param name="index">The starting index.</param>
                void NotifyRemove(int index) const
                {
                    if (CollectionChanged)
                    {
                        NotifyCollectionChangedEventArgs e;
                        e.Action = NotifyCollectionChangedAction::Remove;
                        e.OldStartingIndex = index;
                        CollectionChanged(const_cast<IContainer*>(this), e);
                    }
                }

                /// <summary>
                /// Raises the <see cref="CollectionChanged"/> event with a remove after action.
                /// </summary>
                /// <param name="t">The items that were removed.</param>
                /// <param name="index">The starting index.</param>
                void NotifyAfterRemove(int index) const
                {
                    if (CollectionChanged)
                    {
                        NotifyCollectionChangedEventArgs e;
                        e.Action = NotifyCollectionChangedAction::AfterRemove;
                        e.OldStartingIndex = index;
                        CollectionChanged(const_cast<IContainer*>(this), e);
                    }
                }

				/// <summary>
				/// Raises the <see cref="CollectionChanged"/> event with an add action.
				/// </summary>
				/// <param name="t">The items that were added.</param>
				/// <param name="index">The starting index.</param>
				void NotifyValueAdd(System::Reflection::Variant const& index) const
				{
					if (CollectionChanged)
					{
						NotifyCollectionChangedEventArgs e;
						e.Action = NotifyCollectionChangedAction::Add;
						e.NewValue = std::move(index);
						CollectionChanged(const_cast<IContainer*>(this), e);
					}
				}

				/// <summary>
				/// Raises the <see cref="CollectionChanged"/> event with a remove action.
				/// </summary>
				/// <param name="t">The items that were removed.</param>
				/// <param name="index">The starting index.</param>
				void NotifyValueRemove(System::Reflection::Variant const&  index) const
				{
					if (CollectionChanged)
					{
						NotifyCollectionChangedEventArgs e;
						e.Action = NotifyCollectionChangedAction::Remove;
						e.OldValue = std::move(index);
						CollectionChanged(const_cast<IContainer*>(this), e);
					}
				}

                /// <summary>
                /// Raises the <see cref="CollectionChanged"/> event with a remove action.
                /// </summary>
                /// <param name="t">The items that were removed.</param>
                /// <param name="index">The starting index.</param>
                void NotifyClear() const
                {
                    if (CollectionChanged)
                    {
                        NotifyCollectionChangedEventArgs e;
                        e.Action = NotifyCollectionChangedAction::Reset;
                        CollectionChanged(const_cast<IContainer*>(this), e);
                    }
                }
            };

            struct IMapContainer : IContainer
            {
                // Type Safe
                virtual void Add(System::Reflection::Variant const& key, System::Reflection::Variant const& value) = 0;
                virtual const System::Reflection::Type* GetKeyType() const = 0;
                virtual void Insert(int index, System::Reflection::Variant const& value) { throw System::NotImplementedException(); }
                virtual void Replace(int oldindex, int index) const { throw System::NotImplementedException(); }
            };
        }
        template<typename TValue>
        struct IEnumerator : public Generic::IEnumerator
        {
            virtual TValue* ToPtr() const = 0;
            virtual TValue operator->() const = 0;
            virtual TValue operator*() const = 0;
            virtual operator TValue() const = 0;

            virtual bool operator==(Generic::IEnumerator const& right) const override { return Current() == right.Current(); }
            virtual bool operator!=(Generic::IEnumerator const& right) const override { return Current() != right.Current(); }
            virtual bool operator==(IEnumerator<TValue> const& right) const = 0;
            virtual bool operator!=(IEnumerator<TValue> const& right) const = 0;
            virtual IEnumerator<TValue>& operator++() = 0;
            virtual ~IEnumerator() = default;
        };

        template<typename TValue>
        struct IEnumerable : public virtual Generic::IEnumerable
        {
            typedef std::shared_ptr<IEnumerator<TValue>> iterator;
            virtual iterator begin() = 0;
            virtual iterator end() = 0;
            virtual ~IEnumerable() = default;
        };

        template<typename TValue>
        struct Iterator : public Generic::Iterator
        {
            typedef TValue& ReferenceValue;
            typedef typename std::remove_pointer<TValue>::type* PointerValue;

            virtual ~Iterator() = default;

            virtual ReferenceValue CurrentValue() const = 0;
            virtual PointerValue   CurrentPointerValue() const = 0;

            virtual System::Reflection::Variant Current() const override
            {
                return CurrentValue();
            }

            virtual bool operator==(Generic::Iterator const& right) const override { return Current() == right.Current() ? true : false; }
            virtual bool operator!=(Generic::Iterator const& right) const override { return Current() != right.Current() ? true : false; }
            virtual bool operator==(Iterator<TValue> const& right) const { return CurrentPointerValue() == right.CurrentPointerValue() ? true : false; }
            virtual bool operator!=(Iterator<TValue> const& right) const { return CurrentPointerValue() != right.CurrentPointerValue() ? true : false; }
            virtual ReferenceValue operator->() const { return CurrentValue(); }
            virtual ReferenceValue operator*() const { return CurrentValue(); }
        };

        template<typename TValue>
        struct IContainer : public Generic::IContainer
        {
            typedef TValue value_type;
            typedef std::shared_ptr<Iterator<value_type>> iterator;

            virtual ~IContainer() = default;

            virtual Meta(Disable) iterator GetEnumerator() = 0;
            virtual Meta(Disable) iterator mfind(TValue const&) = 0;
            virtual TValue& operator[](int) const = 0;
            virtual size_t IndexOf(const TValue&) const = 0;

            size_t IndexOf(const System::Reflection::Variant& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                return IndexOf(value.GetValue<TValue>());
            }

            System::Reflection::Variant ElementAt(int index) const override
            {
                return (*this)[index];
            }

            virtual void UpdateAt(int index, const System::Reflection::Variant& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                (*this)[index] = value.GetValue<TValue>();
            }

            virtual void Insert(int index, System::Reflection::Variant const& value) override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                Insert(index, value.GetValue<TValue>());
            }

            virtual void Add(TValue const&) = 0;
            virtual void Insert(int index, TValue const& value) = 0;

            void Add(System::Reflection::Variant const& key, System::Reflection::Variant const& value) override final
            {
                throw System::InvalidOperationException();
            }
            virtual void Remove(TValue const&) = 0;

            void Add(System::Reflection::Variant const& value) override
            {
                if (typeid(TValue) == typeid(System::Reflection::Variant))
                {
                    Add(*(TValue*)(void*)&value);
                }
                else
                {
                    if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                    {
                        if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                            throw System::ArgumentException("invalid arg");
                    }

                    Add(value.GetValue<TValue>());
                }
            }

            void Remove(System::Reflection::Variant const& value) override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                Remove(value.GetValue<TValue>());
            }

            virtual Generic::IContainer::iiterator GetGenericEnumerator() override
            {
                return GetEnumerator()->shared_from_base_static<Generic::Iterator>();
            }

            virtual const System::Reflection::Type* GetValueType() const override final
            {
                static const System::Reflection::Type * ValueType = &typeof(TValue);
                return ValueType;
            }
        };

        template<class TContainer>
        struct STLIterator : Iterator<typename TContainer::value_type>
        {
            using base = Iterator<typename TContainer::value_type>;
            typedef typename TContainer::iterator iterator;

            iterator __itr;
            iterator __first_itr;
            iterator __end_itr;

            STLIterator(iterator itr, iterator end_itr)
                : __itr(itr)
                , __first_itr(itr)
                , __end_itr(end_itr)
            {
            }

            void operator++() override final
            {
                StepForward();
            }

            void operator--() override final
            {
                StepBackward();
            }

            typename base::ReferenceValue CurrentValue() const override final { return *__itr; }
            typename base::PointerValue   CurrentPointerValue() const override final
            {
                if (__itr == __end_itr)
                    return nullptr;

                return &*__itr;
            }

            inline void StepForward() override final
            {
                ++__itr;
            }

            inline void StepBackward() override final
            {
                --__itr;
            }

            inline bool MoveNext() override final
            { 
                if (IsFinished())
                    return false;
                ++__itr;
                return IsFinished();
            }

            void Reset() override final
            {
                __itr = __first_itr;
            }

            bool IsFinished() override final
            {
                return __itr == __end_itr ? true : false;
            }
        };

        // Todo: implement IEnumerable
        template<typename TValue>
        struct Vector : IContainer<TValue>
        {
            typedef std::vector<TValue> _inner_container;
            typedef STLIterator<_inner_container> iterator;

            Meta(Disable) mutable _inner_container _container;

            Vector()
            { }

            Vector(Vector const& from)
                : _container(from._container)
            { }

            Vector(Vector const&& from)
                : _container(std::move(from._container))
            { }

            Meta(Disable) _inner_container& GetContainer() { return _container; }
            Meta(Disable) _inner_container const& GetContainer() const { return _container; }

            Meta(Disable) typename _inner_container::iterator begin() { return _container.begin(); }
            Meta(Disable) typename _inner_container::iterator end() { return _container.end(); }

            Meta(Disable) typename IContainer<TValue>::iterator mfind(TValue const& value) override final
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Iterator<typename IContainer<TValue>::value_type>>();
            }

            Meta(Disable) typename IContainer<TValue>::iterator GetEnumerator() override final { return std::make_shared<iterator>(_container.begin(), _container.end())->shared_from_base_static<Iterator<typename IContainer<TValue>::value_type>>(); }

            size_t size() const override { return _container.size(); }
            bool empty() const override { return _container.empty(); }

            void Clear() override
            {
                NotifyClear();
                _container.clear();
            }

            Meta(Disable) Generic::IContainer::iiterator ifind(System::Reflection::Variant const& value) override final
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value.GetValue<TValue>());
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Generic::Iterator>();
            }

            bool Contains(System::Reflection::Variant const& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                return std::find(_container.begin(), _container.end(), value.GetValue<TValue>()) != _container.end();
            }

            void Add(TValue const& value) override
            {
                _container.push_back(value);
                if (CollectionChanged)
                    NotifyAdd(_container.size() - 1, _container.back());
            }

            void Insert(int index, TValue const& value) override
            {
                auto itr = _container.begin() + index;
                _container.insert(itr, value);
                if (CollectionChanged)
                    NotifyAdd(index, *(_container.begin() + index));
            }

            void Remove(TValue const& value) override
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyRemove(std::distance(_container.begin(), itr));
                    _container.erase(itr);
                    if (CollectionChanged)
                        NotifyAfterRemove(0);
                }
            }

            Meta(Disable) _inner_container& GetNativeEnumerator()
            {
                return _container;
            }

            Meta(Disable) _inner_container const& GetNativeEnumerator() const
            {
                return _container;
            }

            void QuickSort(std::function<bool(System::Reflection::Variant const&, System::Reflection::Variant const&)> const& func)
            {
                std::sort(_container.begin(), _container.end(), [func](TValue const& lhs, TValue const& rhs)
                {
                    return func(lhs, rhs);
                });
            }

            virtual size_t IndexOf(const TValue& value) const override
            {
                auto itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    size_t index = itr - _container.begin();
                    return index;
                }

                return size_t(-1);
            }

            void Replace(int oldindex, int index) const override
            {
                auto olditr = _container.begin() + oldindex;
                auto itr = _container.begin() + index;
                if (itr != _container.end() && olditr != _container.end())
                {
                    std::swap(*olditr, *itr);
                    if (CollectionChanged)
                        NotifyReplace(oldindex, index);
                }
            }

            virtual void RemoveAt(int index) const override
            {
                auto itr = _container.begin() + index;
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyRemove(std::distance(_container.begin(), itr));
                    _container.erase(itr);
                    if (CollectionChanged)
                        NotifyAfterRemove(0);
                }
            }

            Meta(Disable) void Erase(typename _inner_container::iterator itr)
            {
                if (CollectionChanged)
                    NotifyRemove(std::distance(_container.begin(), itr));
                _container.erase(itr);
                if (CollectionChanged)
                    NotifyAfterRemove(0);
            }

            virtual TValue& operator[](int index) const override
            {
                if (index < 0 || index >= _container.size())
                    throw std::runtime_error("invalid index");

                return const_cast<TValue&>(_container[index]);
            }

            Meta(Disable) bool operator!=(Vector<TValue> const& rhs)
            {
                if (this == &rhs)
                    return true;

                if (rhs.size() != size())
                    return false;

                //for (auto const& v : rhs.GetNativeEnumerator())
                //    Add(v);

                return false;
            }

            Meta(Disable) Vector<TValue>& operator=(Vector<TValue> const& rhs)
            {
                for (auto const& v : rhs.GetNativeEnumerator())
                    Add(v);
                return *this;
            }
        };

        // Todo: implement IEnumerable
        template<typename TValue>
        struct List : IContainer<TValue>
        {
            typedef std::list<TValue> _inner_container;
            typedef STLIterator<_inner_container> iterator;

            Meta(Disable) mutable _inner_container _container;

            List()
            { }

            List(List const& from)
                : _container(from._container)
            { }

            List(List const&& from)
                : _container(std::move(from._container))
            { }

            Meta(Disable) _inner_container& GetContainer() { return _container; }
            Meta(Disable) _inner_container const& GetContainer() const { return _container; }

            Meta(Disable) typename _inner_container::iterator begin() { return _container.begin(); }
            Meta(Disable) typename _inner_container::iterator end() { return _container.end(); }

            Meta(Disable) typename IContainer<TValue>::iterator mfind(TValue const& value) override final
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Iterator<typename IContainer<TValue>::value_type>>();
            }

            Meta(Disable) typename IContainer<TValue>::iterator GetEnumerator() override final { return std::make_shared<iterator>(_container.begin(), _container.end())->shared_from_base_static<Iterator<typename IContainer<TValue>::value_type>>(); }

            size_t size() const override { return _container.size(); }
            bool empty() const override { return _container.empty(); }

            void Clear() override
            {
                NotifyClear();
                _container.clear();
            }

            Meta(Disable) Generic::IContainer::iiterator ifind(System::Reflection::Variant const& value) override final
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value.GetValue<TValue>());
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Generic::Iterator>();
            }

            void Add(TValue const& value) override
            {
                _container.push_back(value);
                if (CollectionChanged)
                    NotifyAdd(_container.size() - 1, _container.back());
            }

            void Insert(int index, TValue const& value) override
            {
                auto itr = std::next(_container.begin(), index);
                _container.insert(itr, value);
                if (CollectionChanged)
                    NotifyAdd(index, *std::next(_container.begin(), index));
            }

            void Remove(TValue const& value) override
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyRemove(std::distance(_container.begin(), itr));
                    _container.erase(itr);
                    if (CollectionChanged)
                        NotifyAfterRemove(0);
                }
            }

            Meta(Disable) void Erase(typename _inner_container::iterator itr)
            {
                if (CollectionChanged)
                    NotifyRemove(std::distance(_container.begin(), itr));
                _container.erase(itr);
                if (CollectionChanged)
                    NotifyAfterRemove(0);
            }

            bool Contains(System::Reflection::Variant const& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                return std::find(_container.begin(), _container.end(), value.GetValue<TValue>()) != _container.end();
            }

            Meta(Disable) _inner_container& GetNativeEnumerator()
            {
                return _container;
            }

            Meta(Disable) _inner_container const& GetNativeEnumerator() const
            {
                return _container;
            }

            void QuickSort(std::function<bool(System::Reflection::Variant const&, System::Reflection::Variant const&)> const& func)
            {
                _container.sort([func](TValue const& lhs, TValue const& rhs)
                {
                    return func(lhs, rhs);
                });
            }

            virtual size_t IndexOf(const TValue& value) const override
            {
                auto itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    size_t index = std::distance(_container.begin(), itr);
                    return index;
                }

                return size_t(-1);
            }

            void Replace(int oldindex, int index) const override
            {
                auto olditr = std::next(_container.begin(), oldindex);
                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end() && olditr != _container.end())
                {
                    std::swap(*olditr, *itr);
                    if (CollectionChanged)
                        NotifyReplace(oldindex, index);
                }
            }

            virtual void RemoveAt(int index) const override
            {
                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyRemove(std::distance(_container.begin(), itr));
                    _container.erase(itr);
                    if (CollectionChanged)
                        NotifyAfterRemove(0);
                }
            }

            virtual TValue& operator[](int index) const override
            {
                if (index < 0 || index >= _container.size())
                    throw std::runtime_error("invalid index");

                typename _inner_container::iterator it = std::next(_container.begin(), index);
                return const_cast<TValue&>(*it);
            }

            Meta(Disable) List<TValue>& operator=(List<TValue> const& rhs)
            {
                for (auto const& v : rhs.GetNativeEnumerator())
                    Add(v);
                return *this;
            }
        };

        struct DynamicList : public Generic::IContainer
        {
            typedef std::list<System::Reflection::Variant> _inner_container;
            typedef STLIterator<_inner_container> iterator;

            Meta(Disable) mutable _inner_container _container;

            DynamicList()
            { }

            DynamicList(DynamicList const& from)
                : _container(from._container)
            { }

            DynamicList(DynamicList const&& from)
                : _container(std::move(from._container))
            { }

            Meta(Disable) _inner_container& GetContainer() { return _container; }
            Meta(Disable) _inner_container const& GetContainer() const { return _container; }

            Meta(Disable) typename _inner_container::iterator begin() { return _container.begin(); }
            Meta(Disable) typename _inner_container::iterator end() { return _container.end(); }

            Meta(Disable) typename std::shared_ptr<iterator> GetEnumerator() { return std::make_shared<iterator>(_container.begin(), _container.end()); }

            size_t size() const override { return _container.size(); }
            bool empty() const override { return _container.empty(); }

            void Clear() override
            {
                NotifyClear();
                _container.clear();
            }

            Meta(Disable) Generic::IContainer::iiterator ifind(System::Reflection::Variant const& value) override final
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Generic::Iterator>();
            }

            void Add(System::Reflection::Variant const& value) override
            {
                _container.push_back(value);
                NotifyAdd(_container.size() - 1, _container.back());
            }

            void Insert(int index, System::Reflection::Variant const& value) override
            {
                auto itr = std::next(_container.begin(), index);
                _container.insert(itr, value);
                NotifyAdd(index, *std::next(_container.begin(), index));
            }

            void Remove(System::Reflection::Variant const& value) override
            {
                typename _inner_container::iterator itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyRemove(std::distance(_container.begin(), itr));
                    _container.erase(itr);
                    if (CollectionChanged)
                        NotifyAfterRemove(0);
                }
            }

            bool Contains(System::Reflection::Variant const& value) const override
            {
                return std::find(_container.begin(), _container.end(), value) != _container.end();
            }

            Meta(Disable) _inner_container& GetNativeEnumerator()
            {
                return _container;
            }

            Meta(Disable) _inner_container const& GetNativeEnumerator() const
            {
                return _container;
            }

            virtual size_t IndexOf(const System::Reflection::Variant& value) const override
            {
                auto itr = std::find(_container.begin(), _container.end(), value);
                if (itr != _container.end())
                {
                    size_t index = std::distance(_container.begin(), itr);
                    return index;
                }

                return size_t(-1);
            }

            void Replace(int oldindex, int index) const override
            {
                auto olditr = std::next(_container.begin(), oldindex);
                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end() && olditr != _container.end())
                {
                    std::swap(*olditr, *itr);
                    if (CollectionChanged)
                        NotifyReplace(oldindex, index);
                }
            }

            virtual void RemoveAt(int index) const override
            {
                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end())
                    _container.erase(itr);
            }

            virtual System::Reflection::Variant& operator[](int index) const
            {
                if (index < 0 || index >= _container.size())
                    throw std::runtime_error("invalid index");

                typename _inner_container::iterator it = std::next(_container.begin(), index);
                return *it;
            }

            Meta(Disable) DynamicList& operator=(DynamicList const& rhs)
            {
                for (auto const& v : rhs.GetNativeEnumerator())
                    Add(v);
                return *this;
            }

            System::Reflection::Variant ElementAt(int index) const override
            {
                return (*this)[index];
            }

            virtual void UpdateAt(int index, const System::Reflection::Variant& value) const override
            {
                (*this)[index] = value;
            }

            void Add(System::Reflection::Variant const& key, System::Reflection::Variant const& value) override final
            {
                throw System::InvalidOperationException();
            }

            virtual Generic::IContainer::iiterator GetGenericEnumerator() override
            {
                return GetEnumerator()->shared_from_base_static<Generic::Iterator>();
            }

            virtual const System::Reflection::Type* GetValueType() const override final
            {
                static const System::Reflection::Type * ValueType = &typeof(System::Reflection::Variant);
                return ValueType;
            }
        };

        template<typename TKey, typename TValue>
        struct IMapContainer : public Generic::IMapContainer
        {
            typedef std::pair<TKey, TValue> value_type;
            typedef std::shared_ptr<Iterator<value_type>> iterator;

            virtual ~IMapContainer() = default;

            virtual Meta(Disable) iterator GetEnumerator() = 0;
            virtual Meta(Disable) iterator mfind(TKey const&) = 0;

            virtual void Add(TKey const&, TValue const&) = 0;
            virtual void Remove(TKey const&) = 0;

            virtual size_t IndexOf(const TKey& value) const = 0;
            virtual TValue& operator[](int index) const = 0;

            size_t IndexOf(const System::Reflection::Variant& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }
                
                return IndexOf(value.GetValue<TKey>());
            }

            System::Reflection::Variant ElementAt(int index) const override
            {
                return ElementAt(index);
            }

            void Add(System::Reflection::Variant const& value) override
            {
                if (value.GetNativeTypeHash() != typeid(value_type).hash_code())
                    throw System::ArgumentException("invalid arg");

                value_type& kvp = value.GetValue<value_type>();
                Add(kvp.first, kvp.second);
            }

            void Add(System::Reflection::Variant const& key, System::Reflection::Variant const& value) override
            {
                if (key.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                if (typeid(TValue) == typeid(System::Reflection::Variant))
                {
                    Add(key.GetValue<TKey>(), *(TValue*)(void*)&value);
                }
                else
                {
                    if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                    {
                        if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                            throw System::ArgumentException("invalid arg");
                    }

                    Add(key.GetValue<TKey>(), value.GetValue<TValue>());
                }
            }

            void Remove(System::Reflection::Variant const& value) override
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                Remove(value.GetValue<TKey>());
            }

            virtual Generic::IContainer::iiterator GetGenericEnumerator() override
            {
                return GetEnumerator()->shared_from_base_static<Generic::Iterator>();
            }

            virtual const System::Reflection::Type* GetKeyType() const override final
            {
                static const System::Reflection::Type * KeyType = &typeof(TKey);
                return KeyType;
            }

            virtual const System::Reflection::Type* GetValueType() const override final
            {
                static const System::Reflection::Type * ValueType = &typeof(TValue);
                return ValueType;
            }
        };

        // Todo: implement IEnumerable
        template<typename TKey, typename TValue>
        struct Map : IMapContainer<TKey, TValue>
        {
            typedef std::map<TKey, TValue> _inner_container;
            typedef STLIterator<_inner_container> iterator;

            Meta(Disable) mutable _inner_container _container;

            Map()
            { }

            Map(Map const& from)
                : _container(from._container)
            { }

            Map(Map const&& from)
                : _container(std::move(from._container))
            { }

            Meta(Disable) _inner_container& GetContainer() { return _container; }
            Meta(Disable) _inner_container const& GetContainer() const { return _container; }

            Meta(Disable) typename _inner_container::iterator begin() { return _container.begin(); }
            Meta(Disable) typename _inner_container::iterator end() { return _container.end(); }

            Meta(Disable) typename IMapContainer<TKey, TValue>::iterator mfind(TKey const& key) override final
            {
                typename _inner_container::iterator itr = _container.find(key);
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Iterator<typename IMapContainer<TKey, TValue>::value_type>>();
            }

            Meta(Disable) typename IMapContainer<TKey, TValue>::iterator GetEnumerator() override final { return std::make_shared<iterator>(_container.begin(), _container.end())->shared_from_base_static<Iterator<typename IMapContainer<TKey, TValue>::value_type>>(); }

            size_t size() const override { return _container.size(); }
            bool empty() const override { return _container.empty(); }

            void Clear() override
            {
                NotifyClear();
                _container.clear();
            }

            virtual size_t IndexOf(const TKey& value) const override
            {
                typename _inner_container::iterator itr = _container.find(value);
                if (itr != _container.end())
                {
                    size_t index = std::distance(_container.begin(), itr);
                    return index;
                }

                return size_t(-1);
            }

            virtual void UpdateAt(int index, const System::Reflection::Variant& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                auto itr = std::next(_container.begin(), index);
				if (itr != _container.end())
				{
					itr->second = value.GetValue<TValue>();
					if (CollectionChanged)
						NotifyValueAdd(itr->first);
				}
            }

            virtual void RemoveAt(int index) const override
            {
                auto itr = std::next(_container.begin(), index);
				if (itr != _container.end())
				{
					if (CollectionChanged)
						NotifyValueRemove(itr->first);
					_container.erase(itr);
				}
            }

            virtual TValue& operator[](int index) const override
            {
                if (index < 0 || index >= _container.size())
                    throw std::runtime_error("invalid index");

                typename _inner_container::iterator it = std::next(_container.begin(), index);
                return const_cast<TValue&>(it->second);
            }

            Meta(Disable) Generic::IContainer::iiterator ifind(System::Reflection::Variant const& value) override final
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                typename _inner_container::iterator itr = _container.find(value.GetValue<TKey>());
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Generic::Iterator>();
            }

            bool Contains(System::Reflection::Variant const& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                return  _container.find(value.GetValue<TKey>()) != _container.end();
            }

            void Add(TKey const& key, TValue const& value) override
            {
                _container[key] = value;
				if (CollectionChanged)
					NotifyValueAdd(key);
            }

            void Remove(TKey const& value) override
            {
                typename _inner_container::iterator itr = _container.find(value);
                if (itr != _container.end())
				{
					if (CollectionChanged)
						NotifyValueRemove(itr->first);
					_container.erase(itr);
				}
            }

            Meta(Disable) _inner_container& GetNativeEnumerator()
            {
                return _container;
            }

            Meta(Disable) _inner_container const& GetNativeEnumerator() const
            {
                return _container;
            }

            Meta(Disable) Map<TKey, TValue>& operator=(Map<TKey, TValue> const& rhs)
            {
                for (auto const& v : rhs.GetNativeEnumerator())
                    Add(v.first, v.second);
                return *this;
            }
        };

        // Todo: implement IEnumerable
        template<typename TKey, typename TValue>
        struct HashMap : IMapContainer<TKey, TValue>
        {
            typedef std::unordered_map<TKey, TValue> _inner_container;
            typedef STLIterator<_inner_container> iterator;

            Meta(Disable) mutable _inner_container _container;

            HashMap()
            { }

            HashMap(HashMap const& from)
                : _container(from._container)
            { }

            HashMap(HashMap const&& from)
                : _container(std::move(from._container))
            { }

            Meta(Disable) _inner_container& GetContainer() { return _container; }
            Meta(Disable) _inner_container const& GetContainer() const { return _container; }

            Meta(Disable) typename _inner_container::iterator begin() { return _container.begin(); }
            Meta(Disable) typename _inner_container::iterator end() { return _container.end(); }

            Meta(Disable) typename IMapContainer<TKey, TValue>::iterator mfind(TKey const& key) override final
            {
                typename _inner_container::iterator itr = _container.find(key);
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Iterator<typename IMapContainer<TKey, TValue>::value_type>>();
            }

            Meta(Disable) typename IMapContainer<TKey, TValue>::iterator GetEnumerator() override final { return std::make_shared<iterator>(_container.begin(), _container.end())->shared_from_base_static<Iterator<typename IMapContainer<TKey, TValue>::value_type>>(); }

            size_t size() const override { return _container.size(); }
            bool empty() const override { return _container.empty(); }

            void Clear() override
            {
                NotifyClear();
                _container.clear();
            }

            virtual size_t IndexOf(const TKey& value) const override
            {
                typename _inner_container::iterator itr = _container.find(value);
                if (itr != _container.end())
                {
                    size_t index = std::distance(_container.begin(), itr);
                    return index;
                }

                return size_t(-1);
            }

            virtual void UpdateAt(int index, const System::Reflection::Variant& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TValue).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetValueType()) && !GetValueType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end())
                {
                    itr->second = value.GetValue<TValue>();
                    if (CollectionChanged)
                        NotifyValueAdd(itr->first);
                }
            }

            virtual void RemoveAt(int index) const override
            {
                auto itr = std::next(_container.begin(), index);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyValueRemove(itr->first);
                    _container.erase(itr);
                }
            }

            virtual TValue& operator[](int index) const override
            {
                if (index < 0 || index >= _container.size())
                    throw std::runtime_error("invalid index");

                typename _inner_container::iterator it = std::next(_container.begin(), index);
                return const_cast<TValue&>(it->second);
            }

            Meta(Disable) Generic::IContainer::iiterator ifind(System::Reflection::Variant const& value) override final
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                typename _inner_container::iterator itr = _container.find(value.GetValue<TKey>());
                return std::make_shared<iterator>(itr, _container.end())->shared_from_base_static<Generic::Iterator>();
            }

            bool Contains(System::Reflection::Variant const& value) const override
            {
                if (value.GetNativeTypeHash() != typeid(TKey).hash_code())
                {
                    if (!value.GetType()->IsAssignableFrom(*GetKeyType()) && !GetKeyType()->IsAssignableFrom(*value.GetType()))
                        throw System::ArgumentException("invalid arg");
                }

                return  _container.find(value.GetValue<TKey>()) != _container.end();
            }

            void Add(TKey const& key, TValue const& value) override
            {
                _container[key] = value;
                if (CollectionChanged)
                    NotifyValueAdd(key);
            }

            void Remove(TKey const& value) override
            {
                typename _inner_container::iterator itr = _container.find(value);
                if (itr != _container.end())
                {
                    if (CollectionChanged)
                        NotifyValueRemove(itr->first);
                    _container.erase(itr);
                }
            }

            Meta(Disable) _inner_container& GetNativeEnumerator()
            {
                return _container;
            }

            Meta(Disable) _inner_container const& GetNativeEnumerator() const
            {
                return _container;
            }

            Meta(Disable) HashMap<TKey, TValue>& operator=(HashMap<TKey, TValue> const& rhs)
            {
                for (auto const& v : rhs.GetNativeEnumerator())
                    Add(v.first, v.second);
                return *this;
            }
        };
        template<typename T, std::size_t Size>
        inline std::size_t GetArrLength(T(&)[Size]) { return Size; }

        template<typename T>
        inline bool array_size_check(unsigned int id, T& array)
        {
            unsigned int size = GetArrLength(array);
            return id < size;
        }

        inline int enum_count() { return 0; }
        template<typename ENUM, typename... Args>
        constexpr int enum_count(ENUM a, Args... args) { return 1 + enum_member_count(args...); }


        ///
        /// Use only std::map unorderedmap std::set unorderedset (can't use multi version)
        ///

        template<class TMAP>
        inline bool GetContains(TMAP& mp, typename TMAP::key_type ky, typename TMAP::iterator& result)
        {
            result = mp.find(ky);
            return result != mp.end();
        }

        template<class TMAP>
        constexpr inline bool GetContains(TMAP& mp, typename TMAP::key_type ky)
        {
            return mp.find(ky) != mp.end();
        }

        ///
        /// Use only std::map unorderedmap std::set unorderedset (can't use multi version)
        ///

        template<class TMAP, typename VALUE>
        inline bool GetValue(TMAP& mp, typename TMAP::key_type ky, VALUE& result)
        {
            typename TMAP::iterator itr = mp.find(ky);
            if (itr != mp.end())
            {
                result = itr->second;
                return true;
            }
            return false;
        }

        template <typename K, typename T>
        class TypeContainer
        {
        public:
            typedef std::map<K, T*> StorageType;

            TypeContainer() : ReadLock(nullptr), WriteLock(nullptr), UnLock(nullptr) {}
            ~TypeContainer() { Clear(); }

            void SetLockMethods(void(*_ReadLock)(), void(*_WriteLock)(), void(*_UnLock)())
            {
                ReadLock = _ReadLock;
                WriteLock = _WriteLock;
                UnLock = _UnLock;
            }

            T* Create(K key)
            {
                T* t = new T;
                Add(key, t);
                return t;
            }

            void Add(K key, T* type)
            {
                if (Exist(key))
                    return;

                if (WriteLock)
                    WriteLock();

                m_container[key] = type;

                if (UnLock)
                    UnLock();
            }

            void Remove(K key)
            {
                if (WriteLock)
                    WriteLock();

                StorageType::iterator i;
                if (GetContains(m_container, key, i))
                {
                    delete i->second;
                    m_container.erase(i);
                }

                if (UnLock)
                    UnLock();
            }

            T* Get(K key)
            {
                return (*this)[key];
            }

            T* operator[](K key)
            {
                T* ret = nullptr;
                StorageType::iterator i;

                if (ReadLock)
                    ReadLock();

                if (GetContains(m_container, key, i))
                    ret = i->second;

                if (UnLock)
                    UnLock();
                return nullptr;
            }


            bool Exist(K key)
            {
                StorageType::iterator i;
                if (ReadLock)
                    ReadLock();

                bool ret = (GetContains(m_container, key, i));

                if (UnLock)
                    UnLock();
                return ret;
            }

            void Clear(void)
            {
                if (WriteLock)
                    WriteLock();

                while (!m_container.empty())
                    Remove(m_container.begin()->first);

                if (UnLock)
                    UnLock();
            }

        private:
            TypeContainer(TypeContainer const&);

            StorageType m_container;

            void(*ReadLock)();
            void(*WriteLock)();
            void(*UnLock)();
        };

        template<typename T>
        class StringIndexedMap
        {
        public:
            typedef std::pair<std::string, T>   NamePair;
            typedef std::vector<NamePair>       NameList;

            NameList m_map;

            void clear()
            {
                m_map.clear();
            }

            void remove(unsigned int pos)
            {
                if (pos < m_map.size())
                {
                    std::swap(m_map[pos], m_map.back());
                    m_map.pop_back();
                }
            }

            void remove(typename NameList::iterator __itr)
            {
                if (__itr != m_map.end())
                {
                    std::swap(*__itr, m_map.back());
                    m_map.pop_back();
                }
            }

            void remove(typename NameList::const_iterator __itr)
            {
                if (__itr != m_map.end())
                {
                    std::swap(*__itr, m_map.back());
                    m_map.pop_back();
                }
            }

            void erase(typename NameList::iterator __itr)
            {
                if (__itr != m_map.end())
                {
                    std::swap(*__itr, m_map.back());
                    m_map.pop_back();
                }
            }

            void erase(typename NameList::const_iterator __itr)
            {
                if (__itr != m_map.end())
                {
                    std::swap(*__itr, m_map.back());
                    m_map.pop_back();
                }
            }

            static bool stringCompare(const NamePair &left, const NamePair &right)
            {
                return strcmp(left.first.c_str(), right.first.c_str()) <= 0;
                //for( std::string::const_iterator lit = left.first.begin(), rit = right.first.begin(); lit != left.first.end() && rit != right.first.end(); ++lit, ++rit )
                //{
                //   if ( *lit == *rit )
                //       continue;
                //    return(*lit < *rit ? true : false);
                //}
                //
                //return(left.first.size() < right.first.size() ? true : false);
            }

            void push_back(NamePair value) { m_map.push_back(value); }
            void Sort() { std::sort(m_map.begin(), m_map.end(), &StringIndexedMap::stringCompare); }

            /*
            * searches for a value in sorted array
            *   arr is an array to search in
            *   value is searched value
            *   left is an index of left boundary
            *   right is an index of right boundary
            * returns position of searched value, if it presents in the array
            * or -1, if it is absent
            */
            int binarySearch(const char* value, int left, int right)
            {
                int size = right;
                while (left <= right)
                {
                    int middle = (left + right) / 2;
                    if (size <= middle)
                        break;
                    const char* middleChar = m_map[middle].first.c_str();
                    int diff = strcmp(middleChar, value);
                    if (!diff)
                        return middle;
                    else if (diff > 0)
                        right = middle - 1;
                    else
                        left = middle + 1;
                }
                return -1;
            }

            bool Find(NamePair* result, const char* pattern, unsigned int* ElementPos = NULL)
            {
                if (m_map.empty())
                    return false;

                int cpos = binarySearch(pattern, 0, m_map.size());
                if (cpos != -1)
                {
                    if (ElementPos)
                        *ElementPos = cpos;

                    if (result)
                        *result = m_map[cpos];
                    return true;
                }
                return false;
            }
        };
    }
};

#endif //__Collection_H__