#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <stdlib.h>

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif


//What needs refactoring
#define GovnoCode
#define Govnocode

//Debug
#undef NDEBUG

#define _assert(cond) \
{ \
    if (!(cond)) \
    { \
        int* a = NULL; \
        *a = 0; \
    }; \
    assert(cond); \
}

#ifdef __cplusplus 

#ifndef NDEBUG

#define myThrow(a) { prln(); throw(a); }
//#define myThrow { prln(); throw; }

#else //~NDEBUG

#define myThrow(a) { throw(a); }
//#define myThrow { throw; }

#endif //~NDEBUG

#endif //~__cplusplus

#define SUPERCHECKS
#ifdef  SUPERCHECKS

#define prln() printf("\n FILE %s; LINE %i\n", __FILE__, __LINE__)
#define prdel() printf("\n___________________________________________________________________\n")
#define prhello() printf("\n\033[1m\033[31m\033[4m\033[5mI am here! Line №%i\033[0m\n", __LINE__);

 /*
  Assert and print error message
 */
#define assMes(cond, mes)                        \
 if (!(cond))                                    \
 {                                               \
   assert(!mes);                                 \
 }

 /*
  Check for array overrun, or for validity of counter. 'belTo' stands for 'belongs to'
 */
#define belTo(index, left, right, mes)           \
 if ( ( index < left ) || ( index >  right ) )   \
 {                                               \
   assert(!mes);                                 \
 }

#define belToLeft(index, left, mes)              \
 if ( index < left )                             \
 {                                               \
   assert(!mes);                                 \
 }

#define belToRight(index, right, mes)            \
 if ( index > right )                            \
 {                                               \
   assert(!mes);                                 \
 }

#ifdef __cplusplus

#define belToEx(index, left, right, code)        \
 if ( ( index < left ) || ( index >  right ) )   \
 {                                               \
   myThrow(code);                                  \
 }

#define belToLeftEx(index, left, code)           \
 if ( index < left )                             \
 {                                               \
   myThrow(code);                                  \
 }

#define belToRightEx(index, right, code)         \
 if ( index > right )                            \
 {                                               \
   myThrow(code);                                  \
 }

#endif //~__cplusplus

#else //~STUPIDUSER

#define prln()

#define prdel()

#define prhello()

#define assMes(cond, mes)

#define belTo(index, left, right, mes)

#define belToLeft(index, left, mes)

#define belToRight(index, right, mes)

#ifdef __cplusplus

#define belToEx(index, left, right, code)

#define belToLeftEx(index, left, code)

#define belToRightEx(index, right, code)



#endif //~__cplusplus

#endif //~STUPIDUSER

//Some constants

#define MAX_LINE_LEN 256
#define MACH_EPS 1e-10

//Some error messages/status codes

enum ERRORS_T
{
    ERR_EMPTY    =   0, //< Nothing is printed
    ERR_NOERR    =  -1, //< Error: no such error exists :)
    ERR_STD      =  -2, //< Error: user, call the administrator
    ERR_ALLOC    =  -3, //< Error: failed to allocate memory
    ERR_OPFILER  =  -4, //< Error: failed to open file for reading(doesn't exist)
    ERR_OPFILEW  =  -5, //< Error: failed to open file for writing(already busy)
    ERR_LONG     =  -6, //< Error: too long file name
    ERR_MANY     =  -7, //< Error: too many arguments/parameters
    ERR_MISSO    =  -8, //< Error: missing '-o' parameter
    ERR_WRINPAR  =  -9, //< Error: wrong input option
    ERR_EXCHAR   = -10, //< Error: Excess character in option
    ERR_NOUTNAME = -11, //< Error: no output file name found
    ERR_UNKPAR   = -12, //< Error: Unknown option
    ERR_TWICE    = -13, //< Error: twice entered option
    ERR_SCANF    = -14, //< Error: scanf can't read what you asked for
    ERR_FREAD    = -15, //< Error: can't read from file
    ERR_COMMON   = -16, //< Error: not specified error occured
    ERR_OPFILE   = -17, //< Error: failed to open file
    ERR_QT        = -18,
};

typedef enum ERRORS_T ERRORS_T;

extern const char* _ERR_ST[-ERR_QT];

//Returns string that represents error
const char* ERR_ST(ERRORS_T ERR);

//My collection of potions

extern const char POISON_CHAR;
extern const int  POISON_INT;
extern const size_t POISON_SIZE_T;

#ifdef __cplusplus
#define POISON_DOUBLE (std::numeric_limits<double>::quiet_NaN())
#else  //__cplusplus
#define POISON_DOUBLE 0xDEADBEEFDEADBEEF
#endif //__cplusplus

//Useful defines

#define printfTo(__command, ...)       \
{                                      \
   char __temporal[MAX_LINE_LEN] = ""; \
   sprintf(__temporal, __VA_ARGS__);   \
   __command(__temporal);              \
}                                      

#define DO_OR_DIE(function) \
{                           \
    if (function)           \
        return -1;          \
}

#define FREE(a)   { free(a);   (a) = NULL; }

#define FCLOSE(a) { fclose(a); (a) = NULL; }

#ifdef __cplusplus 

#define FOPEN(a)            \
{                           \
    if (!(a))               \
    {                       \
        prln();             \
        myThrow(ERR_OPFILE); \
    }                       \
}

#define ALLOC(a)      \
if (!(a))             \
{                     \
    prln();           \
    myThrow(ERR_ALLOC); \
}

#else //__cplusplus

#define FOPEN(a)            \
{                           \
    if (!(a))               \
    {                       \
        prln();             \
        myThrow(ERR_OPFILE); \
    }                       \
}

#define ALLOC(a)      \
if (!(a))             \
{                     \
    prln();           \
    return ERR_ALLOC; \
}

#endif

//#define checkSum(smth, verb) { assert(strlen(#smth) <= MAX_LINE_LEN); _checkSum(smth, verb, #smth); }

//Common functions

size_t myRandom(int seed);

int genSeed();

bool D_EQ(double a, double y);

#endif
