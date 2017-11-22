#ifndef EOLIAN_MONO_FUNCTION_DECLARATION_HH
#define EOLIAN_MONO_FUNCTION_DECLARATION_HH

#include "grammar/generator.hpp"
#include "grammar/klass_def.hpp"

#include "grammar/indentation.hpp"
#include "grammar/list.hpp"
#include "grammar/alternative.hpp"
#include "type.hh"
#include "parameter.hh"
#include "keyword.hh"
#include "using_decl.hh"

namespace eolian_mono {

struct function_declaration_generator
{
  template <typename OutputIterator, typename Context>
  bool generate(OutputIterator sink, attributes::function_def const& f, Context const& context) const
  {
    if(is_function_blacklisted(f.c_name))
      return true;
    else
      return as_generator
        (eolian_mono::type(true) << " " << string << "(" << (parameter % ", ") << ");\n")
        .generate(sink, std::make_tuple(f.return_type, escape_keyword(f.name), f.parameters), context);
  }
};

function_declaration_generator const function_declaration = {};

}

namespace efl { namespace eolian { namespace grammar {

template <>
struct is_eager_generator< ::eolian_mono::function_declaration_generator> : std::true_type {};
template <>
struct is_generator< ::eolian_mono::function_declaration_generator> : std::true_type {};

namespace type_traits {
template <>
struct attributes_needed< ::eolian_mono::function_declaration_generator> : std::integral_constant<int, 1> {};
}
      
} } }

#endif
