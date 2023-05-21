#ifndef SET_EXEPTION_H
#define SET_EXEPTION_H
#include <iostream> // библиотека для использования пространства имен std

class set_exeption: public std::exception // public, как наследуется - protected & public идет для всех
{ // что значит пространство имен std
private:
    char *message;
public:
    // параметртизированный конструктор - тут частично
    explicit set_exeption(const char *msg) : std::exception(){message = (char *)msg;} // explicit - запрещает неявный вызов, используется только на конструкторах
    const char *what() const noexcept{return message;} // noexcept - показывает, что никаких исключений не выдаст
    // два const, первый показывает что вернет константый тип, второый показывает, что внутренний this метода будеи константый
};

#endif // SET_EXEPTION_H
