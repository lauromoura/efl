#ifndef EOLIAN_MONO_LIBRARY_CONTEXT_HH
#define EOLIAN_MONO_LIBRARY_CONTEXT_HH

namespace eolian_mono {

struct library_context
{
  std::string library_name;
  int v_major;
  int v_minor;

  std::string actual_library_name() const;
};

std::string
library_context::actual_library_name() const
{
    std::string name = library_name;

#ifdef _WIN32
    // Windows does not strip the 'lib' prefix.
    name = "lib" + name + "-";

    // Libtool versioning stuff.
    // On windows, the version of the library goes into the filename.
    // See https://autotools.io/libtool/windows.html#libtool.windows.version
    // Based on the current:revision:age string passed to libtool, the generated suffix
    // has the value of (current - age).
    // To get the current and age, we use the same calculation from m4/efl.m4 (lt_cur, lt_age)
    int current = (v_major == 1) ? (v_major + v_minor) : (v_major * 100 + v_minor);
    int age = v_minor;

    name += std::to_string(current - age);
#endif
    return name;
}
  
}

#endif
