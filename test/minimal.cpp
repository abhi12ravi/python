// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/module.hpp>

BOOST_PYTHON_MODULE_INIT(minimal_ext)
{
    boost::python::module m("minimal_ext");
}

#include "module_tail.cpp"
