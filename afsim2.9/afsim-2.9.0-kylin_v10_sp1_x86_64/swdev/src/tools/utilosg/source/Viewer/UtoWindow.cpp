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
// UtoWindow.cpp : implementation file

#include "UtoWindow.hpp"

#include "UtoWindowImp.hpp"

/**
\ingroup view
\class UtoWindow
\brief Represents the basic window.

A window object is accessed from UtoViewer, and is never created directly by
the user.  A window is used to pass information about the windowing system into
Uto.

One of the first steps in getting Uto to display in a window is to call Attach
with the window handle.  Once this is done, calls to
UtoViewer::Draw can be made.
*/
class UtoWindow;

/////////////////////////////////////////////////////////////////////////////

UtoWindow::UtoWindow() {}

UtoWindow::~UtoWindow() {}

/** @name Type Definitions */
//@{

/*! \enum UtoWindow::Multisampling
 *  Multisampling enumeration
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_0X
 *  A basic window with no multisampling applied.
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_1X
 *  A basic window with one multisampling pass applied.
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_2X
 *  A basic window with two multisampling passes applied.
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_4X
 *  A basic window with four multisampling passes applied.
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_8X
 *  A basic window with eight multisampling passes applied.
 */
/*! \var UtoWindow::Multisampling UtoWindow::MULTISAMPLING_16X
 *  A basic window with 16 multisampling passes applied.
 */
//@}

/** @name Accessors */
//@{

/**
\fn UtoSize UtoWindow::Size () const
\brief Returns the window size in pixels.

Returns the window size in pixels.
\return UtoSize - the size of the window in pixels.
*/

/**
\fn int UtoWindow::PixelFormat () const
\brief Returns the window pixel format.

See the OpenGL documentation for more information on pixel formats.
The pixel format currently isn't used and isn't settable, the number
returned is undefined.
\return int - the OpenGL pixel format for the window.
*/

/**
\fn UtoWindowId UtoWindow::Window () const
\brief Returns the platform independent window ID.

Returns the platform independent window ID.
\return UtoWindowId - the window ID.
*/

//@}

/** @name Operations */
//@{

/**
\fn bool UtoWindow::SetPixelFormat (int fmt)
\brief Sets the window pixel format.

Sets the window pixel format.

See the OpenGL documentation for more information on pixel formats.  Setting
the pixel format is not required.

Setting the pixel format is currently ignored and does not affect
the OpenGL context chosen.
\param fmt the new pixel format for the window.
\return bool - <table>
                  <tr><td>true</td><td>Window pixel format set.</td></tr>
                  <tr><td>false</td><td>Window pixel format not set.</td></tr>
               </table>
*/

/**
\fn bool UtoWindow::Attach( UtoWindowId wID, Multisampling nSamples )
\brief Attaches a Uto window to a platform specific window with multisampling.

Attaches a Uto window to a platform specific window with multisampling.A problem that
most OpenGL based applications run into is aliasing.  This is the "jaggies" that exist
on diagonal lines in relation to the square pixels that exist on the screen.  One process
used to achieve anti-aliasing is called "Multisampling."  The idea is that for each pixel,
pixels around the current pixel are sampled to determine if the edge needs to be anti-aliased,
basically by "smudging" the pixel itself.  Using this, however, comes with a performance cost
and a memory cost.  To compute an approximate amount of video memory required, the equation
below will help.<br><br>
VideoMem = sizeof(FrontBuffer) + sizeof(BackBuffer) + NumberOfSamples *
           (sizeof(FrontBuffer) + sizeof(ZBuffer))<br><br>
If the window does not support the requested multisample, the window will try to create
a rendering context with fewer samples.  If the graphics card does not support multisampling,
the card will just create a rendering context with no samples attached to it.<br><br>
This operation can only be called once per viewer.
\param wID the window id to attach to.
\param nSamples The number of samples to be attached to the window.
\return bool - <table>
                  <tr><td>true</td><td>Window attached.</td></tr>
                  <tr><td>false</td><td>Window not attached.</td></tr>
               </table>
*/

/**
\fn bool UtoWindow::WindowToWorld( UtoRect &rect )
\brief Converts the UtoRect points from window coordinates to world coordinates.

Converts the UtoRect points from window coordinates to world coordinates.  This
function does not support conversion to world coordinates when the perspective camera is
active.
\param rect the rectangular area in window coordinates.
\return bool - <table>
                  <tr><td>true</td><td>Parameter converted.</td></tr>
                  <tr><td>true</td><td>Parameter not converted.</td></tr>
               </table>
*/

/**
\fn bool UtoWindow::WindowToWorld( UtoRect &rect )
\brief Converts the UtoPoint from window coordinates to world coordinates.

Converts the UtoPoint from window coordinates to world coordinates.  This
function does not support conversion to world coordinates when the perspective camera is
active.
\param point the point in window coordinates.
\return bool - <table>
                  <tr><td>true</td><td>Parameter converted.</td></tr>
                  <tr><td>true</td><td>Parameter not converted.</td></tr>
               </table>
*/
//@}
