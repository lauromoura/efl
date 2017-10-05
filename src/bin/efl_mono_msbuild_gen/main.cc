
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include <string>
#include <algorithm>
#include <stdexcept>
#include <iosfwd>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.hh>


namespace efl_mono_msbuild_gen {

/// Program options.
struct options_type
{
   std::string out_file;
   std::vector<std::string> cs_files;
   std::vector<std::string> defines;
};

efl::eina::log_domain domain("efl_mono_msbuild_gen");

namespace {

void
_print_version()
{
   std::cerr
     << "EFL Mono MSBuild Generator (EFL "
     << PACKAGE_VERSION << ")" << std::endl;
}

static void
_usage(const char *progname)
{
   std::cerr
     << progname
     << " [options] [files]" << std::endl
     << "Options:" << std::endl
     << "  -o, --out-file <file>   The output file name. [default: <classname>.eo.hh]" << std::endl
     << "  -v, --version           Print the version." << std::endl
     << "  -h, --help              Print this help." << std::endl;
   exit(EXIT_FAILURE);
}

static void
_assert_not_dup(std::string option, std::string value)
{
   if (value != "")
     {
        EINA_CXX_DOM_LOG_ERR(efl_mono_msbuild_gen::domain) <<
          "Option -" + option + " already set (" + value + ")";
     }
}

static bool
opts_check(efl_mono_msbuild_gen::options_type const& opts)
{
   if (opts.out_file.empty())
     {
        EINA_CXX_DOM_LOG_ERR(efl_mono_msbuild_gen::domain)
          << "Nowhere to generate?" << std::endl;
     }
   else
     return true; // valid.
   return false;
}

static efl_mono_msbuild_gen::options_type
opts_get(int argc, char **argv)
{
   efl_mono_msbuild_gen::options_type opts;

   const struct option long_options[] =
     {
       { "out-file",  required_argument, 0,  'o' },
       { "define",    required_argument, 0,  'd' },
       { "version",   no_argument,       0,  'v' },
       { "help",      no_argument,       0,  'h' },
       { 0,           0,                 0,   0  }
     };
   const char* options = "o:d:vh";

   int c, idx;
   while ( (c = getopt_long(argc, argv, options, long_options, &idx)) != -1)
     {
        if (c == 'o')
          {
             _assert_not_dup("o", opts.out_file);
             opts.out_file = optarg;
          }
        else if (c == 'd')
          {
             opts.defines.push_back(optarg);
          }
        else if (c == 'h')
          {
             _usage(argv[0]);
          }
        else if (c == 'v')
          {
             _print_version();
             if (argc == 2) exit(EXIT_SUCCESS);
          }
     }

   for (int i = optind; i < argc; ++i)
     {
        opts.cs_files.push_back(argv[i]);
     }

   if (!efl_mono_msbuild_gen::opts_check(opts))
     {
        _usage(argv[0]);
        assert(false && "Wrong options passed in command-line");
     }

   return opts;
}

static void
run(options_type const& opts)
{
   std::ofstream os (opts.out_file.c_str());
   if(!os.is_open())
     {
        throw std::runtime_error(std::string{"Couldn't open output file "} + opts.out_file);
     }

   os << "<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

   os << "  <PropertyGroup>\n";
   os << "    <OutputType>Library</OutputType>\n";
   //os << "    <Version>" << opts.version << "</Version>\n"; // TODO
   if (!opts.defines.empty())
     {
        os << "    <DefineConstants>$(DefineConstants)";
        for (auto const& def : opts.defines)
          {
              os << ";" << def;
          }
        os << "</DefineConstants>\n";
     }
   os << "  </PropertyGroup>\n";

   os << "  <ItemGroup>\n";
   for (auto fname : opts.cs_files)
     {
        std::replace(fname.begin(), fname.end(), '/', '\\');
        os << "    <CSFile Include=\"src\\" << fname << "\" />\n";
     }
     os << "  </ItemGroup>\n";

   os << "  <Target Name=\"Build\">\n";
   os << "    <CSC Sources=\"@(CSFile)\" OutputAssembly=\"libefl_mono.dll\" />\n";
   os << "  </Target>\n";

   os << "</Project>\n";
}

} // anonymous namespace
} // namespace efl_mono_msbuild_gen

int main(int argc, char **argv)
{
   try
     {
        efl::eina::eina_init eina_init;
        efl_mono_msbuild_gen::options_type opts = efl_mono_msbuild_gen::opts_get(argc, argv);
        efl_mono_msbuild_gen::run(opts);
     }
   catch(std::exception const& e)
     {
        EINA_CXX_DOM_LOG_ERR(efl_mono_msbuild_gen::domain) << "EFL Mono MSBuild generation for the following reason: "  << e.what() << std::endl;
        return -1;
     }
   return 0;
}
