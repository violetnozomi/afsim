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
// UtoModelSwitch.cpp : implementation file

#include "UtoModelSwitch.hpp"

/**
\ingroup shape
\class UtoModelSwitch
\brief Give the ability to select switch positions for a model.

The model switch is used for selecting switch positions in a model.
This class is not created by the user, but is provided by the
model shape (UtoModelShape).
*/
class UtoModelSwitch;

UtoModelSwitch::UtoModelSwitch() {}

/** @name Constructor & Destructors **/
//@{
/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoModelSwitch::~UtoModelSwitch() {}
//@}

/** @name Operations */
//@{
/**
\fn void UtoModelSwitch::SetSelected( int value )
\brief Sets the switch position.

Sets the switch position to the value indicated. If the
value is negative nothing controlled by that switch will
be displayed.
\param value the switch position to set.
*/
//@}

/** @name Accessors */
//@{
/**
\fn int UtoModelSwitch::Count( ) const
\brief Returns the number of switch positions.

Returns the number of switch positions.
\return int - the number of switch positions.
*/

/**
\fn std::string UtoModelSwitch::SelectedName( ) const
\brief Returns the name of the currently selected switch position.

Returns the name of the currently selected switch position.
An empty string is returned if the current switch position is invalid.
\return std::string - current switch position name.
*/

/**
\fn int UtoModelSwitch::Selected( ) const
\brief Returns the currently selected switch position.

Returns the currently selected switch position.
\return int - currently selected switch position.
*/
//@}
