%module pugixml
%{
	// include any needed headers here
	#include "../libs/pugixml-1.10/src/pugixml.hpp"
%}
// include support for some common C++ STL types you might be using,
// see http://swig.org/Doc3.0/Library.html#Library_stl_cpp_library
%include <stl.i>
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>

// if you're using "string" instead of "std::string" (like the openFrameworks source code),
// you'll need this so swig will know that a "string" can be considered a "std::string"
typedef std::string string;

// include custom code you want to be wrapped,
// note the '%' instead of '#' as '%include' is a SWIG
// command to wrap code in the given header
%include "../libs/pugixml-1.10/src/pugixml.hpp"


// process with: swig -c++ -lua -fcompact -fvirtual -I../../../libs/openFrameworks luaBindings.i
