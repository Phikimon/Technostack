#include "Stack.hpp"
#include <stdio.h>
#include <assert.h>
#include <limits>
#include <math.h>

//#define PRINT_CURRENT_FUNCTION
    
#define STACK_IN()                      \
{                                       \
    STACKERRORS_T code = STACKERR_OK;   \
    if ((code = ok()) != STACKERR_OK)   \
        return code;                    \
}

#define STACK_OUT()          \
{                            \
    checkSum_ = checkSum();  \
    return STACKERR_OK;      \
}                            \

Stack::Stack():
    datasArraySize_              (INIT_SIZE),
    canarySize_                  (INIT_SIZE),
    arraysQt_                    (1),
    numberOfAllocatedArrays_     (1),
    currentArraySize_            (INIT_SIZE),
    count_                       (0)
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    assert(INIT_SIZE);

    ALLOC(datasArray_    = (double**)calloc(INIT_SIZE,     sizeof( *datasArray_)));
    ALLOC(datasArray_[0] = (double*) calloc(INIT_SIZE + 2, sizeof(**datasArray_)));
    //                                                  ^ for canaries

    ALLOC(canary_        = (size_t**)calloc(INIT_SIZE,     sizeof(*canary_)      ));
    ALLOC(seed_          = (int*)    calloc(INIT_SIZE,     sizeof(*seed_)        ));

    canary_[0] = (size_t*)datasArray_[0];
    datasArray_[0]++;
    currentArray_ = datasArray_[0];
    canary_[1] = (size_t*)datasArray_[0] + INIT_SIZE;
    /*   
     *    canary[0]   datasArray_[0]                            canary[1]  
     *        ↓           ↓                                         ↓
     *  ┌───────────┬─────────────────┬───────────┬───────────┬───────────┐
     *  │*canary[0] │datasArray_[0][0]│    ...    │  double   │*canary[1] │
     *  └───────────┴─────────────────┴───────────┴───────────┴───────────┘
     */

    //Fill canaries' values
    for (size_t i = 0; i < 2; i++)
    {
         seed_  [i] = genSeed();
        *canary_[i] = myRandom(seed_[i]);
    }
                                                  
    //Can't be placed in the pre-initialization block
    checkSum_ = checkSum();
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
}

Stack::~Stack() 
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    for (size_t i = 0; i < numberOfAllocatedArrays_; i++)
    {
        datasArray_[i]--; //because there are canaries
        assert(datasArray_[i]);
        FREE(datasArray_[i]);
    }
    FREE(datasArray_);
    currentArray_ = NULL;
    for (size_t i = 0; i < numberOfAllocatedArrays_ * 2; i++)
    {
        seed_  [i] = POISON_INT;
        canary_[i] = (size_t*)POISON_SIZE_T;
    }
    FREE(seed_);
    FREE(canary_);
    
    canarySize_ = datasArraySize_ = numberOfAllocatedArrays_ = arraysQt_ = count_ = checkSum_ = POISON_SIZE_T;
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
}

size_t Stack::checkSum() const
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
#define checkSumXorSmth(smth)                                      \
{                                                                  \
    newCheckSum ^= ((size_t)smth);                                 \
    newCheckSum  = (newCheckSum << 1) |                            \
                   (newCheckSum >> (sizeof(newCheckSum) * 8 - 1)); \
}
    size_t newCheckSum = 0;
    for (size_t i = 0; i < sizeof(Stack) - sizeof(checkSum_); i++)
    {
        checkSumXorSmth(*((char*)this + i));
    }
    
    assert(seed_); assert(canary_);
    for (size_t i = 0; i < arraysQt_ * 2; i++)
    {
        checkSumXorSmth(seed_  [i]);
        checkSumXorSmth(canary_[i]);
    }

    assert(datasArray_);
    for (size_t i = 0; i < arraysQt_; i++)
    {
        checkSumXorSmth(datasArray_[i]);
        for (size_t j = 0; j < INIT_SIZE * pow(2, i); j++)
        {
            assert(datasArray_[i]);
            checkSumXorSmth(datasArray_[i][j]);
        }
    }
    
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    static size_t callNum = 0;
    callNum++;
    return newCheckSum;
#undef checkSumXorSmth
}

STACKERRORS_T Stack::realloc()
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    STACK_IN();
    arraysQt_++;
    if (numberOfAllocatedArrays_ < arraysQt_)
    {
        //realloc array of arrays
        while (datasArraySize_ <= arraysQt_)
        {
            ALLOC( datasArray_ = (double**)std::realloc(datasArray_, 
                                                        datasArraySize_ * 2));
            datasArraySize_ *= 2;
        }

        //Allocate memory for the new array in the array of arrays
        currentArraySize_ = INIT_SIZE * pow(2, arraysQt_ - 1);
        assert(currentArraySize_);
        ALLOC( datasArray_[arraysQt_ - 1] = (double*)calloc(
                                        currentArraySize_ + 2, sizeof(**datasArray_)) );
                                                    //      ^ for canaries
        numberOfAllocatedArrays_++;

        assert(canary_);
        assert(seed_  );
        //realloc array of pointers to canaries
        while (canarySize_ <= arraysQt_ * 2)
        {
            ALLOC( canary_ = (size_t**)std::realloc(canary_, canarySize_ * 2));
            ALLOC( seed_   = (int*)    std::realloc(seed_  , canarySize_ * 2));
            canarySize_ *= 2;
        }

        assert(canary_);

        canary_[arraysQt_ * 2 - 2] = (size_t*)datasArray_[arraysQt_ - 1];
        datasArray_[arraysQt_ - 1]++;
        canary_[arraysQt_ * 2 - 1] = (size_t*)datasArray_[arraysQt_ - 1] + currentArraySize_;
    }

    currentArray_ = datasArray_[arraysQt_ - 1];
    count_ = 0;
    
    /*  
     *    canary[.]   datasArray_[.]                             canary[. + 1]  
     *        ↓           ↓                                            ↓
     *  ┌───────────┬─────────────────┬───────────┬───────────┬───────────────┐
     *  │*canary[.] │datasArray_[.][0]│    ...    │  double   │*canary[. + 1] │
     *  └───────────┴─────────────────┴───────────┴───────────┴───────────────┘
     *  SEE THE STACK CONSTRUCTOR
     */

    //Fill canaries' values
    for (size_t i = arraysQt_ * 2 - 2; i < arraysQt_ * 2; i++)
    {
        assert(seed_); assert(seed_ + i);
         seed_  [i] = genSeed();
        assert(canary_); assert(canary_ + i); assert(canary_[i]);

        *canary_[i] = myRandom(seed_[i]);
    }

#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    STACK_OUT();
}

STACKERRORS_T Stack::ok() const
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
#define CHECK_CONDITION(cond, err)            \
{                                             \
    if (!(cond))                              \
    {                                         \
        printf("\n%s error occured\n", #err); \
        return STACKERR_##err;                \
    }                                         \
}

    //STACK_IN(); //<'STACK_IN' calls 'ok'

    CHECK_CONDITION(checkSum_ == checkSum(), CHECKSUM);
    CHECK_CONDITION(datasArray_, NULLDATAPTRPTR);
    CHECK_CONDITION(canary_,     NULLCANPTR);
    CHECK_CONDITION(seed_,       NULLSEEDPTR);
    CHECK_CONDITION(numberOfAllocatedArrays_ >= arraysQt_, ARRSALLOCATED);

    for (size_t i = 0; i < arraysQt_; i++)
        CHECK_CONDITION(datasArray_[i], NULLDATAPTR);

    for (size_t i = 0; i < arraysQt_ * 2; i++)
        CHECK_CONDITION(*canary_[i] == myRandom(seed_[i]), CANARY);

#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    return STACKERR_OK;
#undef CHECK_CONDITION
};

STACKERRORS_T Stack::_dump(const char stackName[MAX_LINE_LEN]) const
{ 
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    STACKERRORS_T okVal = ok();

    printf("\tStack '%s' is %s [%p] \n", stackName, ((okVal == STACKERR_OK) ? "okay" : "NOT OKAY."), this);
    printf("\t{\n");
    if ((okVal != STACKERR_OK)) printf("\t Error code is %i\n", okVal);
    printf("\t count = %lu\n", count_);
    printf("\t currentArray_ = %p\n", currentArray_);
    printf("\t arraysQt_ = %d\n", arraysQt_);
#ifdef NDEBUG
    printf("\t Sorry, I can't show you canaries' values\n");
#else
    printf("\n\t Canaries:\n");
    for (size_t i = 0; i < arraysQt_ * 2; i++)
    {
        printf("\t  seed   [%2d][%p]  = %x\n",  i, seed_ + i,   seed_[i]);
        printf("\t *canary_[%2d][%p]  = %lx\n", i, canary_[i], *canary_[i]);
    }
    printf("\n");
#endif
    printf("\t datasArray_ [%p] has size %lu:\n", datasArray_, datasArraySize_);
    for (size_t i = 0; i < arraysQt_; i++)
        printf("\t datasArray_[%d] = %p\n", i, datasArray_[i]);
    assert(datasArray_);
    printf("\t data: [%p]\n", datasArray_);
    for (size_t i = 0; i < arraysQt_; i++)
    {
        assert(datasArray_[i]);
        size_t newSize = INIT_SIZE * pow(2, i);
        printf("\t NEW ARRAY [%p] OF SIZE %lu\n", datasArray_[i], newSize);
        for (size_t j = 0; j < newSize; j++)
        {
            printf("%- 4d [%p] %+lg\n", j, (void*)((size_t)(datasArray_[i] + j) % 0xFFFF),
                                   datasArray_[i][j]);
        }
        printf("\t END OF ARRAY\n");
    }

    printf("\n\t}\n\n");
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    return okVal;
}


STACKERRORS_T Stack::push(double value)
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    STACK_IN();

    if (count_ == currentArraySize_) 
    {
        realloc();
    }
    assert(currentArray_);
    assert(currentArray_ + count_);
    currentArray_[count_++] = value;

    checkSum_ = checkSum();
    
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    STACK_OUT();
}

double Stack::pop()
{
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    STACK_IN();

    double data = currentArray_[count_];

    if (!count_)
    {
        if (arraysQt_ <= 1)
        {
            prln();
            printf("\nStack error: nothing to pop\n");
            myThrow(STACKERR_NOPOP);
        }
        currentArraySize_ /= 2;
        count_ = currentArraySize_;
        arraysQt_--;
        currentArray_ = datasArray_[arraysQt_ - 1];
    }
    
    count_--;

    checkSum_ = checkSum();

#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
    return data;
}

void stackTorture(bool verbose)
{    
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nI am %s!\n", __func__);
#endif
    try 
    {
        Stack stk;
        putc('\n', stdout);
        for (int i = 0; i < 25555; i++)
        {
            printf("\r%d", i);
            stk.push(i);
        }
        for (int i = 100; i > 0; i--)
        {
            stk.pop();
            stackDump(stk);
        }
    }
    catch (ERRORS_T errCode)
    {
        printf("\n%s\n", ERR_ST(errCode));
        myThrow(ERR_EMPTY);
    }
    catch (STACKERRORS_T errCode)
    {
        printf("\nError STACKERRORS_T #(%i)\n", errCode);
        myThrow(ERR_EMPTY);
    };
#ifdef PRINT_CURRENT_FUNCTION
    printf("\nBye from %s!\n", __func__);
#endif
}

#undef STACK_IN
#undef STACK_OUT

