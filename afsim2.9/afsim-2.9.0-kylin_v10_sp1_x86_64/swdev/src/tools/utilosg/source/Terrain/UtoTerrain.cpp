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
// UtoTerrain.cpp : implementation file

#include "UtoTerrain.hpp"

#include "UtoTerrainImp.hpp"

/**
\ingroup terrain
\class UtoTerrain
\brief Base class for all terrains.

This is the Terrain class, it is abstract thus cannot be instantiated.
Objects of this class are created by the UtoResourceDB::LoadTerrain method.
This class defines the interface that all terrain resources have.
*/
class UtoTerrain;

///////////////////////////////////////////////////////////////////////////////
// {secret}
UtoTerrain::UtoTerrain() {}

/** @name Destructors */
//@{
/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoTerrain::~UtoTerrain() {}
//@}

/** @name Accessors */
//@{
/**
\fn virtual std::string UtoTerrain::Name( ) = 0
\brief Returns the terrain name.

Returns the terrain name.
\return std::string - the name of the terrain.
*/

/**
\fn virtual const UtoColor & UtoTerrain::Color( ) const = 0
\brief Returns the current blended color of the terrain.

Returns the current blended color of the terrain.
\return const UtoColor & - a reference to the current blend values.
*/

/**
\fn virtual void UtoTerrain::Height( double* rPosn, UtoOrientation & rOrient ) const = 0;
\brief Returns the height of terrain in the rPosn parameter.

Returns the height of terrain in the rPosn parameter.  The Z parameter will contain
the new height value for the given terrain coordinates.  The rPosn coordinate system
will be preserved.  If no height is found, the Z coordinate will be zero.  The orientation
will use the yaw component to calculate the correct pitch and roll for the specified location.
\param[in] rPosn the coordinates to be looked up.
\param[out] rPosn the coordinates after the lookup.  The Z coordinate contains the new height.
\param[out] rOrient the orientation of the terrain at the specified location.
*/

/**
\fn virtual void UtoTerrain::Height( double* rPosn ) const = 0;
\brief Returns the height of terrain in the rPosn parameter.

Returns the height of terrain in the rPosn parameter.  The Z parameter will contain
the new height value for the given terrain coordinates.  The rPosn coordinate system
will be preserved.  If no height is found, the Z coordinate will be zero.
\param[in] rPosn the coordinates to be looked up.  The Z coordinate is ignored.
\param[out] rPosn the coordinates after the lookup.  The Z coordinate contains the new height.
*/

/**
\fn virtual bool UtoTerrain::IsColorBlendingDisabled( ) const = 0
\brief Determines the current state of color blending.

Sets the state of color blending.
\return bool - <table>
                  <tr><td>true</td><td>Color blending is enabled.</td></tr>
                  <tr><td>false</td><td>Color blending is disabled.</td></tr>
               </table>
*/
//@}

/** @name Operations */
//@{
/**
\fn virtual void UtoTerrain::SetName( const std::string& Name ) = 0
\brief Sets the terrain name.

Sets the terrain name.  This is set automatically when the terrain is loaded
into the resource database.
\param Name the new name of the terrain.
*/

/**
\fn virtual void UtoTerrain::SetColor( const UtoColor & rColor, unsigned int nClrMask ) = 0
\brief Sets the blended color of the terrain.

Sets the blended color of the terrain.
\param rColor the new color blending value.
\param nClrMask the color components to be updated.
*/

/**
\fn virtual void UtoTerrain::SetDisableColorBlending( bool bDisable ) = 0
\brief Allows color blending to be enabled or disabled for the terrain.

Sets the state of color blending.
\param bDisable &nbsp; \htmlonly <table border="1"><tr><td>true</td><td>Enables color blending.</td></tr>
                                                   <tr><td>false</td><td>Disables color blending.  The current color for
the terrain will be reset to white.</td></tr>
                                  </table>
                       \endhtmlonly
*/

/**
\fn void UtoTerrain::SetUniform(const std::string &name, float value)
\brief see glUniform

*/
void UtoTerrain::SetUniform(const std::string& name, const float value)
{
   Impl()->SetUniform(name, value);
}

/**
\fn void UtoTerrain::SetUniform(const std::string &name, float value)
\brief see glUniform

*/
void UtoTerrain::SetUniform(const std::string& name, const float value[4])
{
   Impl()->SetUniform(name, value);
}

/**
\fn void UtoTerrain::SetUniform(const std::string &name, float value[][4])
\brief see glUniform

*/
void UtoTerrain::SetUniformMatrix(const std::string& name, const double value[][4])
{
   Impl()->SetUniformMatrix(name, value);
}

/**
\fn void UtoTerrain::SetUniformInverseMatrix(const std::string &name, float value[][4])
\brief see glUniform

*/
void UtoTerrain::SetUniformInverseMatrix(const std::string& name, const double value[][4])
{
   Impl()->SetUniformInverseMatrix(name, value);
}

void UtoTerrain::PostLoad()
{
   Impl()->PostLoad();
}

void UtoTerrain::SetProjection(const std::string& aProjection, const double& aOffset)
{
   Impl()->SetProjection(aProjection, aOffset);
}
//@}
