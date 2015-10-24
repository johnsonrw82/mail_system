/*****************************************************************************
** WORKAROUND - gcc bug #52015  - std::to_string does not work on Windows platforms, including
**                                Cygwin and MinGW. (http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52015)
**
** Last updated:  December 26, 2013.  gcc version  4.8.1 and before
** Reference:     ISO/IEC 14882:2011(E) 21.3 (1), and 21.5 - numeric conversions:
**
** Envision this file included by specifying   -include "to_string.hxx"   on the g++ command line so the baseline code is not
** impacted, and is unaware.
**
** C++11 added various overloaded forms of std::to_string in the <string> library, but gcc has a (long) outstanding bug that
** prevents these from being available.  This is a workaround until I can get a gcc patch and addresses only the to_string() and
** to_wstring() functions.  Easy to add the others if ever needed.
**
**    21.3 (1) ... 21.5, numeric conversions:
**        int stoi(const string& str, size_t *idx = 0, int base = 10);
**        long stol(const string& str, size_t *idx = 0, int base = 10);
**        unsigned long stoul(const string& str, size_t *idx = 0, int base = 10);
**        long long stoll(const string& str, size_t *idx = 0, int base = 10);
**        unsigned long long stoull(const string& str, size_t *idx = 0, int base = 10);
**        float stof(const string& str, size_t *idx = 0);
**        double stod(const string& str, size_t *idx = 0);
**        long double stold(const string& str, size_t *idx = 0);
**        string to_string(int val);
**        string to_string(unsigned val);
**        string to_string(long val);
**        string to_string(unsigned long val);
**        string to_string(long long val);
**        string to_string(unsigned long long val);
**        string to_string(float val);
**        string to_string(double val);
**        string to_string(long double val);
**        int stoi(const wstring& str, size_t *idx = 0, int base = 10);
**        long stol(const wstring& str, size_t *idx = 0, int base = 10);
**        unsigned long stoul(const wstring& str, size_t *idx = 0, int base = 10);
**        long long stoll(const wstring& str, size_t *idx = 0, int base = 10);
**        unsigned long long stoull(const wstring& str, size_t *idx = 0, int base = 10);
**        float stof(const wstring& str, size_t *idx = 0);
**        double stod(const wstring& str, size_t *idx = 0);
**        long double stold(const wstring& str, size_t *idx = 0);
**        wstring to_wstring(int val);
**        wstring to_wstring(unsigned val);
**        wstring to_wstring(long val);
**        wstring to_wstring(unsigned long val);
**        wstring to_wstring(long long val);
**        wstring to_wstring(unsigned long long val);
**        wstring to_wstring(float val);
**        wstring to_wstring(double val);
**        wstring to_wstring(long double val);
**
** Tom Bettens
**       Updated 26-DEC-2013 Replaced templates with inline functions.  The template's argument type "const T&" caused an undefined
**       symbol link error when an initialized constant integral class (static) attribute without an external class definition was
**       passed as a parameter into the to_string function.  The function's argument type of reference-to T constant caused the
**       error when no memory was allocated outside the class for constant integral class (static) attributes.  The standard says
**       the arguments should be copies of, not references to the parameter.  Fixing that resolved the link problem, but while I was
**       at it I replaced the templates with manually overloaded functions to prevent providing more capability that the standard
**       prescribes.
*******************************************************************************/
#if (__GNUC__ && __cplusplus)  // if compiling C++ code with GNU's gcc compiler
  #ifndef TO_STRING_52015_WORKAROUND_HXX
    #include <iostream>
    #include <string>
    #include <sstream>

    namespace std
    {
      // The standard overloads only the [signed | unsigned] [int | long | long long] plus the floating point types for T so this is
      // way more than we have otherwise, but it's only a patch, right? As is, any type that supports the insertion operator will
      // instantiate these functions, so don't get too over zealous.
      //
      //    template<typename T> std::string  to_string (T val) { std::ostringstream ss; ss << val; return ss.str(); }
      //
      //    template<typename T> std::wstring to_wstring(T val) { std::wostringstream ss; ss << val; return ss.str(); }




      inline string to_string(int                val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(unsigned           val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(long               val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(unsigned long      val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(long long          val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(unsigned long long val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(float              val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(double             val)         { std::ostringstream ss; ss << val; return ss.str(); }
      inline string to_string(long double        val)         { std::ostringstream ss; ss << val; return ss.str(); }




      inline wstring to_wstring(int                val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(unsigned           val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(long               val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(unsigned long      val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(long long          val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(unsigned long long val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(float              val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(double             val)       { std::wostringstream ss; ss << val; return ss.str(); }
      inline wstring to_wstring(long double        val)       { std::wostringstream ss; ss << val; return ss.str(); }
    }
  #endif
#endif
