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

// UtoCmeTerrain.cpp: implementation of the UtoCmeTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "UtoCmeTerrain.hpp"

#include "UtoCmeTerrainImp.hpp"
#include "UtoTerrainFactory.hpp"

/**
\ingroup terrain
\class UtoCmeTerrain
\brief Constructs a Cme terrain.

This is the Cme Terrain class.  Terrains can only be created by the
UtoResourceDB using the UtoResourceDB::LoadTerrain method; however,
copies of a terrain can be made by way of the copy constructor.
This terrain is used to use terrain that are in flt, osg, ive, or other
model formats.  Terrains are created using the .dat file format.
*/
class UtoCmeTerrain;

// {secret}
UtoTerrain* CreateCmeTerrain()
{
   return new UtoCmeTerrain;
}

// By using an unnamed namespace we can force the initialization and thus the
// function calls
namespace
{
bool m_bCreateCmeTerrain = UtoTerrainFactory::Instance()->RegisterTerrain("cme_terrain", CreateCmeTerrain);
}

UtoCmeTerrain::UtoCmeTerrain()
   : m_pImpRoot(new UtoCmeTerrainImp)
{
}

/** @name Constructor & Destructors **/
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoCmeTerrain::UtoCmeTerrain(const UtoCmeTerrain& rhs)
   : m_pImpRoot(new UtoCmeTerrainImp(*rhs.m_pImpRoot))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoCmeTerrain::~UtoCmeTerrain()
{
   delete m_pImpRoot;
}
//@}

// {secret}
UtoTerrainImp* UtoCmeTerrain::Impl()
{
   return m_pImpRoot;
}

// {secret}
const UtoTerrainImp* UtoCmeTerrain::Impl() const
{
   return m_pImpRoot;
}

// {secret}
UtoTerrain* UtoCmeTerrain::Clone() const
{
   return new UtoCmeTerrain(*this);
}

//{secret}
void UtoCmeTerrain::Dump(std::ostream&) {}

/** @name Accessors */
//@{

/**
\brief Returns the terrain name.

Returns the terrain name.
\return std::string - the name of the terrain.
*/
std::string UtoCmeTerrain::Name()
{
   return m_pImpRoot->Name();
}

/**
\brief Returns the current blended color of the terrain.

Returns the current blended color of the terrain.
\return const UtoColor & - a reference to the current blend values.
*/
const UtoColor& UtoCmeTerrain::Color() const
{
   return m_pImpRoot->Color();
}
//@}

/** @name Operations */
//@{

/**
\brief Sets the terrain name.

Sets the terrain name.  This is set automatically when the terrain is loaded
into the resource database.
\param Name the new name of the terrain.
*/
void UtoCmeTerrain::SetName(const std::string& Name)
{
   m_pImpRoot->SetName(Name);
}

/**
\brief Sets the blended color of the terrain.

Sets the blended color of the terrain.
\param rColor the new color blending value.
\param nClrMask the color components to be updated.
*/
void UtoCmeTerrain::SetColor(const UtoColor& rColor, unsigned int nClrMask)
{
   m_pImpRoot->SetColor(rColor, nClrMask);
}
//@}

void UtoCmeTerrain::Height(double* rPosn) const
{
   m_pImpRoot->Height(rPosn);
}

void UtoCmeTerrain::Height(double* rPosn, UtoOrientation& rOrient) const
{
   m_pImpRoot->Height(rPosn, &rOrient);
}

bool UtoCmeTerrain::Intersect(const double aPos[3], const double aVec[3], double aIPoint[3], double aINorm[3], double aMagnitude) const
{
   return m_pImpRoot->Intersect(aPos, aVec, aIPoint, aINorm, aMagnitude);
}

bool UtoCmeTerrain::Intersect(osgUtil::IntersectionVisitor* aIsectVisitor) const
{
   return m_pImpRoot->Intersect(aIsectVisitor);
}

bool UtoCmeTerrain::IsColorBlendingDisabled() const
{
   return m_pImpRoot->IsColorBlendingDisabled();
}

void UtoCmeTerrain::SetDisableColorBlending(bool bDisable)
{
   m_pImpRoot->DisableColorBlending(bDisable);
}

void UtoCmeTerrain::SetUniform(const std::string& aName, const float aValue)
{
   m_pImpRoot->SetUniform(aName, aValue);
}

void UtoCmeTerrain::SetUniform(const std::string& aName, const float aValue[4])
{
   m_pImpRoot->SetUniform(aName, aValue);
}

void UtoCmeTerrain::SetUniformMatrix(const std::string& aName, const double aValue[][4])
{
   m_pImpRoot->SetUniformMatrix(aName, aValue);
}

bool UtoCmeTerrain::Lighting()
{
   return m_pImpRoot->Lighting();
}

void UtoCmeTerrain::EnableLighting(const bool aLit)
{
   m_pImpRoot->EnableLighting(aLit);
}

void UtoCmeTerrain::SetFiles(const std::string& aFiles)
{
   m_pImpRoot->SetFiles(aFiles);
}

bool UtoCmeTerrain::SupportsLayers() const
{
   return m_pImpRoot->SupportsLayers();
}

bool UtoCmeTerrain::AddShapeLayer(const std::string& aFilename)
{
   return m_pImpRoot->AddShapeLayer(aFilename);
}

bool UtoCmeTerrain::AddShapeLayer(const std::string& aFilename, float aRed, float aGreen, float aBlue)
{
   return m_pImpRoot->AddShapeLayer(aFilename, aRed, aGreen, aBlue);
}

void UtoCmeTerrain::RemoveLayer(const std::string& aFilename)
{
   m_pImpRoot->RemoveLayer(aFilename);
}

void UtoCmeTerrain::ShowLayer(const std::string& aFilename, bool aState)
{
   m_pImpRoot->ShowLayer(aFilename, aState);
}

void UtoCmeTerrain::SetShapeLayerColor(const std::string& aFilename, float aRed, float aGreen, float aBlue)
{
   m_pImpRoot->SetShapeLayerColor(aFilename, aRed, aGreen, aBlue);
}

bool UtoCmeTerrain::AddImageLayer(const std::string& aFilename)
{
   return m_pImpRoot->AddImageLayer(aFilename);
}

bool UtoCmeTerrain::AddImageLayer(const std::string& aFilename, double aMinLat, double aMinLon, double aMaxLat, double aMaxLon)
{
   return m_pImpRoot->AddImageLayer(aFilename, aMinLat, aMinLon, aMaxLat, aMaxLon);
}

unsigned int UtoCmeTerrain::AddProjector(const std::string& aName, const UtColor& aColor)
{
   return m_pImpRoot->AddProjector(aName, aColor);
}

void UtoCmeTerrain::RemoveProjector(unsigned int aId)
{
   m_pImpRoot->RemoveProjector(aId);
}

void UtoCmeTerrain::UpdateProjectorMatrix(unsigned int       aProjectorId,
                                          const osg::Matrix& aViewMatrix,
                                          const osg::Matrix& aProjectionMatrix)
{
   return m_pImpRoot->UpdateProjectorMatrix(aProjectorId, aViewMatrix, aProjectionMatrix);
}

bool UtoCmeTerrain::AddSensorPlotImageLayer(const std::string&              aFilename,
                                            const std::string&              aIdentifier,
                                            double                          aMinValue,
                                            double                          aMaxValue,
                                            unsigned int                    aDataSet,
                                            const std::map<float, UtColor>& aGradient)
{
   return m_pImpRoot->AddSensorPlotImageLayer(aFilename, aIdentifier, aMinValue, aMaxValue, aDataSet, aGradient);
}

void UtoCmeTerrain::SetSensorPlotLayerGradient(const std::string&              aIdentifier,
                                               double                          aMinValue,
                                               double                          aMaxValue,
                                               const std::map<float, UtColor>& aGradient)
{
   m_pImpRoot->SetSensorPlotLayerGradient(aIdentifier, aMinValue, aMaxValue, aGradient);
}

void UtoCmeTerrain::SetImageLayerOpacity(const std::string& aIdentifier, float aValue)
{
   m_pImpRoot->SetImageLayerOpacity(aIdentifier, aValue);
}
