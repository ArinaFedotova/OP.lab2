#ifndef SET_H
#define SET_H

#include <initializer_list>
#include <iostream>
#include <iterator.h>
#include "set_exeption.h"

template <typename Type>
class set
{
private:
    Type *st;
    int len = 0;
public:
    set();                                          //конструктор по умолчанию
    set(const set<Type>&s);                         //конструктор копирования
    set (set<Type> &&s);                            //конструктор перемещения
    explicit set(std::initializer_list<Type> lst);  //конструктор со списком инициализации
    ~set();                                         //деструктор
    set<Type>& operator = (const set<Type>& lst);   //перегрузка оператора присваивания
    int get_length() const;                         //получить текущий размер
    bool contains(const Type& elem);                //проверить наличие в множестве элемента
    void add(const Type& elem);                     //добавить элемент в множество
    void remove(const Type& elem);                  //удалить элемент в множество
    Type* to_array();                               //создать новый массив, в который необходимо записать все элементы множества
    set<Type>&unionn(const set<Type>& s);           //результат – объединение this с s
    set<Type>& intersection(const set<Type>& s);    //результат – пересечение this с s
    set<Type>& subtract(const set<Type>& s);        //результат – разность this и s
    template<typename _T>                           //перегрузка оператора <<
    friend std::ostream& operator <<(std::ostream& os, const set<_T>& lst);
    set<Type>& operator +=(const set<Type>& s);     //перегрузка оператора += результат – объединение множеств this и s
    set<Type>& operator *=(const set<Type>& s);     //перегрузка оператора *=, результат – пересечение множеств this и s
    set<Type>& operator /=(const set<Type>& s);     //перегрузка оператора /=, разность множеств this и s
    template<typename _T>                           //перегрузка оператора + результат – объединение множеств v1 и v2
    friend set<_T> operator +(const set<_T>& s1, const set<_T>& s2);
    template<typename _T>                           //перегрузка оператора * результат – пересечение множеств v1 и v2
    friend set<_T> operator *(const set<_T>& s1, const set<_T>& s2);
    template<typename _T>                           //перегрузка оператора / результат – разность множеств v1 и v2
    friend set<_T> operator /(const set<_T>& s1, const set<_T>& s2);
    Iterator<Type> iterator_begin();                //метод получения итератора на начало множества (первый элемент)
    Iterator<Type> iterator_end();                  //метод получения итератора на конец множества (фиктивный элемент, следующий за последним в множестве)
    void clear();                                   //очистить множество
    void resize(int amount);
    Type &operator[](int index);

};

template<typename Type>
Type &set<Type>::operator[](int index)
{
    if(index < 0 || len <= index)
        throw set_exeption("Wrong index of m_vector");
    return *(st + index);
}

template<typename Type>
void set<Type>::resize(int len){
    st = (Type*)realloc(st, sizeof(Type) * len);
    if (st == nullptr){
        len = 0;
        throw set_exeption("Bad alloc!");
    }
}

template <typename Type>
set<Type>::set()    //конструктор по умолчанию
{
    st = nullptr;
    len = 0;
}

template <typename Type>
set<Type>::set(const set<Type>&s)   //конструктор копирования
{
    try {
        len = s.len;
        st = new Type[len];
        for(int i = 0; i < len; i++)
            st[i] = s.st[i];
    } catch (std::bad_alloc const&){
        throw set_exeption("Bad alloc!");
    }
}

template <typename Type>
set<Type>::set (set<Type> &&s)  //конструктор перемещения
{
    st = s.st;
    len = s.len;
    s.len = 0;
    s.st = nullptr;
}

template <typename Type>
set<Type>::set(std::initializer_list<Type> lst)   //конструктор со списком инициализации
{
    len = lst.size();
    try {
        if(len < 0)
            throw set_exeption("Bad length of set");
        st = new Type[len]{};
        for(Type item : lst)
            (*st).add(item);
    } catch (std::bad_alloc const&) {
        st = nullptr;
        throw set_exeption("Bad alloc");
    }
}

template <typename Type>
set<Type>::~set()   //деструктор
{
    delete[] st;
}

template<typename Type>
set<Type>& set<Type>::operator=(const set<Type>& lst)   //перегрузка оператора присваивания
{
    try {
        st = new Type[lst.len]{};
        len = lst.len;
        for(int i = 0; i < lst.len; i++)
            st[i] = lst.st[i];

        return *this;
    } catch (std::bad_alloc const&){
        throw set_exeption("bad alloc");
    }
}

template<typename Type>
int set<Type>::get_length() const  //получить текущий размер
{
    return len;
}

template<typename Type>
bool set<Type>::contains(const Type& elem)  //проверить наличие в множестве элемента
{
    bool ans = false;
    Iterator<Type> iter(*this);
    while (!iter.is_end()) {
        if (iter.value() == elem){
            ans = true;
            break;
        }
        ++iter;
    }
    return ans;
}

template<typename Type>
void set<Type>::add(const Type& elem)   //добавить элемент в множество
{
    if (!contains(elem))
    {
        len++;
        resize(len);
        st[len-1] = elem;
    }

}

template<typename Type>
void set<Type>::remove(const Type& elem)   //удалить элемент в множествe
{
    len--;
    if(len < 0)
        throw set_exeption("Bad length for set");
    for(int i = 0; i < len; i++){
        if (elem == st[i]){
            st[i] = st[i+1];
            break;
        }
    }
//    if (st[len] == elem)
//        *(st + len) = nullptr;
    resize(len);
}

template<typename Type>
Type* set<Type>::to_array()    //создать новый массив, в который необходимо записать все элементы множества
{
    Type *array = new Type[len];
    Iterator<Type> iter(*this);
    for (int i = 0; i < len; i++) {
        array[i] = iter.value();
        ++iter;
    }
    return array;
}

template<typename Type>
set<Type>& set<Type>::unionn(const set<Type>& s)    //результат – объединение this с s
{
    set<Type> *uni = this;
    Iterator<Type> iter(*uni);
    Iterator<Type> iter_2(s);
    while (!(iter.is_end())){
        (*uni).add(iter.value());
        ++iter;
    }
    return *uni;
}

template<typename Type>
set<Type>& set<Type>::intersection(const set<Type>& s)    //результат – пересечение this с s
{
//    set<Type> iter = new Type[std::min(s.len, get_length())];
//    int length = 0;
//    for (int i = 0; i < s.len(); i++)
//        if (contains(s[i])){
//            iter.add();
//            length++;
//        }
//    iter.resize(length);
//    return iter;

    set<Type> *s_new = this;
    Iterator<Type> iter(*s_new);
    set<Type> s_copy = s;
    while (!iter.is_end()) {
        const int k = iter.value();
        if (!s_copy.contains(k)) {
            remove(iter.value());
        } else {
            ++iter;
        }
    }
    return *s_new;
}

template<typename Type>
set<Type>& set<Type>::subtract(const set<Type>& s)  //результат – разность this и s
{
//    set<Type> subt = new Type[*this->len];
//    subt = unionn(s);
//    int del_num = 0;
//    for (Type item : intersection(s))
//    {
//        subt.remove(item);
//        del_num++;
//    }
//    subt = new Type[subt.len - del_num]{};
//    return subt;

    set<Type> *s_new = this;
    Iterator<Type> iter(*s_new);
    Iterator<Type> iter_2(s);
    while (!iter_2.is_end()) {
        int k = iter_2.value();
        if ((*s_new).contains(k)) {
            (*s_new).remove(k);
        } else {
            (*s_new).add(k);
        }
        ++iter_2;
    }
    return *s_new;
}

template<typename _T>
std::ostream& operator <<(std::ostream& os, const set<_T>& lst)  //перегрузка оператора <<
{
    std::string container = "set {len: " + std::to_string(lst.len) + ", set: [";
    for(int i = 0; i < lst.len; i++)
        container += std::to_string(lst.st[i]) + ", ";
    if(lst.len > 0)
        container = container.substr(0, container.size()-2); // Удалить два лишних ", "
    container += "]}";
    return os << container;
}

template<typename Type>
set<Type>& set<Type>::operator +=(const set<Type>& s)   //перегрузка оператора += результат – объединение множеств this и s
{
    *this = *this + s;
    return *this;
}

template<typename Type>
set<Type>& set<Type>::operator *=(const set<Type>& s)   //перегрузка оператора += результат – пересечение множеств this и s
{
    *this = *this * s;
    return *this;
}

template<typename Type>
set<Type>& set<Type>::operator /=(const set<Type>& s)   //перегрузка оператора += результат – разность множеств this и s
{
    *this = *this / s;
    return *this;
}

template<typename _T>
set<_T> operator +(const set<_T>& s1, const set<_T>& s2)  //перегрузка оператора + результат – объединение множеств s1 и s2
{
    set<_T> s;
    s = set(s1);
    s = s.unionn(s2);
    return s;
}

template<typename _T>
set<_T> operator *(const set<_T>& s1, const set<_T>& s2)    //перегрузка оператора * результат – пересечение множеств s1 и s2
{
    set<_T> s;
    s = set(s1);
    s = s.intersection(s2);
    return s;
}

template<typename _T>
set<_T> operator /(const set<_T>& s1, const set<_T>& s2)    //перегрузка оператора / результат – разность множеств s1 и s2
{
    set<_T> s;
    s = set(s1);
    s = s.subtract(s2);
    return s;
}

template<typename Type>
Iterator<Type> set<Type>::iterator_begin()
{
    return Iterator<Type>(*this);
}

template<typename Type>
Iterator<Type> set<Type>::iterator_end()
{
    Iterator<Type> it = Iterator<Type>(*this);
    while(!it.is_end())
        it.next();
    return it;
}

template<typename Type>
void set<Type>::clear()    //очистить множество
{
    for (int i = 0; i<len; i++)
        st[i] = nullptr;
    len = 0;
    st = nullptr;
}

#endif // SET_H
