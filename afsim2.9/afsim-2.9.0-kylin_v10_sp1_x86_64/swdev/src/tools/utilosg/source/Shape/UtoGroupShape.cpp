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

/////////////////////////////////////////////////////////////////////////////
// UtoGroupShape.cpp : implementation file

#include "UtoGroupShape.hpp"

#include "UtoGroupShapeImp.hpp"
#include "UtoSmallObj.hpp"

/**
\ingroup shape
\class UtoGroupShape
\brief Combines and organizes a collection of shapes.

The group shape is used to combine and organize a collection of shapes
under a single parent shape.  Therefore, when the group shape is copied or
cloned, all contained shapes are copied or cloned as well.

The group shape provides Bind, Rebind, Unbind, and Lookup methods for
managing the list of shapes.  A Shapes method is provide to access the
begin and end iterators for custom traversal and operations on the
contained shapes.  The EnableRecursiveProperties method provides a way to
have the group color and transparency properties applied to all contained
shapes.  However, EnableRecursiveProperties is currently not implemented.

The order in which shapes are bound will determine the order of render.
This means that if you bind a text shape then a polygon shape the polygon
will overlap the text (unless offsets are being used).  Rebind effectively
removes the previous shape and adds the new shape at the end of the render
order.  To prevent order of draw problems, care must be taken in the order
which shapes are bound to the group.
*/
class UtoGroupShape;

//{secret}
inline UtoGroupShapeImp* GetImpl(UtoGroupShape* shape)
{
   return static_cast<UtoGroupShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoGroupShapeImp* GetImpl(const UtoGroupShape* shape)
{
   return static_cast<const UtoGroupShapeImp*>(shape->Impl());
}

// {secret}
struct UtoGroupShape::Policies::Impl : public Loki::SmallObject<>
{
   UtoShapeMap::iterator m_Iter;
   char                  m_Memory[sizeof(Utok::NameShape)];
   Utok::NameShape*      m_pNameShape;

   Impl()
      : /* m_Iter (0), PMG CANNOT INIT AN ITERATOR TO NULL*/ m_pNameShape(nullptr)
   {
   }
   Impl(UtoShapeMap::iterator i)
      : m_Iter(i)
      , m_pNameShape(nullptr)
   {
   }
   Impl(const UtoGroupShape::Policies::Impl& rhs)
      : m_Iter(rhs.m_Iter)
      , m_pNameShape(nullptr)
   {
      if (rhs.m_pNameShape)
      {
         m_pNameShape = new (m_Memory) Utok::NameShape(m_Iter->first, *m_Iter->second);
      }
   }
};


/////////////////////////////////////////////////////////////////////////////

UtoGroupShape::Policies::Policies()
   : m_pImpl(new Impl)
{
}

UtoGroupShape::Policies::Policies(Impl* imp)
   : m_pImpl(imp)
{
}

UtoGroupShape::Policies::Policies(const UtoGroupShape::Policies& rhs)
   : m_pImpl(new Impl(*rhs.m_pImpl))
{
}

UtoGroupShape::Policies::~Policies()
{
   delete m_pImpl;
}

UtoGroupShape::Policies::value_type* UtoGroupShape::Policies::ptr()
{
   if (m_pImpl->m_pNameShape == nullptr)
   {
      // tricky, sneaky in-place memory allocation.  don't try this at home.
      m_pImpl->m_pNameShape = new (m_pImpl->m_Memory) Utok::NameShape(m_pImpl->m_Iter->first, *m_pImpl->m_Iter->second);
   }
   return m_pImpl->m_pNameShape;
}

void UtoGroupShape::Policies::forward()
{
   ++m_pImpl->m_Iter;
   m_pImpl->m_pNameShape = nullptr;
}

bool UtoGroupShape::Policies::is_equal(const UtoGroupShape::Policies& rhs) const
{
   return (m_pImpl->m_Iter == rhs.m_pImpl->m_Iter);
}

void UtoGroupShape::Policies::swap(UtoGroupShape::Policies& rhs)
{
   Impl* temp  = rhs.m_pImpl;
   rhs.m_pImpl = m_pImpl;
   m_pImpl     = temp;
}

/////////////////////////////////////////////////////////////////////////////

/** @name Constructor & Destructor */
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoGroupShape::UtoGroupShape()
   : UtoShape(new UtoGroupShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoGroupShape::UtoGroupShape(const UtoGroupShape& rhs)
   : UtoShape(new UtoGroupShapeImp(*GetImpl(&rhs)))
{
}

UtoGroupShape::UtoGroupShape(UtoGroupShapeImp* impl)
   : UtoShape(impl)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoGroupShape::~UtoGroupShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoGroupShape & - a reference to the current object.
*/
UtoGroupShape& UtoGroupShape::operator=(const UtoGroupShape& rhs)
{
   UtoGroupShape T(rhs);
   Swap(T);
   return *this;
}
//@}

// {secret}
void UtoGroupShape::Dump(std::ostream& o)
{
   o << "  Type: UtoGroupShape" << std::endl;
   UtoShape::Dump(o);
}

/** @name Accessors */
//@{

/**
\brief Looks for a shape bound to the specified name.

Looks for a shape bound to the specified name.
\param name the name of the bound shape.
\return UtoGroup::iterator - An iterator that reference the bound shape.
*/

UtoShape* UtoGroupShape::Lookup(const std::string& name)
{
   return GetImpl(this)->FindShape(name);
}

/**
\brief Returns all the shapes bound to the group.

Returns all the shapes bound to the group.
\returns UtoGroupShape::iterator_pair - begin and end iterators to
all the shapes bound to the group.
*/
const UtoShapeMap& UtoGroupShape::Shapes()
{
   return GetImpl(this)->Shapes();
}

/**
\brief Recursive property attribute.

Returns wheter the recursive properties for the group are enabled.

\note Not implemented.
\return bool - <table>
                  <tr><td>true</td><td>The recursive properties are enabled.</td></tr>
                  <tr><td>false</td><td>The recursive properties are disabled.</td></tr>
               </table>
*/
bool UtoGroupShape::IsRecursivePropertiesEnabled() const
{
   return GetImpl(this)->IsRecursivePropertiesEnabled();
}
//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoGroupShape::Clone() const
{
   return new UtoGroupShape(*this);
}


/**
\brief Binds a shape to the specified name.

Binds a shape to the specified name.  This method makes a copy of the
shape parameter and binds it to the specified name.
\param name the name the shape will be bound with.
\param shape the shape to be copied and bound to name.
\return UtoGroupShape::iterator - a reference to the bound shape.
*/
// pushes new onto the compiler stack and
// undefines the new operator
UtoShape* UtoGroupShape::Bind(const std::string& name, const UtoShape& shape)
{
   return GetImpl(this)->Bind(name, shape);
}

/**
\brief Removes a shape binding.

Removes a shape binding and destroys the shape if successful.
\param name the shape's bound name.
\return bool - <table>
                  <tr><td>true</td><td>The shape was unbound.</td></tr>
                  <tr><td>false</td><td>The shape was not unbound.</td></tr>
               </table>
*/
bool UtoGroupShape::Unbind(const std::string& name)
{
   return GetImpl(this)->UnbindShape(name);
}

/**
\brief Rebinds a shape to a name.

Rebind a shape to the specified name.  The previous shape is destroyed and
replaces it with a copy of the new shape.  Only iterators to the replaced
shape are invalid.
\param name the name the shape will be rebound with.
\param shape the shape to be copied and rebound to name.
\return bool - <table>
                  <tr><td>true</td><td>The shape was rebound.</td></tr>
                  <tr><td>false</td><td>The shape was not rebound.</td></tr>
               </table>
*/
bool UtoGroupShape::Rebind(const std::string& name, const UtoShape& shape)
{
   GetImpl(this)->UnbindShape(name);
   return GetImpl(this)->Bind(name, shape) != nullptr;
}

/**
\brief Enables / disables recursive properties.

Enables or disables the recursive properties for the group.

\note Not implemented.
\param enable a flag used to enable or disable recursive properties.
\return bool - value is ignored.
*/
bool UtoGroupShape::EnableRecursiveProperties(bool enable)
{
   return GetImpl(this)->EnableRecursiveProperties(enable);
}

UtoShape* UtoGroupShape::FindShape(const std::string& name) const
{
   return GetImpl(this)->FindShape(name);
}

//@}

/** @name Type Definitions */
//@{
/**
\typedef UtoGroupShape::Utok::Iterator<Policies> UtoGroupShape::iterator
\brief Provides access to a NameSpace.

Provides access to a NameSpace.
*/
/**
\typedef UtoGroupShape::Utok::IteratorPair<iterator> UtoGroupShape::iterator_pair
\brief Provides access to the begin and end iterators.

Provides access to the begin and end iterators.
*/
//@}
