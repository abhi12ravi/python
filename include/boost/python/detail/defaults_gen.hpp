///////////////////////////////////////////////////////////////////////////////
//
// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DEFAULTS_GEN_JDG20020807_HPP
#define DEFAULTS_GEN_JDG20020807_HPP

#include <boost/python/detail/preprocessor.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/repeat_from_to.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/comma_if.hpp>
#include <boost/config.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/front.hpp>

namespace boost { namespace python {

//  overloads_base is used as a base class for all function
//  stubs. This class holds the doc_string of the stubs.

namespace detail
{
  struct overloads_base
  {
      overloads_base(char const* doc_)
          : doc(doc_) {}

      char const* doc_string() const
      { return doc; }

      char const* doc;
  };
}

//  overloads_proxy is generated by the overloads_common operator[] (see
//  below). This class holds a user defined call policies of the stubs.

template <class CallPoliciesT, class OverloadsT>
struct overloads_proxy
    : public detail::overloads_base
{
    typedef typename OverloadsT::non_void_return_type   non_void_return_type;
    typedef typename OverloadsT::void_return_type       void_return_type;

    overloads_proxy(CallPoliciesT const& policies_, char const* doc)
    : detail::overloads_base(doc), policies(policies_) {}

    CallPoliciesT
    call_policies() const
    { return policies; }

    CallPoliciesT policies;
};

//  overloads_common is our default function stubs base class. This class
//  returns the default_call_policies in its call_policies() member function.
//  It can generate a overloads_proxy however through its operator[]

template <class DerivedT>
struct overloads_common
: public detail::overloads_base {

    overloads_common(char const* doc)
    : detail::overloads_base(doc) {}

    default_call_policies
    call_policies() const
    { return default_call_policies(); }

    template <class CallPoliciesT>
    ::boost::python::overloads_proxy<CallPoliciesT, DerivedT>
    operator[](CallPoliciesT const& policies) const
    {
        return overloads_proxy<CallPoliciesT, DerivedT>
            (policies, doc);
    }
};

}} // namespace boost::python

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_TYPEDEF_GEN(z, index, data)                                \
    typedef typename BOOST_PP_CAT(iter, index)::next                            \
        BOOST_PP_CAT(iter, BOOST_PP_INC(index));                                \
    typedef typename BOOST_PP_CAT(iter, index)::type BOOST_PP_CAT(T, index);    \

#define BOOST_PYTHON_FUNC_WRAPPER_GEN(z, index, data)                           \
    static RT BOOST_PP_CAT(func_,                                               \
        BOOST_PP_SUB_D(1, index, BOOST_PP_TUPLE_ELEM(3, 1, data))) (            \
        BOOST_PYTHON_BINARY_ENUM(                                               \
            index, T, arg))                                                     \
    {                                                                           \
        BOOST_PP_TUPLE_ELEM(3, 2, data)                                         \
        BOOST_PP_TUPLE_ELEM(3, 0, data)(                                        \
            BOOST_PP_ENUM_PARAMS(                                               \
                index,                                                          \
                arg));                                                          \
    }

#define BOOST_PYTHON_GEN_FUNCTION(fname, fstubs_name, n_args, n_dflts, ret)     \
    struct fstubs_name {                                                        \
                                                                                \
        BOOST_STATIC_CONSTANT(int, n_funcs = BOOST_PP_INC(n_dflts));            \
        BOOST_STATIC_CONSTANT(int, max_args = n_funcs);                         \
                                                                                \
        template <typename SigT>                                                \
        struct gen {                                                            \
                                                                                \
            typedef typename ::boost::mpl::begin<SigT>::type rt_iter;           \
            typedef typename rt_iter::type RT;                                  \
            typedef typename rt_iter::next iter0;                               \
                                                                                \
            BOOST_PP_REPEAT_2ND(                                                \
                n_args,                                                         \
                BOOST_PYTHON_TYPEDEF_GEN,                                       \
                0)                                                              \
                                                                                \
            BOOST_PP_REPEAT_FROM_TO_2(                                          \
                BOOST_PP_SUB_D(1, n_args, n_dflts),                             \
                BOOST_PP_INC(n_args),                                           \
                BOOST_PYTHON_FUNC_WRAPPER_GEN,                                  \
                (fname, BOOST_PP_SUB_D(1, n_args, n_dflts), ret))               \
        };                                                                      \
    };                                                                          \

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_MEM_FUNC_WRAPPER_GEN(z, index, data)                       \
    static RT BOOST_PP_CAT(func_,                                               \
        BOOST_PP_SUB_D(1, index, BOOST_PP_TUPLE_ELEM(3, 1, data))) (            \
            ClassT& obj BOOST_PP_COMMA_IF(index)                                \
            BOOST_PYTHON_BINARY_ENUM(index, T, arg)                             \
        )                                                                       \
    {                                                                           \
        BOOST_PP_TUPLE_ELEM(3, 2, data) obj.BOOST_PP_TUPLE_ELEM(3, 0, data)(    \
            BOOST_PP_ENUM_PARAMS(index, arg)                                    \
        );                                                                      \
    }

#define BOOST_PYTHON_GEN_MEM_FUNCTION(fname, fstubs_name, n_args, n_dflts, ret) \
    struct fstubs_name {                                                        \
                                                                                \
        BOOST_STATIC_CONSTANT(int, n_funcs = BOOST_PP_INC(n_dflts));            \
        BOOST_STATIC_CONSTANT(int, max_args = n_funcs + 1);                     \
                                                                                \
        template <typename SigT>                                                \
        struct gen {                                                            \
                                                                                \
            typedef typename ::boost::mpl::begin<SigT>::type rt_iter;           \
            typedef typename rt_iter::type RT;                                  \
                                                                                \
            typedef typename rt_iter::next class_iter;                          \
            typedef typename class_iter::type ClassT;                           \
            typedef typename class_iter::next iter0;                            \
                                                                                \
            BOOST_PP_REPEAT_2ND(                                                \
                n_args,                                                         \
                BOOST_PYTHON_TYPEDEF_GEN,                                       \
                0)                                                              \
                                                                                \
            BOOST_PP_REPEAT_FROM_TO_2(                                          \
                BOOST_PP_SUB_D(1, n_args, n_dflts),                             \
                BOOST_PP_INC(n_args),                                           \
                BOOST_PYTHON_MEM_FUNC_WRAPPER_GEN,                              \
                (fname, BOOST_PP_SUB_D(1, n_args, n_dflts), ret))               \
        };                                                                      \
    };

///////////////////////////////////////////////////////////////////////////////
#if defined(BOOST_NO_VOID_RETURNS)

#define BOOST_PYTHON_GEN_FUNCTION_STUB(fname, fstubs_name, n_args, n_dflts)     \
    BOOST_PYTHON_GEN_FUNCTION                                                   \
        (fname, BOOST_PP_CAT(fstubs_name, _NV), n_args, n_dflts, return)        \
    BOOST_PYTHON_GEN_FUNCTION                                                   \
        (fname, BOOST_PP_CAT(fstubs_name, _V), n_args, n_dflts, ;)              \
    struct fstubs_name                                          \
    : public boost::python::overloads_common<fstubs_name>                       \
    {                                                           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  non_void_return_type;           \
        typedef BOOST_PP_CAT(fstubs_name, _V)   void_return_type;               \
                                                                \
        fstubs_name(char const* doc = 0)                        \
        : boost::python::                                       \
            overloads_common<fstubs_name>(doc) {}                               \
    };                                                                          \

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_GEN_MEM_FUNCTION_STUB(fname, fstubs_name, n_args, n_dflts) \
    BOOST_PYTHON_GEN_MEM_FUNCTION                                               \
        (fname, BOOST_PP_CAT(fstubs_name, _NV), n_args, n_dflts, return)        \
    BOOST_PYTHON_GEN_MEM_FUNCTION                                               \
        (fname, BOOST_PP_CAT(fstubs_name, _V), n_args, n_dflts, ;)              \
    struct fstubs_name                                                          \
    : public boost::python::overloads_common<fstubs_name>                       \
    {                                                                           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  non_void_return_type;           \
        typedef BOOST_PP_CAT(fstubs_name, _V)   void_return_type;               \
                                                                                \
        fstubs_name(char const* doc = 0)                                        \
        : boost::python::                                                       \
            overloads_common<fstubs_name>(doc) {}                               \
    };                                                                          \

#else

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_GEN_FUNCTION_STUB(fname, fstubs_name, n_args, n_dflts)     \
    BOOST_PYTHON_GEN_FUNCTION                                                   \
        (fname, BOOST_PP_CAT(fstubs_name, _NV), n_args, n_dflts, return)        \
    struct fstubs_name                                                          \
    : public boost::python::overloads_common<fstubs_name>                       \
    {                                                                           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  non_void_return_type;           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  void_return_type;               \
                                                                                \
        fstubs_name(char const* doc = 0)                                        \
        : boost::python::                                                       \
            overloads_common<fstubs_name>(doc) {}                               \
    };                                                                          \

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_GEN_MEM_FUNCTION_STUB(fname, fstubs_name, n_args, n_dflts) \
    BOOST_PYTHON_GEN_MEM_FUNCTION                                               \
        (fname, BOOST_PP_CAT(fstubs_name, _NV), n_args, n_dflts, return)        \
    struct fstubs_name                                                          \
    : public boost::python::overloads_common<fstubs_name>                       \
    {                                                                           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  non_void_return_type;           \
        typedef BOOST_PP_CAT(fstubs_name, _NV)  void_return_type;               \
                                                                                \
        fstubs_name(char const* doc = 0)                                        \
        : boost::python::                                                       \
            overloads_common<fstubs_name>(doc) {}                               \
    };                                                                          \

#endif // defined(BOOST_MSVC)

///////////////////////////////////////////////////////////////////////////////
//
//  MAIN MACROS
//
//      Given generator_name, fname, min_args and max_args, These macros
//      generate function stubs that forward to a function or member function
//      named fname. max_args is the arity of the function or member function
//      fname. fname can have default arguments. min_args is the minimum
//      arity that fname can accept.
//
//      There are two versions:
//
//          1. BOOST_PYTHON_FUNCTION_OVERLOADS for free functions
//          2. BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS for member functions.
//
//      For instance, given a function:
//
//      int
//      foo(int a, char b = 1, unsigned c = 2, double d = 3)
//      {
//          return a + b + c + int(d);
//      }
//
//      The macro invocation:
//
//          BOOST_PYTHON_FUNCTION_OVERLOADS(foo_stubs, foo, 1, 4)
//
//      Generates this code:
//
//      struct foo_stubs_NV  {
//
//          static const int n_funcs = 4;
//          static const int max_args = n_funcs;
//
//          template <typename SigT>
//          struct gen {
//
//              typedef typename ::boost::mpl::begin<SigT>::type    rt_iter;
//              typedef typename rt_iter::type                      RT;
//              typedef typename rt_iter::next                      iter0;
//              typedef typename iter0::type                        T0;
//              typedef typename iter0::next                        iter1;
//              typedef typename iter1::type                        T1;
//              typedef typename iter1::next                        iter2;
//              typedef typename iter2::type                        T2;
//              typedef typename iter2::next                        iter3;
//              typedef typename iter3::type                        T3;
//              typedef typename iter3::next                        iter4;
//
//              static RT func_0(T0 arg0)
//              { return foo(arg0); }
//
//              static RT func_1(T0 arg0, T1 arg1)
//              { return foo(arg0, arg1); }
//
//              static RT func_2(T0 arg0, T1 arg1, T2 arg2)
//              { return foo(arg0, arg1, arg2); }
//
//              static RT func_3(T0 arg0, T1 arg1, T2 arg2, T3 arg3)
//              { return foo(arg0, arg1, arg2, arg3); }
//          };
//      };
//
//      struct foo_stubs
//      : public boost::python::overloads_common<foo_stubs>
//
//          typedef foo_stubs_NV    non_void_return_type;
//          typedef foo_stubs_NV    void_return_type;
//
//          fstubs_name(char const* doc = 0)
//          : boost::python::
//              overloads_common<foo_stubs>(doc) {}
//      };
//
//      The typedefs non_void_return_type and void_return_type are
//      used to handle compilers that do not support void returns. The
//      example above typedefs non_void_return_type and
//      void_return_type to foo_stubs_NV. On compilers that do not
//      support void returns, there are two versions: foo_stubs_NV and
//      foo_stubs_V.  The "V" version is almost identical to the "NV"
//      version except for the return type (void) and the lack of the
//      return keyword.
//
//      See the overloads_common above for a description of the foo_stubs'
//      base class.
//
///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_FUNCTION_OVERLOADS(generator_name, fname, min_args, max_args)          \
    BOOST_PYTHON_GEN_FUNCTION_STUB(                                                         \
        fname,                                                                              \
        generator_name,                                                                     \
        max_args,                                                                           \
        BOOST_PP_SUB_D(1, max_args, min_args))

#define BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(generator_name, fname, min_args, max_args)   \
    BOOST_PYTHON_GEN_MEM_FUNCTION_STUB(                                                     \
        fname,                                                                              \
        generator_name,                                                                     \
        max_args,                                                                           \
        BOOST_PP_SUB_D(1, max_args, min_args))

// deprecated macro names (to be removed)
#define BOOST_PYTHON_FUNCTION_GENERATOR BOOST_PYTHON_FUNCTION_OVERLOADS
#define BOOST_PYTHON_MEM_FUN_GENERATOR BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS

///////////////////////////////////////////////////////////////////////////////
#endif // DEFAULTS_GEN_JDG20020807_HPP


