#include "Stack.hpp"
#include <typeinfo>
//#include <iostream>

const size_t MAX_SIZE = (1 << 5);

template <typename T>
void stackTorture(Stack<T>& stk)
{
    //----------------------------------
    for (size_t i = 0; i <= MAX_SIZE; i++)
    {
        stk.push(i);
    }
    stk.dump();
    //----------------------------------
    for (size_t i = 0; i <= MAX_SIZE; i++)
    {
        stk.push(i/2);
        if ((size_t)stk.pop() != i/2)
            throw -1;
    }
    stk.dump();
    //----------------------------------
    for (size_t i = 0; i <= MAX_SIZE; i++)
    { 
        if ((size_t)stk.pop() != MAX_SIZE - i)
            throw -1;
    }
    stk.dump();
    //----------------------------------
}

template <typename T>
void stackUnitTest()
{
    try 
    {
        //----------------------------------
        Stack<T> stk(255); 
        stackTorture<T>(stk);
        //----------------------------------
        Stack<T> stk1; 
        stackTorture<T>(stk1);
        //----------------------------------
        Stack<T> stk2 = stk;
        stk2.dump();
        stk2.push(255);
        stk2.dump();
        //----------------------------------
        Stack<T> stk3(stk2);
        stk.push(511);
        stk3.dump();
        //----------------------------------
    } 
    catch (int err)
    {
        if (err == -1)
        {
            std::cout << "Stack<" << typeid(T).name() << ">::pop failed";
            std::cout << std::endl;
        }

    }
    catch (STACKERRORS_T err)
    {
        switch (err)
        {
#define PRINTERR(errname) \
        case errname:   \
            std::cout << std::endl << #errname << " OCCURED" << std::endl; \
            break;

        PRINTERR(ERR_UNDERFLOW);
        PRINTERR(ERR_OVERFLOW);
        PRINTERR(ERR_ALLOC);
        PRINTERR(ERR_DATANULLPTR);

#undef PRINTERR
        }
    }
}

int main()
{
    stackUnitTest<double>();    
    return 0;
}
