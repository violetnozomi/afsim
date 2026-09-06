// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#if !defined(UTOPRIV_HPP)
#define UTOPRIV_HPP

//#include <map>
//#include <vector>
#include <istream>
#include <sstream>

#include <osg/Math>
#include <osg/Vec3>

#include "UtoTypes.hpp"

// Function prototypes:
std::string UtoFindFirstPath(const std::string& file_name, const std::string& search_path);
void        StripLeadingTrailing(std::string& str, const std::string& tok = " \t\r\n");
std::string UtoAbsolutePathOf(const std::string& path, const std::string& base);

// expands an environment string
std::string UtoExpandEnvString(const std::string& rString);

// canonicalize a string to
bool UtoPathCanonicalize(std::string& dst, const std::string& path);

// determines if the provided path is relative
bool UtoPathIsRelative(const std::string& path);

// determines if the file currently exists
bool UtoPathFileExists(const std::string& path);

// determine if the path is a directory
bool UtoPathIsDirectory(const std::string& path);

// UtoStrlen is a macro for char or wide-char C-runtime function.
#define UtoStrlen strlen

// used to convert a color rgb component (char) into a double.
#define Clr2Dbl(x) (((double)x) / 255)

// Degress to Radians and then reverse
inline float DtoR(float d)
{
   return osg::DegreesToRadians(d);
}
inline double DtoR(double d)
{
   return osg::DegreesToRadians(d);
}
inline float RtoD(float r)
{
   return osg::RadiansToDegrees(r);
}
inline double RtoD(double r)
{
   return osg::RadiansToDegrees(r);
}

#ifndef M_PI
// from /usr/include/math.h
#define M_PI 3.14159265358979323846 /* pi */
#endif

// These are the vertices to use when doing a rotation on an Open Scene Graph
// Matrix.
extern const osg::Vec3 YawAxis;
extern const osg::Vec3 PitchAxis;
extern const osg::Vec3 RollAxis;

template<class T>
struct DestroyObj
{
   void operator()(T* obj) { delete obj; }
};

template<class T>
class RefCntBase
{
public:
   typedef T  value_type;
   typedef T* ptr_type;
   typedef T& ref_type;

   RefCntBase()
      : m_pData(new RefCnt)
   {
      m_pData->pObject = 0;
      m_pData->nRefCnt = 0;
   }

   RefCntBase(ptr_type obj)
      : m_pData(new RefCnt)
   {
      m_pData->pObject = obj;
      m_pData->nRefCnt = 1;
   }

   RefCntBase(const RefCntBase<T>& rhs)
      : m_pData(rhs.m_pData)
   {
      ++m_pData->nRefCnt;
   }

   ~RefCntBase()
   {
      // if(0 >= m_pData->nRefCnt--)
      if (0 >= --m_pData->nRefCnt)
      {
         delete m_pData->pObject;
         delete m_pData;
      }
   }

   ref_type       operator*() { return *m_pData->pObject; }
   ptr_type       operator->() { return m_pData->pObject; }
   const ptr_type operator->() const { return m_pData->pObject; }

private:
   RefCntBase& operator=(const RefCntBase<T>&);

   struct RefCnt
   {
      ptr_type pObject;
      int      nRefCnt;
   };

   RefCnt* m_pData;
};

// for such as models_path for separating multiple paths
#if _WIN32
const char* const UtoSearchDelimiter = ";,";
#else
const char* const UtoSearchDelimiter = ":";
#endif

#if 0
// Define multi-threading models.
struct UtoMTThreadModel
{
   typedef AtomicOp<int> IntType;
   typedef AtomicOp<double> DoubleType;
   typedef AtomicOp<short> ShortType;
   typedef AtomicOp<long> LongType;
   typedef Mutex MutexType;
   typedef RWLock RWLockType;
};

struct UtoSTThreadModel
{
   typedef int IntType;
   typedef double DoubleType;
   typedef short ShortType;
   typedef long LongType;
   typedef NullMutex MutexType;
   typedef NullRWLock RWLockType;
};

#ifdef UTO_IS_MT
typedef UtoMTThreadModel UtoDefaultThreadModel;
#else
typedef UtoSTThreadModel UtoDefaultThreadModel;
#endif

// Define memory allocation helper classes.
struct SmallObject
{
   operator new() {}
   operator delete() {}
};

#endif

template<class T>
class StringTok
{
   // modified from Jim Hyslop & Herb Sutter, Conversations: Al-Go-Rithms, CUJ 10/02
public:
   StringTok(const T& seq, typename T::size_type pos = 0)
      : m_Seq(seq)
      , m_Pos(pos)
   {
   }

   T operator()(const T& delim);

   bool done() const { return m_Pos == T::npos; }

private:
   const T&              m_Seq;
   typename T::size_type m_Pos;
};

template<class T>
T StringTok<T>::operator()(const T& delim)
{
   T token;

   if (m_Pos != T::npos)
   {
      // start of found token
      typename T::size_type first = m_Seq.find_first_not_of(delim.c_str(), m_Pos);

      if (first != T::npos)
      {
         // length of found token
         typename T::size_type num = m_Seq.find_first_of(delim.c_str(), first) - first;

         // do all the work off to the side
         token = m_Seq.substr(first, num);

         // done; now commit using
         // nonthrowing operations only
         m_Pos = first + num;
         if (m_Pos != T::npos)
            ++m_Pos;
         if (m_Pos >= m_Seq.size())
            m_Pos = T::npos;
      }
   }

   return token;
}

template<class InsIt, typename E>
void ExtractSubStrings(InsIt insert, const std::basic_string<E>& strings, const E* delim)
{
   // search <sym_tokens> for comma separated strings.
   StringTok<std::string> tokenizer(strings);
   while (!tokenizer.done())
   {
      std::string token = tokenizer(delim);
      if (!token.empty())
      {
         insert++ = token;
      }
   }
}

class UtoHashCompare
{ // traits class for hash containers
public:
   enum
   {                   // parameters for hash table
      bucket_size = 4, // 0 < bucket_size
      min_buckets = 8  // min_buckets = 2 ^^ N, 0 < N
   };

   size_t operator()(const std::string& key) const
   { // hash key to size_t value
      size_t       value = 0;
      size_t       size  = key.size();
      unsigned int n     = 0;
      while (n < size)
      {
         value = (value << 5) + value + key[n++];
      }
      return value;
   }

   bool operator()(const std::string& k1, const std::string& k2) const
   {
      // test if k1 equals k2, the original SGI and unordered_map symantics
      return k1 == k2;
   }
};

template<class T>
class UtoPtrHashCompare
{ // traits class for hash containers
public:
   enum
   {                   // parameters for hash table
      bucket_size = 4, // 0 < bucket_size
      min_buckets = 8  // min_buckets = 2 ^^ N, 0 < N
   };

   size_t operator()(const T* p) const
   { // hash key to size_t value
      return (size_t)p;
   }

   bool operator()(const T* k1, const T* k2) const
   {
      // test if k1 equals k2, the original SGI and unordered_map symantics
      return k1 == k2;
   }
};

namespace Utok
{
template<class T>
int ReadSingle(const std::string& in, T& value)
{
   // "<x>"
   std::istringstream sstr(in);
   sstr >> value;
   if (sstr.fail())
      return -1;
   return 0;
}

template<class T>
int ReadDouble(const std::string& in, T (&xy)[2])
{
   // "<x> <y>"
   std::istringstream sstr(in);
   sstr >> xy[0];
   if (sstr.fail())
      return -1;

   sstr >> xy[1];
   if (sstr.fail())
      return -1;

   return 0;
}

template<class T>
int ReadTriple(const std::string& in, T (&xyz)[3])
{
   // "<x> <y> <z>"
   std::istringstream sstr(in);
   sstr >> xyz[0];
   if (sstr.fail())
      return -1;

   sstr >> xyz[1];
   if (sstr.fail())
      return -1;

   sstr >> xyz[2];
   if (sstr.fail())
      return -1;

   return 0;
}
} // namespace Utok

#endif // !defined(UTOPRIV_HPP)
