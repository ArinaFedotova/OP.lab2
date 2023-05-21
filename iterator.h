#ifndef ITERATOR_H
#define ITERATOR_H

#include <initializer_list>
#include <iostream>


template <typename Type>
class set;

template <typename Type>
class Iterator
{
private:
    set<Type> itr_set;
    int itr_set_index = -1;
public:
    Iterator(set<Type> container_obj);
    Iterator next();
    Type value();
    bool is_end();
    Iterator &operator ++ ();
    Type &operator * ();
    bool operator == (Iterator &b);
    bool operator != (Iterator &b);
};

template <typename Type>
Iterator<Type>::Iterator(set<Type> container_obj) : itr_set(container_obj), itr_set_index(0)
{
    while (! (*this).is_end())
        (*this).next();
    itr_set_index = 0;
}

template <typename Type>
Iterator<Type> Iterator<Type>::next()
{
    if(!this->is_end())
        itr_set_index += 1;
    return *this;
}

template <typename Type>
Type Iterator<Type>::value()
{
    return itr_set[itr_set_index];
}

template <typename Type>
bool Iterator<Type>::is_end()
{
    return (itr_set_index == itr_set.get_length()-1 || itr_set.get_length() == 0);
}


template <typename Type>
Iterator<Type>& Iterator<Type>::operator++()
{
    Iterator::next();
    return *this;
}

template <typename Type>
Type &Iterator<Type>::operator*()
{
    return itr_set[itr_set_index];
}


template <typename Type>
bool Iterator<Type>::operator == (Iterator &b)
{
    return (itr_set_index == b.itr_set_index) && (itr_set[itr_set_index] == b.itr_set[itr_set_index]);
}


template <typename Type>
bool Iterator<Type>::operator != (Iterator &b)
{
    return (itr_set_index != b.itr_set_index) && (itr_set[itr_set_index] != b.itr_set[itr_set_index]);
}

#endif // ITERATOR_H
