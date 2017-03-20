#ifndef STACK_HPP_INCLUDED
#define STACK_HPP_INCLUDED

#include <string.h>
#include "Common/Common.hpp"

enum { INIT_SIZE = 32 };
Govnocode // Replace with class' const static field

enum STACKERRORS_T
{
    STACKERR_OK              =  0, //< No error occured
    STACKERR_COMMON          = -1, //< Non-specified error occured
    STACKERR_CANARY          = -2, //< One of the canaries has wrong value
    STACKERR_NOPOP           = -3, //< Nothing to pop
    STACKERR_CHECKSUM        = -4, //< Check sum is incorrect
    STACKERR_NULLDATAPTRPTR  = -5, //< datasArray_    pointer is NULL
    STACKERR_NULLCANPTR      = -6, //< canary_        pointer is NULL
    STACKERR_NULLSEEDPTR     = -7, //< seed_          pointer is NULL
    STACKERR_NULLDATAPTR     = -8, //< datasArray_[i] pointer is NULL
    STACKERR_ARRSALLOCATED   = -9, //< number of allocated arrays < arrays quantity
};

#define stackDump(stk)                    \
{                                         \
    assert(strlen(#stk) <= MAX_LINE_LEN); \
    stk._dump(#stk);                      \
}

#define PROP_SET(type, varName)                         \
    void set_##varName(type val) { varName##_ = val;  };

#define PROP_GET(type, varName)                         \
    type get_##varName() const   { return varName##_; };

#define PROPERTIES

class Stack
{
    public:
        Stack();
        ~Stack();
        STACKERRORS_T push(double value);
        double        pop ();
        double        peek() const;
        //dump is in private section, because there is a macro to call it
        STACKERRORS_T _dump(const char stackName[MAX_LINE_LEN] = "name") const;
    PROPERTIES
        PROP_GET(size_t, count)
    private:

        double** datasArray_;     //< array of pointers to arrays
        size_t** canary_;         //< array of pointers to canaries
        int*     seed_;           //< are used to generate canaries' values
        size_t datasArraySize_;   //< current size of 'datasArray_' array
        size_t canarySize_;       //< current size of 'canary_' array
        size_t arraysQt_;         //< quantity of currently allocated arrays in datasArray_
        size_t currentArraySize_; //< size of currently used array
        double* currentArray_;    //< pointer to the first cell
        size_t numberOfAllocatedArrays_;//< Number of arrays, memory for which is allocated

/*               ┌───────────┬───────────┬───────────┬───────────┐
 * datasArray =  │  double*  │  double*  │    ...    │     ...   │  
 *               └───────────┴───────────┴───────────┴───────────┘
 *                     ↓           ↓                        ↓
 *                ┌────────┐   ┌────────┐⬋canary[2]     ┌────────┐
 *                │ size_t │   │ size_t │               │  ...   │
 *              ⬈ ├────────┤   ├────────┤ arraysQt = 2  ├────────┤
 *    canary[0]   │ double │   │ double │⬅ count = 0    │  ...   │
 *                ├────────┤   ├────────┤               ├────────┤
 *    canary[1]   │  ...   │   │  ...   │ canary[3]     │  ...   │
 *              ⬊ ├────────┤   ├────────┤⬋              ├────────┤
 *                │ size_t │   │ size_t │               │  ...   │
 *                └────────┘   └────────┘               └────────┘
 *            │ INIT_SIZE + 2 │INIT_SIZE * 2 + 2 │
 *            │   elements    │elements          │
 */
        size_t        checkSum() const;
        STACKERRORS_T ok      () const;
        STACKERRORS_T realloc ();
    PROPERTIES
        size_t count_;
    //THIS  FIELD HAS TO BE IN THE END OF THE CLASS!
        size_t   checkSum_;
};
#undef PROPERTIES
#undef PROP_SET
#undef PROP_GET

    /**
     Unit test for stack
     */
    void stackTorture(bool verbose = false);

#endif
