//  (C) Copyright David Abrahams 2000. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  The author gratefully acknowleges the support of Dragon Systems, Inc., in
//  producing this work.

#include "newtypes.h"
#include "pyptr.h" // for handle_exception()
#include "module.h"
#include "none.h"
#include <cstring>
#include <vector>
#include <cstddef>
#include <boost/smart_ptr.hpp>
#include "objects.h"

namespace py {

namespace detail {
UniquePodSet& UniquePodSet::instance()
{
    static UniquePodSet me;
    return me;
}

struct UniquePodSet::Compare
{
    bool operator()(const std::pair<const char*, const char*>& x1,
                   const std::pair<const char*, const char*>& x2) const
    {
        const std::ptrdiff_t n1 = x1.second - x1.first;
        const std::ptrdiff_t n2 = x2.second - x2.first;
        return n1 < n2 || n1 == n2 && PY_CSTD_::memcmp(x1.first, x2.first, n1) < 0;
    }
};

const void* UniquePodSet::get_element(const void* buffer, std::size_t size)
{
    const Holder element(static_cast<const char*>(buffer),
                   static_cast<const char*>(buffer) + size);
    
    const Storage::iterator found
        = std::lower_bound(m_storage.begin(), m_storage.end(), element, Compare());
    
    if (found != m_storage.end() && !Compare()(element, *found))
        return found->first;

    std::size_t length = element.second - element.first;
    char* base_address = new char[length];
    try {
        PY_CSTD_::memcpy(base_address, element.first, length);
        Holder new_element(base_address, base_address + length);
        m_storage.insert(found, new_element);
    }
    catch(...) {
        delete[] base_address;
        throw;
    }
    return base_address;
}

UniquePodSet::~UniquePodSet()
{
    for (Storage::const_iterator p = m_storage.begin(), finish = m_storage.end();
         p != finish; ++p)
    {
        delete[] const_cast<char*>(p->first);
    }
}
} // namespace detail

template <class MethodStruct, class MemberPtr, class Fn>
static MethodStruct* enable_method(const MethodStruct* base, MemberPtr p, Fn f)
{
    MethodStruct new_value;
    
    if (base != 0)
        new_value = *base;
    else
        PY_CSTD_::memset(&new_value, 0, sizeof(PyMappingMethods));
    
    new_value.*p = f;
    
    return const_cast<MethodStruct*>(detail::UniquePodSet::instance().get(new_value));
}

// TODO: is there a problem with calling convention here, or can I really pass a
// pointer to a C++ linkage function as a C-linkage function pointer? The
// compilers seem to swallow it, but is it legal? Symantec C++ for Mac didn't
// behave this way, FWIW.
// Using C++ linkage allows us to keep the virtual function members of
// TypeObjectBase private and use friendship to get them called.

extern "C" {

static PyObject* do_instance_repr(PyObject* instance)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_repr(instance);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static int do_instance_compare(PyObject* instance, PyObject* other)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_compare(instance, other);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static PyObject* do_instance_str(PyObject* instance)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_str(instance);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static long do_instance_hash(PyObject* instance)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_hash(instance);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}
    
static PyObject* do_instance_call(PyObject* instance, PyObject* args, PyObject* keywords)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_call(instance, args, keywords);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static void do_instance_dealloc(PyObject* instance)
{
    try
    {
        static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_dealloc(instance);
    }
    catch(...)
    {
        assert(!"exception during destruction!");
        handle_exception();
    }
}

static PyObject* do_instance_getattr(PyObject* instance, char* name)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_getattr(instance, name);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static int do_instance_setattr(PyObject* instance, char* name, PyObject* value)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_setattr(instance, name, value);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static int do_instance_mp_length(PyObject* instance)
{
    try
    {
        const int outcome =
            static_cast<TypeObjectBase*>(instance->ob_type)
                ->instance_mapping_length(instance);
        
        if (outcome < 0)
        {
            PyErr_SetString(PyExc_ValueError, "__len__() should return >= 0");
            return -1;
        }
        return outcome;
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static int do_instance_sq_length(PyObject* instance)
{
    try
    {
        const int outcome =
            static_cast<TypeObjectBase*>(instance->ob_type)
                ->instance_sequence_length(instance);
        
        if (outcome < 0)
        {
            PyErr_SetString(PyExc_ValueError, "__len__() should return >= 0");
            return -1;
        }
        return outcome;
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static PyObject* do_instance_mp_subscript(PyObject* instance, PyObject* index)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_mapping_subscript(instance, index);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static PyObject* do_instance_sq_item(PyObject* instance, int index)
{
    try
    {
        const PyTypeObject* const type = instance->ob_type;
        
        // This is an extension to standard class behavior. If sequence_length
        // is implemented and n >= sequence_length(), raise an IndexError. That
        // keeps users from having to worry about raising it themselves
        if (type->tp_as_sequence != 0 && type->tp_as_sequence->sq_length != 0
            && index >= type->tp_as_sequence->sq_length(instance))
        {
            PyErr_SetString(PyExc_IndexError, type->tp_name);
            return 0;
        }
    
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_item(instance, index);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static int do_instance_mp_ass_subscript(PyObject* instance, PyObject* index, PyObject* value)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_mapping_ass_subscript(instance, index, value);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static int do_instance_sq_ass_item(PyObject* instance, int index, PyObject* value)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_ass_item(instance, index, value);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

static PyObject* do_instance_sq_concat(PyObject* instance, PyObject* other)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_concat(instance, other);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static PyObject* do_instance_sq_repeat(PyObject* instance, int n)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_repeat(instance, n);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static PyObject* do_instance_sq_slice(
    PyObject* instance, int start, int finish)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_slice(instance, start, finish);
    }
    catch(...)
    {
        handle_exception();
        return 0;
    }
}

static int do_instance_sq_ass_slice(
    PyObject* instance, int start, int finish, PyObject* value)
{
    try
    {
        return static_cast<TypeObjectBase*>(instance->ob_type)
            ->instance_sequence_ass_slice(instance, start, finish, value);
    }
    catch(...)
    {
        handle_exception();
        return -1;
    }
}

}

namespace detail  {
template <std::size_t> struct category_type;

#define DECLARE_CAPABILITY_TYPE(field, sub_structure) \
     template <> \
     struct category_type<(offsetof(PyTypeObject, tp_as_##field))> \
     { \
         typedef sub_structure type; \
     }

#define CAPABILITY(field) \
        { offsetof(PyTypeObject, tp_##field), 0, Dispatch(do_instance_##field), 0, -1 }

#define CAPABILITY2(category, field) \
        { offsetof(PyTypeObject, tp_as_##category), \
          offsetof(category_type<offsetof(PyTypeObject, tp_as_##category)>::type, field), \
          Dispatch(do_instance_##field), \
          sizeof(category_type<offsetof(PyTypeObject, tp_as_##category)>::type), \
          offsetof(AllMethods, category) \
        }

DECLARE_CAPABILITY_TYPE(mapping, PyMappingMethods);
DECLARE_CAPABILITY_TYPE(sequence, PySequenceMethods);

const CapabilityEntry capabilities[] = {
    CAPABILITY(hash),
    CAPABILITY(call),
    CAPABILITY(str),
    CAPABILITY(getattr),
    CAPABILITY(setattr),
    CAPABILITY(compare),
    CAPABILITY(repr),

    CAPABILITY2(mapping, mp_length),
    CAPABILITY2(mapping, mp_subscript),
    CAPABILITY2(mapping, mp_ass_subscript),
    
    CAPABILITY2(sequence, sq_length),
    CAPABILITY2(sequence, sq_item),
    CAPABILITY2(sequence, sq_ass_item),
    CAPABILITY2(sequence, sq_concat),
    CAPABILITY2(sequence, sq_repeat),
    CAPABILITY2(sequence, sq_slice),
    CAPABILITY2(sequence, sq_ass_slice)
};

  const std::size_t num_capabilities = PY_ARRAY_LENGTH(capabilities);

  void add_capability(
      std::size_t n,
      PyTypeObject* dest_,
      AllMethods& all_methods,
      const PyTypeObject* src_)
  {
      assert(n < PY_ARRAY_LENGTH(capabilities));
      const CapabilityEntry& c = capabilities[n];

      const char* const* src = src_ ? reinterpret_cast<const char* const*>(
          reinterpret_cast<const char*>(src_) + c.offset1) : 0;

      char** const dest = reinterpret_cast<char**>(
          reinterpret_cast<char*>(dest_) + c.offset1);
            
      if (c.substructure_size == 0)
      {
          if (src == 0 || 
#if defined(__MWERKS__) && __MWERKS__ <= 0x4000
          ((const Dispatch*)src)
#else
          reinterpret_cast<const Dispatch*>(src)
#endif
           != 0) {
              *reinterpret_cast<Dispatch*>(dest) = c.dispatch;
          }
      }
      else
      {
          if (src == 0 ||
              *src != 0 && *reinterpret_cast<const Dispatch*>(*src + c.offset2) != 0)
          {
              *dest = reinterpret_cast<char*>(&all_methods) + c.allmethods_offset;
              *reinterpret_cast<Dispatch*>(*dest + c.offset2) = c.dispatch;
          }
      }
      
  }
} // namespace detail

namespace {
  union SubStructures {
      PyMappingMethods mapping;
      PySequenceMethods sequence;
      PyNumberMethods number;
      PyBufferProcs buffer;
  };
}

void TypeObjectBase::enable(TypeObjectBase::Capability capability)
{
    using detail::capabilities;
    using detail::CapabilityEntry;
    using detail::Dispatch;

    assert((std::size_t)capability < PY_ARRAY_LENGTH(capabilities));
    const CapabilityEntry& c = capabilities[capability];
    
    PyTypeObject* const me = this;
    char* const base_address = reinterpret_cast<char*>(me);
    
    if (c.substructure_size == 0)
    {        
        // Stuff the dispatch function directly into the PyTypeObject
        *reinterpret_cast<Dispatch*>(base_address + c.offset1) = c.dispatch;
        return;
    }
    
    const char*& sub_structure = *reinterpret_cast<const char**>(base_address + c.offset1);

    // Initialize this POD union with the current state-of-the-world
    SubStructures sub;
    if (sub_structure == 0)
        PY_CSTD_::memset(&sub, 0, c.substructure_size);
    else
        PY_CSTD_::memcpy(&sub, sub_structure, c.substructure_size);

    // Stuff the dispatch function into the sub-structure
    *reinterpret_cast<Dispatch*>(reinterpret_cast<char*>(&sub) + c.offset2) = c.dispatch;

    // Retrieve the unique dynamically-allocated substructure and stuff it into
    // the PyTypeObject.
    sub_structure = static_cast<const char*>(
        detail::UniquePodSet::instance().get_element(&sub, c.substructure_size));
}


TypeObjectBase::TypeObjectBase(PyTypeObject* t)
    : PythonType(t)
{
    this->tp_dealloc = do_instance_dealloc;
}

namespace {
  struct ErrorType {
      operator PyObject*() const { return 0; }
      operator int() const { return -1; }
  };

  ErrorType unimplemented(const char* name)
  {
      assert(!"Control should never reach here");
      String s("Unimplemented ");
      s += String(name);
      PyErr_SetObject(PyExc_RuntimeError, s.get());
      return ErrorType();
  }
}

PyObject* TypeObjectBase::instance_repr(PyObject*) const
{
    return unimplemented("instance_repr");
}

int TypeObjectBase::instance_compare(PyObject*, PyObject*) const
{
    return unimplemented("instance_compare");
}

PyObject* TypeObjectBase::instance_str(PyObject*) const
{
    return unimplemented("instance_str");
}

long TypeObjectBase::instance_hash(PyObject* /* instance */) const
{
    return unimplemented("instance_hash");
}

PyObject* TypeObjectBase::instance_call(PyObject* /*instance*/, PyObject* /*args*/, PyObject* /*kw*/) const
{
    return unimplemented("instance_call");
}

PyObject* TypeObjectBase::instance_getattr(PyObject* /*instance*/, const char* /*name*/) const
{
    return unimplemented("instance_getattr");
}

int TypeObjectBase::instance_setattr(PyObject* /*instance*/, const char* /*name*/, PyObject* /*value*/) const
{
    return unimplemented("instance_setattr");
}

int TypeObjectBase::instance_mapping_length(PyObject*) const
{
    return unimplemented("instance_mapping_length");
}

int TypeObjectBase::instance_sequence_length(PyObject*) const
{
    return unimplemented("instance_sequence_length");
}

PyObject* TypeObjectBase::instance_mapping_subscript(PyObject*, PyObject*) const
{
    return unimplemented("instance_mapping_subscript");
}

PyObject* TypeObjectBase::instance_sequence_item(PyObject*, int) const
{
    return unimplemented("instance_sequence_item");
}

int TypeObjectBase::instance_mapping_ass_subscript(PyObject*, PyObject*, PyObject*) const
{
    return unimplemented("instance_mapping_ass_subscript");
}

int TypeObjectBase::instance_sequence_ass_item(PyObject*, int, PyObject*) const
{
    return unimplemented("instance_sequence_ass_item");
}

PyObject* TypeObjectBase::instance_sequence_concat(PyObject*, PyObject*) const
{
    return unimplemented("instance_sequence_concat");
}

PyObject* TypeObjectBase::instance_sequence_repeat(PyObject*, int) const
{
    return unimplemented("instance_sequence_repeat");
}

PyObject* TypeObjectBase::instance_sequence_slice(PyObject*, int, int) const
{
    return unimplemented("instance_sequence_slice");
}

int TypeObjectBase::instance_sequence_ass_slice(PyObject*, int, int, PyObject*) const
{
    return unimplemented("instance_sequence_ass_slice");
}

TypeObjectBase::~TypeObjectBase()
{
}

}
