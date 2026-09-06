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
// UtoModelShape.cpp : implementation file

#include "UtoModelShape.hpp"

#include "UtoModelPartImp.hpp"
#include "UtoModelShapeImp.hpp"
#include "UtoModelSwitchImp.hpp"

/**
\ingroup shape
\class UtoModelShape
\brief Draws 3D geometry from a model file.

The model shape is used to display 3D geometry specified from a model file.
This shape is not constructed by the user, but is constructed by the resource
database (UtoResourceDB) when model resources are loaded from a resource
definition file.

The model shape provides access to the pre-defined or user specified metrics in
the resource definition file through the Metric method.  For a list of the
pre-defined metrics see the resource definition file specification.  User
defined metrics may be used to access other meta data.  This allows the user to
associate values with strings such as the pilot eye location.  Then this data
can be called up for use when placing the perspective camera in a scene The
InGroup method can be used to test the model group metric information.

The model shape provides access to articulated parts through the HasPart and
FindPart methods.  The HasPart method can be used to verify the existence of a
named part, and FindPart is used to get access to the named part.  The
articulated part geometry and names are determined by the model file.
*/
class UtoModelShape;

//{secret}
inline UtoModelShapeImp* GetImpl(UtoModelShape* shape)
{
   return static_cast<UtoModelShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoModelShapeImp* GetImpl(const UtoModelShape* shape)
{
   return static_cast<const UtoModelShapeImp*>(shape->Impl());
}

UtoModelShape::UtoModelShape()
   : UtoShape(new UtoModelShapeImp)
{
}

UtoModelShape::UtoModelShape(UtoModelShapeImp* imp)
   : UtoShape(imp)
{
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoModelShape::UtoModelShape(const UtoModelShape& rhs)
   : UtoShape(new UtoModelShapeImp(*GetImpl(&rhs)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoModelShape::~UtoModelShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoModelShape & - a reference to the current object.
*/
UtoModelShape& UtoModelShape::operator=(const UtoModelShape& rhs)
{
   UtoModelShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

/**
\brief Sets the named metric value.

Sets the named metric value.
\param[in] name the name of the metric.
\param[in] value the value of the metric.
*/
void UtoModelShape::SetMetric(const std::string& name, double value)
{
   return GetImpl(this)->SetMetric(name, value);
}

/**
\brief Returns the named metric value.

Returns the named metric value.
\param[in] name the name of the metric.
\param[out] value the metric value.
\return bool - <table>
                  <tr><td>true</td><td>The metric exists.</td></tr>
                  <tr><td>false</td><td>The metric does not exist.</td></tr>
               </table>
*/
bool UtoModelShape::Metric(const std::string& name, double& value) const
{
   return GetImpl(this)->Metric(name, value);
}

/**
\brief Indicates the existence of the model in the group.

Indicates the existence of the model in the group.
\param group the group name to test against.
\return bool - <table>
                  <tr><td>true</td><td>The model exists in the group.</td></tr>
                  <tr><td>false</td><td>The model does not exists in the group.</td></tr>
               </table>
*/
bool UtoModelShape::InGroup(const std::string& group) const
{
   return GetImpl(this)->InGroup(group);
}
/**
\brief Returns the names of articulated parts in the model.

Provides the names of articulated parts in the model for further usage.
\return std::vector<std::string> - an array of the string name of parts
*/
std::vector<std::string> UtoModelShape::GetParts()
{
   return GetImpl(this)->GetParts();
}

/**
\brief Returns the names of articulated parts in the model.

Provides the names of articulated parts in the model for further usage.
\param A root node of an osg loaded model
\return std::vector<std::string> - an array of the string name of parts
*/
std::vector<std::string> UtoModelShape::GetParts(osg::ref_ptr<osg::Node> aModelNode)
{
   return UtoModelShapeImp::GetParts(aModelNode);
}

/**
\brief Indicates the existence of articulated parts in the model.

Indicates the existence of articulated parts in the model.
\param name the name of the articulated part.
\return bool - <table>
                  <tr><td>true</td><td>The articulated part exists in the model.</td></tr>
                  <tr><td>false</td><td>The articulated part does not exists in the model.</td></tr>
               </table>
*/
bool UtoModelShape::HasPart(const std::string& name)
{
   return GetImpl(this)->HasPart(name);
}

/**
\brief Returns an articulated part.

Returns an articulated part.  If the named part isn't found a
dummy part will be returned that will not affect the model.
\param name the of the articulated part.
\return UtoModelPart & - a reference to the articulated part.
*/
UtoModelPart& UtoModelShape::FindPart(const std::string& name)
{
   return GetImpl(this)->FindPart(name);
}

/**
\brief Indicates the existence of a model switch.

Indicates the existence of a model switch.
\param name the name of the model switch
\return bool - <table>
                  <tr><td>true</td><td>The switch exists.</td></tr>
                  <tr><td>false</td><td>The switch does not exist.</td></tr>
               </table>
*/
bool UtoModelShape::HasSwitch(const std::string& name)
{
   return GetImpl(this)->HasSwitch(name);
}

/**
\brief Returns the model switch.

Returns the model switch.  If the named switch isn't
found a dummy switch will be returned that will not affect the model.
\param name the name of the model switch.
\return UtoModelSwitch & - a reference to a model switch.
*/
UtoModelSwitch& UtoModelShape::FindSwitch(const std::string& name)
{
   return GetImpl(this)->FindSwitch(name);
}
//@}

/**
\brief Returns the names of switches (switch states) in the model.

Provides the names of switches in the model for further usage.
\return std::vector<std::string> - an array of the string name of switches
*/
std::vector<std::string> UtoModelShape::GetSwitches()
{
   return GetImpl(this)->GetSwitches();
}

/**
\brief Returns the names of switches (switch states) in the model.

Provides the names of switches in the model for further usage.
\param A root node of an osg loaded model
\return std::vector<std::string> - an array of the string name of switches
*/
std::vector<std::string> UtoModelShape::GetSwitches(osg::ref_ptr<osg::Node> aModelNode)
{
   return UtoModelShapeImp::GetSwitches(aModelNode);
}

/**
\brief Returns the number of switches (switch states) in the model.

Provides the number of switches in the model for further usage.
\param A root node of an osg loaded model
\return std::vector<int> - an array of the number of switches
*/
std::vector<int> UtoModelShape::GetSwitchSizes(osg::ref_ptr<osg::Node> aModelNode)
{
   return UtoModelShapeImp::GetSwitchSizes(aModelNode);
}

void UtoModelShape::SetDepthTesting(bool aState)
{
   GetImpl(this)->SetDepthTesting(aState);
}

void UtoModelShape::SetLighting(bool aState)
{
   GetImpl(this)->SetLighting(aState);
}

/**
\brief Changes behavior of copy constructor.

Changes behavior of copy constructor to deep copy selected items.  Default value is DEEP_COPY_OBJECTS | DEEP_COPY_NODES.
\param name copyFlags A combination of CopyOptions bit flags.
*/
void UtoModelShape::SetCopyOptions(int copyFlags)
{
   GetImpl(this)->SetCopyOptions(copyFlags);
}

/** @name Operations */
//@{
/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoModelShape::Clone() const
{
   UtoModelShape* shape = nullptr;
   /*if (Impl ()->HasFlag (UtoShapeImp::SHARED))
   {
      shape = new UtoModelShape;
      shape->SetImpl (const_cast<UtoShapeImp*>(Impl ()));
   }
   else*/
   shape = new UtoModelShape(*this);
   return shape;
}
//@}

// {secret}
void UtoModelShape::Dump(std::ostream& o)
{
   o << "  Type: UtoModelShape" << std::endl;
   UtoShape::Dump(o);
}
