// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/module.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/class.hpp>
#include <boost/mpl/type_list.hpp>

using namespace boost::python;
using boost::mpl::type_list;

int a_instances = 0;

int num_a_instances() { return a_instances; }

struct inner
{
    inner(std::string const& s)
        : s(s)
    {}

    void change(std::string const& new_s)
    {
        this->s = new_s;
    }
    
    std::string s;
};

struct A
{
    A(std::string const& s)
        : x(s)
    {
        ++a_instances;
    }
    
    ~A()
    {
        --a_instances;
    }

    std::string content() const
    {
        return x.s;
    }

    inner& get_inner()
    {
        return x;
    }

    inner x;
};

A* create(std::string const& s)
{
    return new A(s);
}

BOOST_PYTHON_MODULE_INIT(test_pointer_adoption_ext)
{
    boost::python::module("test_pointer_adoption_ext")
        .def("num_a_instances", num_a_instances)

        // Specify the manage_new_object return policy to take
        // ownership of create's result
        .def("create", create, return_value_policy<manage_new_object>())
        
        .add(
            
            class_<A>()
            .def("content", &A::content)
            .def("get_inner", &A::get_inner, return_value_policy<reference_existing_object>())
            )

        .add(
            class_<inner>()
            .def("change", &inner::change)
            )
        
        ;
}

