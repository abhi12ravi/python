///////////////////////////////////////////////////////////////////////////////
//
// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
///////////////////////////////////////////////////////////////////////////////
#if !defined(BOOST_PP_IS_ITERATING)

#ifndef SIGNATURE_JDG20020813_HPP
#define SIGNATURE_JDG20020813_HPP

#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/iterate.hpp>
#include <boost/mpl/type_list.hpp>
#include <boost/preprocessor/debug/line.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace python { namespace detail {

///////////////////////////////////////////////////////////////////////////////
//
//  The following macros generate expansions for:
//
//      template <class RT, class T0... class TN>
//      inline boost::mpl::type_list<RT, T0...TN>
//      get_signature(RT(*)(T0...TN))
//      {
//          return boost::mpl::type_list<RT, T0...TN>();
//      }
//
//      template <class RT, class ClassT, class T0... class TN>
//      inline boost::mpl::type_list<RT, ClassT, T0...TN>
//      get_signature(RT(ClassT::*)(T0...TN)))
//      {
//          return boost::mpl::type_list<RT, ClassT, T0...TN>();
//      }
//
//      template <class RT, class ClassT, class T0... class TN>
//      inline boost::mpl::type_list<RT, ClassT, T0...TN>
//      get_signature(RT(ClassT::*)(T0...TN) const))
//      {
//          return boost::mpl::type_list<RT, ClassT const, T0...TN>();
//      }
//
//  These functions extract the return type, class (for member functions)
//  and arguments of the input signature and stuffs them in an mpl::type_list.
//
///////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_TEMPLATE_GEN(INDEX, DATA)  class BOOST_PP_CAT(T, INDEX)

///////////////////////////////////////////////////////////////////////////////
#define BOOST_PP_ITERATION_PARAMS_1                                             \
    (3, (0, BOOST_PYTHON_MAX_ARITY-1, <boost/python/signature.hpp>))

#include BOOST_PP_ITERATE()
#undef BOOST_PYTHON_TEMPLATE_GEN

}

}} // namespace boost::python

///////////////////////////////////////////////////////////////////////////////
#endif // SIGNATURE_JDG20020813_HPP

#else // defined(BOOST_PP_IS_ITERATING)
// PP vertical iteration code

///////////////////////////////////////////////////////////////////////////////
template <
    class RT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM(BOOST_PP_ITERATION(), BOOST_PYTHON_TEMPLATE_GEN, BOOST_PP_EMPTY)>
inline boost::mpl::type_list<
    RT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>
get_signature(RT(*)(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)))
{
    return boost::mpl::type_list<
            RT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
            BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>();
}

///////////////////////////////////////////////////////////////////////////////
#if BOOST_PP_ITERATION() <= (BOOST_PYTHON_MAX_ARITY - 2)

template <
    class RT, class ClassT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM(BOOST_PP_ITERATION(), BOOST_PYTHON_TEMPLATE_GEN, BOOST_PP_EMPTY)>
inline boost::mpl::type_list<
    RT, ClassT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>
get_signature(RT(ClassT::*)(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)))
{
    return boost::mpl::type_list<
            RT, ClassT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
            BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>();
}

///////////////////////////////////////
template <
    class RT, class ClassT BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM(BOOST_PP_ITERATION(), BOOST_PYTHON_TEMPLATE_GEN, BOOST_PP_EMPTY)>
inline boost::mpl::type_list<
    RT, ClassT const BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
    BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>
get_signature(RT(ClassT::*)(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)) const)
{
    return boost::mpl::type_list<
            RT, ClassT const
            BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
            BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), T)>();
}

#endif // BOOST_PP_ITERATION() < (BOOST_PYTHON_MAX_ARITY - 2)

#endif // !defined(BOOST_PP_IS_ITERATING)
