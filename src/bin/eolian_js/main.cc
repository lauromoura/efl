
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eolian.h>

#include <Eina.hh>

#include <eolian/js/domain.hh>
#include <eolian/class.hh>

#include <iostream>
#include <fstream>

#include <getopt.h>
#include <cstdlib>
#include <vector>


int main(int argc, char** argv)
{
  std::vector<std::string> include_paths;
  std::string out_file, in_file;

  efl::eina::eina_init eina_init;
  struct eolian_init
  {
    eolian_init() { ::eolian_init(); }
    ~eolian_init() { ::eolian_shutdown(); }
  } eolian_init;
  
  const struct option long_options[] =
    {
       { "in",        required_argument, 0,  'I' },
       { "out-file",  required_argument, 0,  'o' },
       { "version",   no_argument,       0,  'v' },
       { "help",      no_argument,       0,  'h' },
       { 0,           0,                 0,   0  }
    };
   const char* options = "I:D:o:c:arvh";

   int c, idx;
   while ( (c = getopt_long(argc, argv, options, long_options, &idx)) != -1)
     {
        if (c == 'I')
          {
            include_paths.push_back(optarg);
          }
        else if (c == 'o')
          {
            if(!out_file.empty())
              {
                // _usage(argv[0]);
                return 1;
              }
            out_file = optarg;
          }
        else if (c == 'h')
          {
             // _usage(argv[0]);
            return 1;
          }
        else if (c == 'v')
          {
             // _print_version();
             // if (argc == 2) exit(EXIT_SUCCESS);
          }
     }

   if (optind == argc-1)
     {
        in_file = argv[optind];
     }


   for(auto src : include_paths)
     if (!::eolian_directory_scan(src.c_str()))
       {
         EINA_CXX_DOM_LOG_WARN(eolian::js::domain)
           << "Couldn't load eolian from '" << src << "'.";
       }
   if (!::eolian_all_eot_files_parse())
     {
       EINA_CXX_DOM_LOG_ERR(eolian::js::domain)
         << "Eolian failed parsing eot files";
       assert(false && "Error parsing eot files");
     }
   if (!::eolian_eo_file_parse(in_file.c_str()))
     {
       EINA_CXX_DOM_LOG_ERR(eolian::js::domain)
         << "Failed parsing: " << in_file << ".";
       assert(false && "Error parsing input file");
     }

   const Eolian_Class *klass = NULL;
   {
     char* dup = strdup(in_file.c_str());
     char *bn = basename(dup);
     klass = ::eolian_class_get_by_file(bn);
     free(dup);
   }
   if(!klass)
     {
       EINA_CXX_DOM_LOG_ERR(eolian::js::domain) << "could not find any class defined in this eo file";
       return -1;
     }

   std::vector<Eolian_Function const*> constructor_functions;
   std::vector<Eolian_Function const*> normal_functions;

   auto separate_functions = [&] (Eolian_Class const* klass, Eolian_Function_Type t)
     {
       efl::eina::iterator<Eolian_Function> first ( ::eolian_class_functions_get(klass, t) )
       , last;
       for(; first != last; ++first)
         {
           Eolian_Function const* function = &*first;
           std::cout << "function " << ::eolian_function_full_c_name_get(function) << std::endl;
           if( ::eolian_function_is_constructor(function, klass))
             // constructor_functions.push_back(function);
             ;
           else
             normal_functions.push_back(function);
         }
     };
   separate_functions(klass, EOLIAN_METHOD);
   separate_functions(klass, EOLIAN_PROPERTY);

   std::function<void(Eolian_Class const*)> recurse_inherits
     = [&] (Eolian_Class const* klass)
     {
   for(efl::eina::iterator<const char> first ( ::eolian_class_inherits_get(klass))
         , last; first != last; ++first)
     {
       std::cout << "base " << &*first << std::endl;
       Eolian_Class const* base = ::eolian_class_get_by_name(&*first);
       separate_functions(base, EOLIAN_METHOD);
       separate_functions(base, EOLIAN_PROPERTY);
       recurse_inherits(base);
     }
     };
   recurse_inherits(klass);
   
   std::ofstream os (out_file.c_str());
   if(!os.is_open())
     {
       EINA_CXX_DOM_LOG_ERR(eolian::js::domain) << "Couldn't open output file " << out_file;
       return -1;
     }

   std::string class_name (name(klass)), upper_case_class_name(class_name)
     , lower_case_class_name(class_name);
   std::transform(upper_case_class_name.begin(), upper_case_class_name.end(), upper_case_class_name.begin()
                  , [] (unsigned char c) { return std::toupper(c); });
   std::transform(lower_case_class_name.begin(), lower_case_class_name.end(), lower_case_class_name.begin()
                  , [] (unsigned char c) { return std::tolower(c); });
   os << "#ifndef EFL_GENERATED_EOLIAN_CLASS_GUARD_" << upper_case_class_name << "_H\n";
   os << "#define EFL_GENERATED_EOLIAN_CLASS_GUARD_" << upper_case_class_name << "_H\n\n";


   os << "#ifdef HAVE_CONFIG_H\n";
   os << "#include \"config.h\"\n";
   os << "#endif\n";
   os << "extern \"C\"\n";
   os << "{\n";
   os << "#include <Efl.h>\n";
   os << "}\n";
   os << "#include <Eo_Js.hh>\n\n";
   os << "#include <Eo.h>\n\n";
   os << "#include <node/v8.h>\n\n";
   os << "extern \"C\" {\n";

   if(is_evas(klass))
     os << "#include <Evas.h>\n";
   
   os << "#include <" << eolian_class_file_get(klass) << ".h>\n\n";
   os << "}\n";

   if(namespace_size(klass))
     {
       os << "namespace ";
       print_lower_case_namespace(klass, os);
       os << " {\n";
     }
   
   os << "EAPI void register_" << lower_case_class_name
      << "(v8::Handle<v8::Object> global, v8::Isolate* isolate)\n";
   os << "{\n";
   os << "  v8::Handle<v8::FunctionTemplate> constructor = v8::FunctionTemplate::New\n";
   os << "    (/*isolate,*/ efl::eo::js::constructor\n"
      << "     , efl::eo::js::constructor_data(isolate\n"
         "         , ";
   print_eo_class(klass, os);

   for(auto function : constructor_functions)
     {
       os << "\n         , & ::"
          << eolian_function_full_c_name_get(function);
     }
   
   os  << "));\n";
   os << "  constructor->SetClassName(v8::String::New/*FromUtf8(isolate,*/( \""
      << class_name
      << "\"));\n";
   os << "  v8::Handle<v8::ObjectTemplate> instance = constructor->InstanceTemplate();\n";
   os << "  instance->SetInternalFieldCount(1);\n";

   if(!normal_functions.empty())
     os << "  v8::Handle<v8::ObjectTemplate> prototype = constructor->PrototypeTemplate();\n";

   for(auto function : normal_functions)
     {
       auto output_begin = [&] (std::string name)
         {
           if(! ::eolian_function_is_constructor(function, klass))
             os << "  prototype->Set( ::v8::String::New/*FromUtf8(isolate,*/( \""
                << name << "\")\n"
                << "    , v8::FunctionTemplate::New(/*isolate,*/ &efl::eo::js::call_function\n"
                << "    , efl::eo::js::call_function_data<\n"
                << "      ::efl::eina::_mpl::tuple_c<std::size_t";
         };
       switch(eolian_function_type_get(function))
         {
         case EOLIAN_METHOD:
           output_begin(eolian_function_name_get(function));
           break;
         case EOLIAN_PROPERTY:
           output_begin(eolian_function_name_get(function) + std::string("_set"));
           break;
         // case EOLIAN_PROP_GET:
         //   output_begin(eolian_function_name_get(function) + std::string("_get"));
         //   break;
         // case EOLIAN_PROP_SET:
         //   output_begin(eolian_function_name_get(function) + std::string("_set"));
         case EOLIAN_PROP_GET:
         case EOLIAN_PROP_SET:
         default:
           continue;
         }
       // os << __func__ << ":" << __LINE__;
       std::size_t i = 0;
       for(efl::eina::iterator< ::Eolian_Function_Parameter> parameter
             ( ::eolian_function_parameters_get(function) )
             , last; parameter != last; ++parameter, ++i)
         {
           // os << __func__ << ":" << __LINE__;
           switch(eolian_parameter_direction_get(&*parameter))
             {
             case EOLIAN_IN_PARAM:
             case EOLIAN_INOUT_PARAM:
               os << ", " << i;
             default: break;
             }
         }
       // os << __func__ << ":" << __LINE__;
       os << ">\n      , ::efl::eina::_mpl::tuple_c<std::size_t";
       i = 0;
       for(efl::eina::iterator< ::Eolian_Function_Parameter> parameter
             ( ::eolian_function_parameters_get(function) )
             , last; parameter != last; ++parameter, ++i)
         {
           switch(eolian_parameter_direction_get(&*parameter))
             {
             case EOLIAN_OUT_PARAM:
             case EOLIAN_INOUT_PARAM:
               os << ", " << i;
             default: break;
             }
         }
       os << ">\n      , std::tuple<\n";
       efl::eina::iterator< ::Eolian_Function_Parameter> parameter
             ( ::eolian_function_parameters_get(function) )
         , last;
       // os << __func__ << ":" << __LINE__;
       while(parameter != last)
         {
           // os << __func__ << ":" << __LINE__;
           Eolian_Type const* type = eolian_parameter_type_get(&*parameter);
           if(eolian_type_is_own(type))
             os << "       ::std::true_type";
           else
             os << "       ::std::false_type";
           if(++parameter != last)
             os << ",\n";
         }
       // // os << __func__ << ":" << __LINE__;
       auto output_end = [&] (std::string const& name)
         {
           os << "> >(isolate, & ::" << name << ")));\n";
         };
       switch(eolian_function_type_get(function))
         {
         case EOLIAN_METHOD:
           output_end(eolian_function_full_c_name_get(function));
           break;
         case EOLIAN_PROPERTY:
           output_end(eolian_function_full_c_name_get(function) + std::string("_set"));
           break;
         case EOLIAN_PROP_SET:
           output_end(eolian_function_full_c_name_get(function) + std::string("_set"));
           break;
         case EOLIAN_PROP_GET:
           output_end(eolian_function_full_c_name_get(function) + std::string("_get"));
           break;
         }
     }

   os << "  global->Set(v8::String::New/*FromUtf8(isolate,*/( \""
      << class_name << "\")"
      << ", constructor->GetFunction());\n";

   os << "}\n";

   for(std::size_t i = 0, j = namespace_size(klass); i != j; ++i)
     os << "}";
   os << "\n";
   os << "\n#endif\n\n";
}
