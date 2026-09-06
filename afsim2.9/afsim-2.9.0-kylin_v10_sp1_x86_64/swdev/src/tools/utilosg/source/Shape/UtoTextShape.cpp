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
// UtoTextShape.cpp : implementation file

#include "UtoTextShape.hpp"

#include "UtoTextShapeImp.hpp"

/**
\ingroup shape
\class UtoTextShape
\brief Used to draw text.

The text shape is used to show text in the scene.  The text shape
uses True Type fonts installed on the system.  Different sizes,
bold, italic, and alignments are possible.

The shape may be constructed with the display text.  The display text may
be changed at any time using the SetString method.  The font may be changed
using the SetFont method.  The alignment may be set using the SetAlignment
method.  Alignment options include top, bottom, left, right, horizontal
centering and vertical centering.
*/
class UtoTextShape;

// Define the public enumerations for the documentation
/** @name Type Definitions */
//@{
/*! \enum UtoTextShape::AlignFlags \brief Text Alignment

    Defines specific alignment actions for text. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::LEFT

    Text will be left aligned. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::CENTER

    Text will be centered. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::RIGHT

    Text will be right aligned. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::TOP

    Text will be vertically aligned from the top. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::VCENTER

    Text will be vertically aligned from the center. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::BOTTOM

    Text will be vertically aligned from the bottom. */
/*! \var UtoTextShape::AlignFlags UtoTextShape::DEFAULT

    Text will be alligned with BOTTOM and LEFT alignment. */


/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_BOTTOM_RIGHT

    Backdrop shadow will be rendered to the bottom right of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_CENTER_RIGHT

    Backdrop shadow will be rendered to the center right of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_TOP_RIGHT

    Backdrop shadow will be rendered to the top right of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_BOTTOM_CENTER

    Text will be alligned with BOTTOM and LEFT alignment. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_TOP_CENTER

    Backdrop shadow will be rendered to the top center of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_BOTTOM_LEFT

    Backdrop shadow will be rendered to the bottom left of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_CENTER_LEFT

    Backdrop shadow will be rendered to the center left of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_TOP_LEFT

    Backdrop shadow will be rendered to the top left of the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_OUTLINE

    Backdrop shadow will be rendered as an outline around the text. */
/*! \var UtoTextShape::Backdrop UtoTextShape::BACKDROP_NONE

    Backdrop shadows will be disabled. */


/*! \var UtoTextShape::BackdropImp UtoTextShape::BACKDROP_IMP_POLYGON_OFFSET

    Backdrop shadows will be rendered using polygon offsets. */
/*! \var UtoTextShape::BackdropImp UtoTextShape::BACKDROP_IMP_NO_DEPTH_BUFFER

    Backdrop shadows will be rendered by disabling the depth buffer. */
/*! \var UtoTextShape::BackdropImp UtoTextShape::BACKDROP_IMP_DEPTH_RANGE

    Backdrop shadows will be rendered using the depth range. */
/*! \var UtoTextShape::BackdropImp UtoTextShape::BACKDROP_IMP_STENCIL_BUFFER

    Backdrop shadows will be rendered using the stencil buffer. */


/*! \struct UtoTextShape::Border \brief Text Shape Border Attributes

    Defines the border attributes that the shape can use. */

/*! \var UtoColor UtoTextShape::Border::oBorderClr

    Defines the type of color to be used for the border. */

/*! \var UtoColor UtoTextShape::Border::oBackgroundClr

    Defines the type of color to be used when filling the border. */

/*! \var unsigned int UtoTextShape::Border::nBorderWidth

    Defines the border line width. */

/*! \var short UtoTextShape::Border::nX0

    Units: Pixels<br>
    Static - Defines the starting location relative to the pivot point defined by the alignment flag.<br>
    Dynamic - Defines the padding that will be applied to the border to increase or decrease the overall size. */

/*! \var short UtoTextShape::Border::nY0

    Units: Pixels<br>
    Static - Defines the starting location relative to the pivot point defined by the alignment flag.<br>
    Dynamic - Defines the padding that will be applied to the border to increase or decrease the overall size. */

/*! \var unsigned short UtoTextShape::Border::nWidth

    Units: Pixels<br>
    Static - Defines the width of the border relative to nX0.<br>
    Dynamic - Not Defined. */

/*! \var unsigned short UtoTextShape::Border::nHeight

    Units: Pixels<br>
    Static - Defines the height of the border relative to nY0.<br>
    Dynamic - Not Defined. */

/*! \var bool UtoTextShape::Border::bDrawDynamic

    Determines if the border is dynamic or static.  Unlike dynamic borders, static borders do not change size when the
   text changes.  Setting this flag to true will draw a dynamic border while setting this flag to false indicates a
   static border. */

/*! \fn UtoTextShape::Border::Border( )

    Default constructor. */

/*! \fn UtoTextShape::Border::Border( UtoColor oBorderClr, UtoColor oBackgroundClr, unsigned int nBorderWidth,
                                      short nX0, short nY0, unsigned short nWidth, unsigned short nHeight, bool
   bDrawDynamic )

    Constructor.
    \param oBorderClr Defines the border color
    \param oBackgroundClr Defines the fill color for the border
    \param nBorderWidth Defines the border line width in pixels
    \param nX0 Defines the relative start location for a static border or padding for dynamic
    \param nY0 Defines the relative start location for a static border or padding for dynamic
    \param nWidth Defines the width of the border relative to nX0 for static borders
    \param nHeight Defines the height of the border relative to nY0 for static borders
    \param bDrawDynamic Defines the type of border being used */

//@}

//{secret}
inline UtoTextShapeImp* GetImpl(UtoTextShape* shape)
{
   return static_cast<UtoTextShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoTextShapeImp* GetImpl(const UtoTextShape* shape)
{
   return static_cast<const UtoTextShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructor */
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoTextShape::UtoTextShape()
   : UtoShape(new UtoTextShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param string initializes the text to the specified string.
*/
UtoTextShape::UtoTextShape(const UtoWString& string)
   : UtoShape(new UtoTextShapeImp(string))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param string initializes the text to the specified string.
*/
UtoTextShape::UtoTextShape(const std::string& string)
   : UtoShape(new UtoTextShapeImp(UtoWString(string.begin(), string.end())))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoTextShape::UtoTextShape(const UtoTextShape& rhs)
   : UtoShape(new UtoTextShapeImp(*GetImpl(&rhs)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoTextShape::~UtoTextShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoHistoryLineShape & - a reference to the current object.
*/
UtoTextShape& UtoTextShape::operator=(const UtoTextShape& rhs)
{
   UtoTextShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

/**
\brief Returns the display string.

Returns the display string.
\return std::string - the currently displayed string.
*/
UtoWString UtoTextShape::String() const
{
   return GetImpl(this)->String();
}

void UtoTextShape::SetShader(osg::Program* aShader)
{
   return GetImpl(this)->SetShader(aShader);
}

/**
\brief Returns the font point size.

Returns the font point size.
\return int the font point size currently in use.
*/
int UtoTextShape::Size() const
{
   return GetImpl(this)->Size();
}

/**
\brief Returns the text font face name.

Returns the text font face name.
\return std::string - the font face currently in use.
*/
std::string UtoTextShape::Face() const
{
   return GetImpl(this)->Face();
}

/**
\brief Returns the current alignment flags.

Returns the current alignment flags.
\return unsigned long - the current alignment bits.
*/
unsigned long UtoTextShape::Alignment() const
{
   return GetImpl(this)->Alignment();
}

void UtoTextShape::GetBounds(float& aWidth, float& aHeight)
{
   return GetImpl(this)->GetBounds(aWidth, aHeight);
}

/**
\brief Indicates the type of backdrop shadow text being used.

Indicates the type of backdrop shadow text being used.
\return UtoTextShape::Backdrop - The current backdrop being used.
*/
UtoTextShape::Backdrop UtoTextShape::BackdropType() const
{
   return (Backdrop)GetImpl(this)->BackdropType();
}

/**
\brief Indicates the amount of horizontal offset to create the backdrop effect.

Indicates the amount of horizontal offset to create the backdrop effect.
\return float - The current horizontal offset.
*/
float UtoTextShape::BackdropHorizontalOffset() const
{
   return GetImpl(this)->BackdropHorizontalOffset();
}

/**
\brief Indicates the amount of vertical offset to create the backdrop effect.

Indicates the amount of vertical offset to create the backdrop effect.
\return float - The current vertical offset.
*/
float UtoTextShape::BackdropVerticalOffset() const
{
   return GetImpl(this)->BackdropVerticalOffset();
}

/**
\brief Returns the current backdrop color.

Returns the current backdrop color.
\return UtoColor - The current backdrop color.
*/
const UtoColor& UtoTextShape::BackdropColor() const
{
   return GetImpl(this)->BackdropColor();
}

/**
\brief Specifies the underlying backdrop rendering implementation.

Specifies the underlying backdrop rendering implementation.
\return UtoTextShape::BackdropImp - The current backdrop rendering implemenation.
*/
UtoTextShape::BackdropImp UtoTextShape::BackdropRenderImp() const
{
   return (BackdropImp)GetImpl(this)->BackdropRenderImp();
}
//@}

// {secret}
void UtoTextShape::Dump(std::ostream& o)
{
   o << "  Type: UtoTextShape" << std::endl;
   UtoShape::Dump(o);
}

/** @name Operations */
//@{
/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoTextShape::Clone() const
{
   return new UtoTextShape(*this);
}

/**
\brief Sets the string that will be displayed.

Sets the string that will be displayed
\param string the new string to display.
\return bool - <table>
                  <tr><td>true</td><td>The string was set.</td></tr>
                  <tr><td>false</td><td>The string was not set.</td></tr>
               </table>
*/
bool UtoTextShape::SetString(const UtoWString& string)
{
   return GetImpl(this)->SetString(string);
}

/**
\brief Sets the string that will be displayed.

Sets the string that will be displayed
\param string the new string to display.
\return bool - <table>
                  <tr><td>true</td><td>The string was set.</td></tr>
                  <tr><td>false</td><td>The string was not set.</td></tr>
               </table>
*/
bool UtoTextShape::SetString(const std::string& sString)
{
   return SetString(UtoWString(sString.begin(), sString.end()));
}

/**
\brief Sets the point size of the text font.

Sets the point size of the text font.
\param size the new point size to be used.
\return bool - <table>
                  <tr><td>true</td><td>The size was set.</td></tr>
                  <tr><td>false</td><td>The size was not set.</td></tr>
               </table>
*/
bool UtoTextShape::SetSize(int size)
{
   return GetImpl(this)->SetSize(size);
}

/**
\brief Sets the style, bold, and italic design for the font face.

Sets the style, bold, and italic design for the font face
\param style the new font style name.
\param bold a flag to set the font style to bold.
\param italic a flag to set the font style to italic.
\return bool - <table>
                  <tr><td>true</td><td>The font face was set.</td></tr>
                  <tr><td>false</td><td>The font face was not set.</td></tr>
               </table>
*/
bool UtoTextShape::SetFont(const std::string& style)
{
   return GetImpl(this)->SetFont(style);
}

/**
\brief Sets the text alignment.

Sets the text alignment.

Flags specify the horizontal and vertical alignment of the text relative to the shape's position.
Horizontal alignment may be LEFT, CENTER, or RIGHT. The default is LEFT.
Vertical alignment may be TOP, VCENTER, or BOTTOM. The default is BOTTOM.
Flags specified in this call do not modify current alignment settings, they supercede them.
\param flags the new alignment values.
*/
void UtoTextShape::SetAlignment(unsigned long flags)
{
   GetImpl(this)->SetAlignment(flags);
}

/**
\brief Renders a backdrop shadow text behind the regular text.

Renders a backdrop shadow text behind the regular text.  This helps
give text extra contrast which can be useful when placing text against
noisy backgrounds.  BACKDROP_BOTTOM_RIGHT will draw backdrop text to
the right and down of the normal text.  Other BACKDROP_* modes do the
same for their respective directions.  BACKDROP_OUTLINE will draw
backdrop text so that it appears that the text has an outline or border
around the normal text.  This mode is particularly useful against really
noisy backgrounds that may put text on top of things that have all types
of colors which you don't have control over.  The default type is
BACKDROP_NONE.
\param nType - The backdrop type to render.
*/
void UtoTextShape::SetBackdropType(Backdrop nType)
{
   GetImpl(this)->SetBackdropType(nType);
}

/**
\brief Sets the amount of text offset to create the backdrop effect.

Sets the amount of text offset to create the backdrop effect.  The values
represent percentages.  1.0 means 100%, so a value of 1.0 in
BACKDROP_LEFT_CENTER mode would cause each glyph to be echoed next ot itself.
The letter 'e' might look like 'ee'.  Good values tend to be in the 0.03 to
0.10 range.
\param fOffset - The amount of text offset.
*/
void UtoTextShape::SetBackdropOffset(float fOffset)
{
   GetImpl(this)->SetBackdropOffset(fOffset);
}

/**
\brief Sets the amount of text offset to creat the backdrop effect.

Sets the amount of text offset to creat the backdrop effect.  This function
allows for manual setting of the offsets.
\param fHorizontal - The amount of horizontal offset.
\param fVertical - The amount of vertical offset.
*/
void UtoTextShape::SetBackdropOffset(float fHorizontal, float fVertical)
{
   GetImpl(this)->SetBackdropOffset(fHorizontal, fVertical);
}

/**
\brief Specifies the backdrop color.

Specifies the backdrop color.  The default is black.
\param rColor - The backdrop color.
\param nClrMask - The color mask to be applied to the backdrop color.
*/
void UtoTextShape::SetBackdropColor(const UtoColor& rColor, unsigned int nClrMask)
{
   GetImpl(this)->SetBackdropColor(rColor, nClrMask);
}

/**
\brief Specifies the underlying backdrop rendering implementation.

Specifies the underlying backdrop rendering implementation.
Unfortunately, at this time, there is no "perfect" rendering solution so,
this function is provided to let you 'pick your poison'.  Each
implementation has its own trade-offs.<br>
<br>
<b>BACKDROP_IMP_POLYGON_OFFSET:</b><br>
This uses glPolygonOffset to draw the text multip times to create the
backdrop and outline effects.  glPolygonOffset is used to prevent z-fighting
of the overlapping text.  This probably should have been the best option, but
all ATI cards have serious problems with this implementation.<br>
<b>BACKDROP_IMP_NO_DEPTH_BUFFER:</b><br><br>
Instead of using glPolygonOffset to prevent z-fighting, this mode just disables
the depth buffer when rendering the text.  This allows the text to be rendered
without any z-fighting.  The downside to this mode is that render order begins
to matter and the text will not correctly appear above or behind other objects
in the scene based on depth values.  This mode is best for text that only needs
to be ontop and not obscured by any objects.<br><br>
<b>BACKDROP_IMP_DEPTH_RANGE</b><br>
This mode uses glDepthRange as a substitue for glPolygonOffset.  Strangely,
experiments on ATI cards seem to produce cleaner results than when using
glPolygonOffset.  The trade-off for this is that the backdrop still may be
placed to far back and might be culled by objects directly behind the object
or by the far z-plane.  With current ATI problems, this option may be
preferable.<br><br>
<b>BACKDROP_IMP_STENCIL_BUFFER</b><br>
The stencil buffer is the most "correct" and reliable way of producing backdrop text.
the stencil buffer is a multipass system that allows writing to the same z-values without
needing to resort to offsets.  This implementation should not have any of the problems
associated with the 3 previous implementation, but the trade-off for this mode is that
without hardware acceleration for the stencil buffer, rendering will be extremely slow.
(There is also potentially more overhead for this algorithm, so it could be slower than
the other implementations.  Benchmarking would be required to determine if the speed
differences are significant on your particular hardware.)  This mode is best for
when quality is important and stencil buffer hardware acceleration is available.
\param nImpType - The type of rendering implementation to use.
*/
void UtoTextShape::SetBackdropRenderImp(BackdropImp nImpType)
{
   GetImpl(this)->SetBackdropRenderImp(nImpType);
}
//@}
