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

#if !defined(UTOTYPES_H)
#define UTOTYPES_H

#include "utilosg_export.h"

#include <exception>
#include <iomanip>
#include <istream>
#include <map>
#include <ostream>
#include <string>

#include <osg/Vec3d>

typedef std::basic_string<wchar_t> UtoWString;

class UtoEntity;
class UtoShape;
class UtoOverlay;
typedef std::map<std::string, UtoShape*> UtoShapeMap;

#ifdef _WIN32
#include <basetsd.h>
// From windef.h
#ifdef _WIN64
typedef INT_PTR(__stdcall* FARPROC)();
#else
typedef int(__stdcall* FARPROC)();
#endif // _WIN64

typedef void*   UtoWindowId;
typedef void*   UtoModuleID;
typedef FARPROC UtoProcID;
#else
typedef void* UtoWindowId;
typedef int   UtoModuleID;
typedef int   UtoProcID;
#endif


/**
\ingroup types
\class UtoPosition
\brief A location relative to a coordinate system.

UtoPosition represents the location relative to the coordinate system used
by Uto to draw shapes in space.  X is +East, Y is +North, and Z is +Up (ENU).

UtoPosition is specified in feet or pixels. If the position is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet. If the shape is being used
by a window object (i.e., UtoWindow::iterator), then the units are pixels.
*/
typedef osg::Vec3d UtoPosition;

inline UtoPosition UtoPositionFromArray(const double* aArray)
{
   return UtoPosition(aArray[0], aArray[1], aArray[2]);
}


struct UTILOSG_EXPORT UtoOrientation
{
   double yaw;   // the psi orientation component.
   double pitch; // the theta orientation component.
   double roll;  // the phi orientation component.

   UtoOrientation();
   UtoOrientation(double y, double p, double r);
};

struct UTILOSG_EXPORT UtoPosOrient
{
   UtoPosition    pos;
   UtoOrientation orient;
};

struct UTILOSG_EXPORT UtoPoint
{
   double x; // the horizontal position component.
   double y; // the vertical position component.

   UtoPoint();
   UtoPoint(double a, double b);

   UtoPoint& operator=(const UtoPosition& pos);
};

struct UTILOSG_EXPORT UtoSize
{
   double cx; // the width component.
   double cy; // the height component.

   static const UtoSize null;

   UtoSize();
   UtoSize(double width, double height);
};

struct UTILOSG_EXPORT UtoRect
{
   double top;    // the top edge of the rectangle.
   double bottom; // the bottom edge of the rectangle.
   double left;   // the left edge of the rectangle.
   double right;  // the right edge of the rectangle.

   static const UtoRect null;

   UtoRect();
   UtoRect(double top, double bottom, double left, double right);
   UtoRect(const UtoPoint& ptTopLeft, const UtoPoint& ptBottomRight);
   UtoRect(const UtoPoint& ptTopLeft, const UtoSize& size);

   void TopLeft(const UtoPosition& pos);
   void BottomRight(const UtoPosition& pos);
};

// UtoBox is currently unused in AFSIM
struct UTILOSG_EXPORT UtoBox
{
   // range in x direction
   float xmin;
   float xmax;
   // range in y direction
   float ymin;
   float ymax;
   // range in z direction
   float zmin;
   float zmax;

   UtoBox();
   UtoBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
};

class UtoColor
{
public:
   UtoColor(unsigned char aR, unsigned char aG, unsigned char aB, unsigned char aA = 255)
   {
      mRGBA[0] = aR;
      mRGBA[1] = aG;
      mRGBA[2] = aB;
      mRGBA[3] = aA;
   }
   UtoColor() { mRGBA[0] = mRGBA[1] = mRGBA[2] = mRGBA[3] = 200; }

   void Set(const UtoColor aFrom, unsigned int aMask)
   {
      // set the specified components from the color masks
      if (aMask & RED)
         mRGBA[0] = aFrom.Red();
      if (aMask & GREEN)
         mRGBA[1] = aFrom.Green();
      if (aMask & BLUE)
         mRGBA[2] = aFrom.Blue();
      if (aMask & ALPHA)
         mRGBA[3] = aFrom.Alpha();
   }
   enum UtoColorComponent
   {
      RED   = 0x01,
      GREEN = 0x02,
      BLUE  = 0x04,
      ALPHA = 0x08,
      RGB   = RED | GREEN | BLUE,
      RGBA  = RED | GREEN | BLUE | ALPHA
   };
   unsigned char Red() const { return mRGBA[0]; }
   unsigned char Green() const { return mRGBA[1]; }
   unsigned char Blue() const { return mRGBA[2]; }
   unsigned char Alpha() const { return mRGBA[3]; }
   float         RedF() const { return mRGBA[0] / 255.0f; }
   float         GreenF() const { return mRGBA[1] / 255.0f; }
   float         BlueF() const { return mRGBA[2] / 255.0f; }
   float         AlphaF() const { return mRGBA[3] / 255.0f; }
   bool          HasTransparency() const { return mRGBA[3] < 255; }
   void          SetRed(unsigned char aR) { mRGBA[0] = aR; }
   void          SetGreen(unsigned char aG) { mRGBA[1] = aG; }
   void          SetBlue(unsigned char aB) { mRGBA[2] = aB; }
   void          SetAlpha(unsigned char aA) { mRGBA[3] = aA; }

private:
   unsigned char mRGBA[4];
};


namespace Utok
{

class no_resource : public std::exception
{
public:
   no_resource(const std::string& name) noexcept
      : m_Name(name)
   {
   }
   ~no_resource() override = default;
   const char* what() const noexcept override { return m_Name.c_str(); }

private:
   std::string m_Name;
};

struct NameShape
{
   //{secret}
   NameShape(const std::string& n, UtoShape& s)
      : name(n)
      , shape(s)
   {
   }

   const std::string& name;  // the name component.
   UtoShape&          shape; // the shape component.
};

/*struct NameOverlay
{
   NameOverlay (std::string& n, UtoOverlay& o) : name (n), overlay (o) {}

   std::string& name;
   UtoOverlay& overlay;
};*/

struct NameOverlay
{
   //{secret}
   NameOverlay()
      : name(std::string())
      , overlay(nullptr)
   {
   }
   //{secret}
   NameOverlay(const std::string& n, UtoOverlay* const p)
      : name(n)
      , overlay(p)
   {
   }

   std::string name;    // the name component.
   UtoOverlay* overlay; // the overlay component.
};

struct TraitValue
{
   //{secret}
   TraitValue(const std::string& n, const std::string& v)
      : name(n)
      , value(v)
   {
   }

   const std::string& name;  // the name component.
   const std::string& value; // the value component.
};

//{secret}
struct DrawFunctor
{
   virtual ~DrawFunctor()             = default;
   virtual DrawFunctor* clone() const = 0;
   virtual void         operator()()  = 0;
};

//{secret}
template<class T, class Func>
struct DrawFunctorHolder : public DrawFunctor
{
   DrawFunctorHolder(T* p, Func f)
      : m_this(p)
      , m_draw(f)
   {
   }
   ~DrawFunctorHolder() override = default;

   DrawFunctor* clone() const override { return new DrawFunctorHolder<T, Func>(m_this, m_draw); }

   void operator()() override { m_draw(m_this); }

   T*   m_this;
   Func m_draw;
};

//{secret}
template<class I>
struct IteratorPair
{
   I begin;
   I end;

   IteratorPair() = default;
   IteratorPair(const I& b, const I& e)
      : begin(b)
      , end(e)
   {
   }
};

//{secret}
template<class Policies>
class Iterator : public Policies
{
public:
   typedef Iterator<Policies> my_type;
   typedef Policies           base_type;

   using iterator_category = std::forward_iterator_tag;
   using difference_type   = int;
   using value_type        = typename Policies::value_type;
   using pointer           = value_type*;
   using reference         = value_type&;

   using Policies::forward;
   using Policies::ptr;

   Iterator() {}
   Iterator(pointer p)
      : base_type(p)
   {
   }
   // Iterator(const my_type& rhs) : base_type (rhs) {}
   ~Iterator() = default;

   template<class U>
   Iterator(const U& rhs)
      : base_type(rhs)
   {
   }

   // reference operator& () const {return ptr ();}
   reference operator*() { return *ptr(); }
   pointer   operator->() { return ptr(); }
   my_type&  operator=(const my_type& rhs)
   {
      if (this == &rhs)
         return *this;

      my_type T(rhs);
      this->swap(T);
      return *this;
   }

   my_type operator++()
   {
      forward();
      return (*this);
   }

   my_type operator++(int)
   {
      my_type tmp = *this;
      ++*this;
      return tmp;
   }

   bool operator==(const my_type& rhs) const { return this->is_equal(rhs); }

   bool operator!=(const my_type& rhs) const { return !this->is_equal(rhs); }
};

} // namespace Utok

// No operator<< for UtoPosition as it's equivalent to osg::Vec3d; use <osg/io_utils> version.

inline std::ostream& operator<<(std::ostream& s, const UtoOrientation& orient)
{
   return s << "(r,p,y):" << orient.roll << ", " << orient.pitch << ", " << orient.yaw;
}

inline std::ostream& operator<<(std::ostream& s, const UtoPoint& pt)
{
   return s << "(x,y):" << pt.x << ", " << pt.y;
}

inline std::ostream& operator<<(std::ostream& s, const UtoRect& rect)
{
   return s << "(t,l,b,r):" << rect.top << ", " << rect.left << ", " << rect.bottom << ", " << rect.right;
}

inline std::ostream& operator<<(std::ostream& s, const UtoSize& sz)
{
   return s << "(cx,cy):" << sz.cx << ", " << sz.cy;
}

inline std::ostream& operator<<(std::ostream& s, const UtoColor& color)
{
   return s << "(r,g,b,a):" << color.Red() << ", " << color.Green() << ", " << color.Blue() << ", " << color.Alpha();
}

inline std::istream& operator>>(std::istream& s, UtoPosition& pos)
{
   std::string str;
   char        c;
   s >> std::setw(8) >> str >> pos.x() >> c >> pos.y() >> c >> pos.z();
   return s;
}

inline std::istream& operator>>(std::istream& s, UtoOrientation& orient)
{
   std::string str;
   char        c;
   s >> std::setw(8) >> str >> orient.roll >> c >> orient.pitch >> c >> orient.yaw;
   return s;
}

inline std::istream& operator>>(std::istream& s, UtoPoint& pt)
{
   std::string str;
   char        c;
   s >> std::setw(6) >> str >> pt.x >> c >> pt.y;
   return s;
}

inline std::istream& operator>>(std::istream& s, UtoRect& rect)
{
   std::string str;
   char        c;
   s >> std::setw(10) >> str >> rect.top >> c >> rect.left >> c >> rect.bottom >> c >> rect.right;
   return s;
}

inline std::istream& operator>>(std::istream& s, UtoSize& sz)
{
   std::string str;
   char        c;
   s >> std::setw(8) >> str >> sz.cx >> c >> sz.cy;
   return s;
}

inline bool operator==(const UtoOrientation& rhs, const UtoOrientation& lhs)
{
   return (rhs.pitch == lhs.pitch && rhs.roll == lhs.roll && rhs.yaw == lhs.yaw);
}

inline bool operator==(const UtoPoint& rhs, const UtoPoint& lhs)
{
   return (rhs.x == lhs.x && rhs.y == lhs.y);
}

inline bool operator==(const UtoRect& rhs, const UtoRect& lhs)
{
   return (rhs.top == lhs.top && rhs.bottom == lhs.bottom && rhs.left == lhs.left && rhs.right == rhs.right);
}

inline bool operator==(const UtoSize& rhs, const UtoSize& lhs)
{
   return (rhs.cx == lhs.cx && rhs.cy == lhs.cy);
}

inline bool operator==(const UtoColor& rhs, const UtoColor& lhs)
{
   return (rhs.Red() == lhs.Red() && rhs.Green() == lhs.Green() && rhs.Blue() == lhs.Blue() && rhs.Alpha() == lhs.Alpha());
}

inline bool operator!=(const UtoOrientation& rhs, const UtoOrientation& lhs)
{
   return !(rhs == lhs);
}

inline bool operator!=(const UtoPoint& rhs, const UtoPoint& lhs)
{
   return !(rhs == lhs);
}

inline bool operator!=(const UtoRect& rhs, const UtoRect& lhs)
{
   return !(rhs == lhs);
}

inline bool operator!=(const UtoSize& rhs, const UtoSize& lhs)
{
   return !(rhs == lhs);
}

inline bool operator!=(const UtoColor& rhs, const UtoColor& lhs)
{
   return !(rhs == lhs);
}

#endif
