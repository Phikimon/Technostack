#include <iostream>
#include <iomanip>
#include <algorithm>
#include <typeinfo>

//This enum is put off the class because otherwise
//how shall I catch throws?
enum STACKERRORS_T
{
//TODO:
//Sorry, Klim, but I still don't know how to use exceptions
//correctly, so in this prototype I throw enum-s
    ERR_OK,          //< No error occured
    ERR_COMMON,      //< Non-specified error occured
    ERR_UNDERFLOW,   //< Nothing to pop
    ERR_OVERFLOW,    //< Size_t overflow
    ERR_ALLOC,       //< Failed to allocate dynamic memory
    ERR_DATANULLPTR, //< data_ is null-pointer
};

template <typename T>
class Stack;

template <typename T>
void stackTorture(Stack<T>&);

template <typename T>
void stackUnitTest();

template <typename T>
class Stack
{
public:
    const Stack& operator = (const Stack& stk);
    
    Stack(size_t size = init_size);
    Stack(Stack& stk);
   ~Stack();

    void push(T value);
    T    pop ();
    void dump() const;
    
private:
    void DSSrealloc();  //Dump, Stupid, Slow realloc(because of no cstdlib)
    void verify() const;
    void swap(Stack& stk);

    T*     data_;
    
    size_t count_;      //< points at first free element
    size_t currentSize_;

    //I used static, because it is more
    //natural to type Stack::init_size
    //rather than type stkInst.init_size
    static const size_t init_size = 32; 

    friend void stackTorture<T> (Stack<T>&);
    friend void stackUnitTest<T>();
};

template <typename T>
Stack<T>::Stack(size_t size):
    data_        (new T[size]),
    count_       (0),
    currentSize_ (size)
{
    if (!data_) throw ERR_ALLOC;
}

template <typename T>
Stack<T>::Stack(Stack& stk)
{
    count_ = stk.count_;
    currentSize_ = stk.currentSize_;
    
    data_ = new T[currentSize_];
    if (!data_) throw ERR_ALLOC;

    std::copy_n(stk.data_, stk.currentSize_,  data_);
}

template <typename T>
Stack<T>::~Stack()
{
    delete[] data_; data_ = NULL;
}

template <typename T>
void Stack<T>::swap(Stack& stk)
{
    //I swap pointers. Is it safe?
    std::swap(this->data_,        stk.data_);
    std::swap(this->count_,       stk.count_);
    std::swap(this->currentSize_, stk.currentSize_);
}

template <typename T>
const Stack<T>& Stack<T>::operator = (const Stack& stk)
{
    printf("QQQQQQQQ");
    if (this == &stk) return this;

    printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
    //Copy and swap idiom
    Stack<T>& victim = Stack(stk);
    swap(victim);

    return *this;
}

template <typename T>
void Stack<T>::push(T value)
{
    while (count_ + 1 > currentSize_)
        DSSrealloc();
    data_[count_++] = value;
}

template <typename T>
T    Stack<T>::pop()
{
    if (count_ <= 0)
        throw ERR_UNDERFLOW;
    return data_[--count_];
}

template <typename T>
void Stack<T>::DSSrealloc() 
{
    if (currentSize_ * 2 < currentSize_) //< in case of overflow
        throw ERR_OVERFLOW;

    T* newData = new T[currentSize_ * 2];
    std::copy_n(data_, currentSize_,  newData);
    delete[] data_;
    data_ = newData;

    currentSize_ *= 2;
}

template <typename T>
void Stack<T>::dump()   const
{
    verify();
#ifndef NDEBUG
    std::cout << std::endl;
    std::cout << "\tStack<" << typeid(T).name() << ">" << std::endl;
    std::cout << "\t{" << std::endl;
    std::cout << "\t count = " << count_ << std::endl,
    std::cout << "\t size  = " << currentSize_  << std::endl,
    std::cout << "\t data: ["  << data_ << "]" << std::endl;

    for (size_t i = 0; i < count_; i++)
        std::cout << "\t#" << std::setw(4) << i << " " << data_[i] << std::endl;

    std::cout << "\t}" << std::endl;
#endif
}

template <typename T>
void Stack<T>::verify() const
{
    if (!data_)
        throw ERR_DATANULLPTR;
    if (count_ < 0)
        throw ERR_UNDERFLOW;
    if (count_ + 1 > currentSize_)
        throw ERR_OVERFLOW;
}
