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
// UtoModelPart.cpp : implementation file

#include "UtoModelPart.hpp"

/**
\ingroup shape
\class UtoModelPart
\brief Draws a model part.

The model part is used for model articulation.  This class is not created by
the user, but is provided by the model shape (UtoModelShape).  The model
part provides access to the position and orientation aspects of the part.
*/
class UtoModelPart;

UtoModelPart::UtoModelPart() {}

/** @name Constructor & Destructor */
//@{
/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoModelPart::~UtoModelPart() {}
//@}

/** @name Operations */
//@{
/**
\fn void UtoModelPart::SetPosition( const UtoPosition & pos )
\brief Sets the model part position.

Sets the model part position.  The position is absolute.
\param pos the new absolute position.
*/

/**
\fn void UtoModelPart::SetOrientation( const UtoOrientation & orient )
\brief Sets the model part orientation.

Sets the model part orientation.  The orientation is absolute.
\param orient the new absolute orientation.
*/

/**
\fn void UtoModelPart::SetPosOrient( const UtoPosOrient & posorient )
\brief Sets the model part position and orientation.

Sets the model part position and orientation.  The position and orientation are absolute.
\param posorient the new absolute position and orientation..
*/
//@}

/** @name Accessors */
//@{
/**
\fn UtoPosition UtoModelPart::Position( ) const
\brief Returns the model part position.

Returns the model part position.  The position is absolute.
\return UtoPosition - an absolute model part position.
*/

/**
\fn UtoOrientation UtoModelPart::Orientation( ) const
\brief Returns the model part orientation.

Returns the model part orientation.  The orientation is absolute.
\return UtoOrientation - an absolute model part orientation.
*/

/**
\fn UtoPosOrient UtoModelPart::PosOrient( ) const
\brief Returns the model part position and orientation.

Returns the model part position and orientation.  The position and
orientation are absolute.
\return UtoPosOrient - an absolute model part position and orientation.
*/
//@}
