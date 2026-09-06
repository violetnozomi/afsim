// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GEOMETRY_GL_WIDGET_HPP
#define GEOMETRY_GL_WIDGET_HPP

#include <array>
#include <stack>
#include <vector>

#include "UtQtGLWidgetBase.hpp"
#include "UtVec3dX.hpp"
#include "Vehicle.hpp"

class QTreeWidgetItem;
class TextureFont;

namespace Designer
{
   class MaterialColorData
   {
      public:
         MaterialColorData() = default;
         ~MaterialColorData() = default;

         void SetProperties(GLfloat aBaseAmbient[4],
                            GLfloat aBaseDiffuse[4],
                            GLfloat aBaseSpecular[4],
                            GLfloat aCurrentAmbient[4],
                            GLfloat aCurrentDiffuse[4],
                            GLfloat aCurrentSpecular[4],
                            GLfloat aShininess,
                            GLfloat aBrightnessFactor);

         void GetProperties(GLfloat  aBaseAmbient[4],
                            GLfloat  aBaseDiffuse[4],
                            GLfloat  aBaseSpecular[4],
                            GLfloat  aCurrentAmbient[4],
                            GLfloat  aCurrentDiffuse[4],
                            GLfloat  aCurrentSpecular[4],
                            GLfloat& aShininess,
                            GLfloat& aBrightnessFactor) const;

      private:
         GLfloat mBaseAmbient[4]     = { 0.4f, 0.2f, 0.2f, 1.0f };
         GLfloat mBaseDiffuse[4]     = { 1.0f, 0.4f, 0.4f, 1.0f };
         GLfloat mBaseSpecular[4]    = { 1.0f, 0.6f, 0.6f, 1.0f };
         GLfloat mCurrentAmbient[4]  = { 0.4f, 0.2f, 0.2f, 1.0f };
         GLfloat mCurrentDiffuse[4]  = { 1.0f, 0.4f, 0.4f, 1.0f };
         GLfloat mCurrentSpecular[4] = { 1.0f, 0.6f, 0.6f, 1.0f };
         GLfloat mShininess          = 100.0f;
         GLfloat mBrightnessFactor   = 1.0f;
   };

   class GeometryGLFocusPoint
   {
      public:
         GeometryGLFocusPoint();
         GeometryGLFocusPoint(double aXPos, double aYPos, double aZPos);
         ~GeometryGLFocusPoint();

         void Draw();

         double GetX() { return mXPos; }
         double GetY() { return mYPos; }
         double GetZ() { return mZPos; }

      private:
         double mXPos = 0.0;
         double mYPos = 0.0;
         double mZPos = 0.0;
   };

   class GeometryGLWidget : public UtQtGLWidgetBase
   {
      public:
         explicit GeometryGLWidget(QWidget* aParent = 0);
         ~GeometryGLWidget();

         // This should be called when geometry objects are modified
         // Set aInitialize to true when objects are first created
         void AdjustViewingDistance(bool aInitialize = false);

         // Override this function to initialize the texture font
         // that is used to label the axes
         void initializeGL() override;

         // User controllable toggle functions
         void ToggleAxis();
         void ToggleSelectedObjectCG();
         void ToggleShowEngines();
         void ToggleThrustVectors();
         void TogglePointMasses();
         void ToggleFuelTanks();
         void ToggleVehicleCG();
         void ToggleLandingGear();
         void ToggleSpeedBrakes();
         void ToggleShowWireframe();

         void ShowAxis(bool aShow);
         void ShowSelectedObjectCG(bool aShow);
         void ShowEngines(bool aShow);
         void ShowThrustVectors(bool aShow);
         void ShowPointMasses(bool aShow);
         void ShowFuelTanks(bool aShow);
         void ShowVehicleCG(bool aShow);
         void ShowLandingGear(bool aShow);
         void ShowSpeedBrakes(bool aShow);
         void ShowWireframe(bool aShow);
         void ShowAllAeroVectors();
         void ShowSelectedAeroVectors();

         void HideAeroVectors();

         void SetViewingFocusPointX(double aValue);
         void SetViewingFocusPointY(double aValue);
         void SetViewingFocusPointZ(double aValue);
         void SetViewingFocusPoint(UtVec3dX aFocusPoint);
         void MoveToFocusPoint(unsigned int aIdx);

         // Returns true if the tree widget item (or any of it's children) is selected
         bool TreeWidgetItemSelected(QTreeWidgetItem* aItem) const;

         double GetViewingFocusPointX();
         double GetViewingFocusPointY();
         double GetViewingFocusPointZ();
         UtVec3dX GetViewingFocusPoint();

         void SaveCurrentFocusPoint();

         int GetRegionStepSize() { return mRegionStepSize; }

         void ReCalculateEyePoint() { ReCalcEyePoint(); }

         // This calculates the vehicle extents (max/min distances in
         // each primary axis direction)
         void CalcVehicleExtents();

         double GetMinExtent() { return mMinExtent; }
         double GetMaxExtent() { return mMaxExtent; }

         void Update() override;

         bool GetResettingView() const { return mResetView; }
         void SetResettingView(bool aSet) { mResetView = aSet; }

      protected:
         void paintGL() override;
         void Draw() override;
         void QtDraw() override;

         void Draw3dView();
         void Draw2dOverlay();
         void Draw2dOverlayTopLeft();
         void Draw2dOverlayTopCenter();
         void Draw2dOverlayTopRight();
         void Draw2dOverlayBottomLeft();
         void Draw2dOverlayBottomRight();

         double EllipticalY_GivenX(double x, double aHalfWidth, double aHalfHeight);
         double EllipticalX_GivenY(double y, double aHalfWidth, double aHalfHeight);

         void DrawBodyCylinder(double aPosX,
                               double aPosY,
                               double aPosZ,
                               double aLength,
                               double aHeight,
                               double aWidth,
                               bool   aWireframe,
                               bool   aAft = false);

         void DrawBodyOgive(double aPosX,
                            double aPosY,
                            double aPosZ,
                            double aLength,
                            double aHeight,
                            double aWidth,
                            bool   aWireframe,
                            bool   aAft = false);

         void DrawBodyCone(double aPosX,
                           double aPosY,
                           double aPosZ,
                           double aLength,
                           double aHeight,
                           double aWidth,
                           bool   aWireframe,
                           bool   aAft = false);

         void DrawBodySphere(double aPosX,
                             double aPosY,
                             double aPosZ,
                             double aLength,
                             double aHeight,
                             double aWidth,
                             bool   aWireframe,
                             bool   aAft = false);

         void DrawBodyBoatTail(double aPosX,
                               double aPosY,
                               double aPosZ,
                               double aLength,
                               double aHeight,
                               double aWidth,
                               double aDiam,
                               bool   aWireframe,
                               bool   aAft = false);

         void DrawBodyBlunt(double aPosX,
                            double aPosY,
                            double aPosZ,
                            double aLength,
                            double aHeight,
                            double aWidth,
                            bool   aWireframe,
                            bool   aAft = false);

         void DrawRoundedNacelle(double             aLength_ft,
                                 double             aHeight_ft,
                                 double             aWidth_ft,
                                 double             aThickness_ft,
                                 const std::string& aOverallShapeString,
                                 bool               aAftSectionIsTapered,
                                 double             aAftSectionLength_ft,
                                 bool               aWireframe);

         void DrawRoundedRightNacelle(double             aLength_ft,
                                      double             aHeight_ft,
                                      double             aWidth_ft,
                                      double             aThickness_ft,
                                      const std::string& aOverallShapeString,
                                      bool               aAftSectionIsTapered,
                                      double             aAftSectionLength_ft,
                                      bool               aWireframe);

         void DrawRoundedLeftNacelle(double             aLength_ft,
                                     double             aHeight_ft,
                                     double             aWidth_ft,
                                     double             aThickness_ft,
                                     const std::string& aOverallShapeString,
                                     bool               aAftSectionIsTapered,
                                     double             aAftSectionLength_ft,
                                     bool               aWireframe);

         void DrawRoundedTopNacelle(double             aLength_ft,
                                    double             aHeight_ft,
                                    double             aWidth_ft,
                                    double             aThickness_ft,
                                    const std::string& aOverallShapeString,
                                    bool               aAftSectionIsTapered,
                                    double             aAftSectionLength_ft,
                                    bool               aWireframe);

         void DrawRoundedBottomNacelle(double             aLength_ft,
                                       double             aHeight_ft,
                                       double             aWidth_ft,
                                       double             aThickness_ft,
                                       const std::string& aOverallShapeString,
                                       bool               aAftSectionIsTapered,
                                       double             aAftSectionLength_ft,
                                       bool               aWireframe);

         void DrawFlatSidedNacelle(double             aLength_ft,
                                   double             aHeight_ft,
                                   double             aWidth_ft,
                                   double             aThickness_ft,
                                   double             aForwardSweepLength_ft,
                                   const std::string& aOverallShapeString,
                                   bool               aAftSectionIsTapered,
                                   double             aAftSectionLength_ft,
                                   bool               aWireframe);

         void DrawFlatSweptRightNacelle(double             aLength_ft,
                                        double             aHeightInner_ft,
                                        double             aHeightOuter_ft,
                                        double             aWidth_ft,
                                        double             aThickness_ft,
                                        double             aForwardSweepLength_ft,
                                        const std::string& aOverallShapeString,
                                        bool               aAftSectionIsTapered,
                                        double             aAftSectionLength_ft,
                                        bool               aWireframe);

         void DrawFlatSweptLeftNacelle(double             aLength_ft,
                                       double             aHeightInner_ft,
                                       double             aHeightOuter_ft,
                                       double             aWidth_ft,
                                       double             aThickness_ft,
                                       double             aForwardSweepLength_ft,
                                       const std::string& aOverallShapeString,
                                       bool               aAftSectionIsTapered,
                                       double             aAftSectionLength_ft,
                                       bool               aWireframe);


         void DrawBody(double                         aPosX,
                       double                         aPosY,
                       double                         aPosZ,
                       double                         aLength,
                       double                         aHeight,
                       double                         aWidth,
                       GeometryBody::ForwardShapeType aFrontShape,
                       double                         aFrontLength,
                       GeometryBody::AftShapeType     aAftShape,
                       double                         aRearLength,
                       double                         aBoatTailDiam,
                       double                         aYaw_deg,
                       double                         aPitch_deg,
                       double                         aRoll_deg,
                       bool                           aWireframe);

         void DrawSurface(double aPosX,
                          double aPosY,
                          double aPosZ,
                          double aSpan,
                          double aSweep_deg,
                          double aRootChord,
                          double aTipChord,
                          double aThicknessRatio,
                          double aDihedral_deg,
                          double aIncidence_deg,
                          bool   aWireframe,
                          int    aNumSpanElements,
                          std::vector<GeometrySurface::GeometryPanel> aControlSurfaces);

         void DrawEngine(double aPosX,
                         double aPosY,
                         double aPosZ,
                         double aDiameter_ft,
                         double aLength_ft,
                         double aOffset_ft,
                         double aYaw_deg,
                         double aPitch_deg,
                         double aRoll_deg,
                         bool   aDrawThrustVector,
                         bool   aHighlighted,
                         bool   aWireframe);

         void DrawNacelle(double             aPosX,
                          double             aPosY,
                          double             aPosZ,
                          double             aLength_ft,
                          double             aHeight_ft,
                          double             aHeightInner_ft,
                          double             aHeightOuter_ft,
                          double             aWidth_ft,
                          double             aThickness_ft,
                          double             aForwardSweepLength_ft,
                          const std::string& aOverallShapeString,
                          const std::string& aAftSectionString,
                          double             aAftSectionLength_ft,
                          double             aYaw_deg,
                          double             aPitch_deg,
                          double             aRoll_deg,
                          bool               aWireframe);

         void DrawSpeedBrake(double aPosX,
                             double aPosY,
                             double aPosZ,
                             double aLength_ft,
                             double aWidth_ft,
                             double aRoll_deg,
                             double aMaxAngle_deg,
                             bool   aWireframe);

         void DrawLandingGear(double aPosX,
                              double aPosY,
                              double aPosZ,
                              double aLength_ft,
                              double aStrutDiam_ft,
                              double aTireDiam_ft,
                              double aTireWidth_ft,
                              double aExtendedAngle_deg,
                              bool   aWireframe);

         void DrawDish(double aPosX,
                       double aPosY,
                       double aPosZ,
                       double aDiameter_ft,
                       double aThickness_ft,
                       bool   aWireframe);

         void DrawPointMass(double aPosX,
                            double aPosY,
                            double aPosZ,
                            double aDiameter_ft,
                            bool   aWireframe);

         void DrawFuelTank(double aPosX,
                           double aPosY,
                           double aPosZ,
                           double aDiameter_ft,
                           bool   aWireframe);

         void DrawCube(double aLength);
         void DrawSphereAtPoint(double aX, double aY, double aZ, double aRadius);
         void DrawFocusPoints();
         void DrawBox(double aLength, double aHeight, double aWidth, bool aWireframe);
         void DrawBoxWithHole(double aLength,
                              double aHeight,
                              double aWidth,
                              double aThickness,
                              bool   aWireframe);

         // 3D arrow functions
         void DrawCone(UtVec3dX& aPtVec, double aLength, double aDiameter);

         void DrawDisk(UtVec3dX& aPtVec,
                       double    aRadius,
                       bool      aForward,
                       double    aInnerRadius = 0.0,
                       bool      aUseInnerRadius = false);

         void DrawCylinder(UtVec3dX& aPtVec,
                           double    aLength,
                           double    aDiameter,
                           bool      aDrawEnds = true,
                           bool      aInside = false,
                           bool      aWireframe = false);

         void DrawArrow(UtVec3dX& aPtVec,
                        double    aHeadLength,
                        double    aTotalLength,
                        double    aDiameter);

         void DrawAxisLabel(UtVec3dX&   aPtVec,
                            double      aTotalLength,
                            double      aDiameter,
                            const char* aLabel);

         void DrawEnclosureBox(double aHalfLength,
                               double aHalfWidth,
                               double aCeilingHeight,
                               double aFloorHeight,
                               int    aStepSize);

         void DrawAxisArrows();
         void DrawAxisArrowLabels();

         void DrawVehicle(Designer::Vehicle* aVehicle, bool aOnlySelectedObject);
         virtual void DrawEngines(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawBodiesAndNacelles(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawWingsAndSurfaces(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawSpeedBrakes(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawLandingGear(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawDishes(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawCanopy(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawFuelTanks(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);
         virtual void DrawPointMasses(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe);

         void DrawAeroData(Designer::Vehicle* aVehicle, bool aOnlySelectedObject);

         void DrawAeroVectors(UtVec3dX& aAeroCenterLowMach,
                              UtVec3dX& aAeroCenterHighMach,
                              UtVec3dX& aUpVec,
                              UtVec3dX& aSideVec,
                              UtVec3dX& aForwardVec,
                              UtVec3dX& aVelocityVec,
                              UtVec3dX& aDragForceVec,
                              UtVec3dX& aSideForceVec,
                              UtVec3dX& aLiftForceVec,
                              double    aArrowLength,
                              double    aArrowDiam);

         // This only uses a single aero center
         void DrawAeroVectors(UtVec3dX& aAeroCenter,
                              UtVec3dX& aUpVec,
                              UtVec3dX& aSideVec,
                              UtVec3dX& aForwardVec,
                              UtVec3dX& aVelocityVec,
                              UtVec3dX& aDragForceVec,
                              UtVec3dX& aSideForceVec,
                              UtVec3dX& aLiftForceVec,
                              double    aArrowLength,
                              double    aArrowDiam);

         // This only draws the drag vector
         void DrawAeroVectors(UtVec3dX& aAeroCenter,
                              UtVec3dX& aDragForceVec,
                              double    aArrowLength,
                              double    aArrowDiam);

         void DrawAeroDataForSingleObject(Designer::Vehicle* aVehicle,
                                          AeroObject*        aAeroObject,
                                          double             aArrowLength,
                                          double             aArrowDiam);

         // Draw the CG symbol at the specified location.
         // Set aIsPart to false for whole vehicle and true for an individual part.
         void DrawCgSymbol(UtVec3dX aCgVector, double aRadius, bool aIsPart);

         // This uses an input vector and returns the yaw and pitch for use with drawing
         void DetermineYawPitchForVector(UtVec3dX aVector, double& aYaw_deg, double& aPitch_deg);

         enum wallType { cPosLengthWall, cNegLengthWall, cPosWidthWall, cNegWidthWall };

         void DrawWall(wallType aType,
                       double   aHalfLength,
                       double   aHalfWidth,
                       double   aCeilingHeight,
                       double   aFloorHeight,
                       int      aStepSize);

         void DrawFloor(double aHalfLength,
                        double aHalfWidth,
                        double aHeight,
                        int    aStepSize);

         void DrawCeiling(double aHalfLength,
                          double aHalfWidth,
                          double aHeight,
                          int    aStepSize);

         void SetColorWalls();
         void SetColorWallGrid();
         void SetColorFloor();
         void SetColorFloorGrid();
         void SetColorCeiling();
         void SetColorCeilingGrid();
         void SetColorOverlayNominal();
         void SetColorOverlayWarning();

         void SetDarkCgColor(bool aIsPart);
         void SetLightCgColor(bool aIsPart);
         void SetBorderCgColor(bool aIsPart);

         // Recalculates mEyePt based on mViewAz_deg, mViewEl_deg, mEyeDist, and mViewingFocusPoint
         void ReCalcEyePoint();

         // Recalculates mLightVec based on mLightAz_deg and mLightEl_deg
         void ReCalcLightGeometry();

         void LimitFOV(double& aFov_deg);

         // Returns the selected geometry object, or nullptr if none is selected
         GeometryObject* GetSelectedGeometryObject();

         // Returns true if the geometry object is renderable
         bool GeometryObjectIsRenderable(GeometryObject* aObject);

         // Returns the selected tree widget, or nullptr if none is selected
         QTreeWidgetItem* GetSelectedTreeWidgetItem();

         // This returns true if a valid calculation of max span was performed.
         // This is the total span from tip to tip
         bool MaxFinSpan(double& aMaxSpan_ft);

         virtual void DrawItems();
         void DrawBillboardItems();
         void DrawLightVector();

         void DrawDirectImage(double aLength,
                              double aWidth,
                              double aCeilingHeight,
                              double aFloorHeight,
                              int    aStepSize);

         void DrawReflectedImage(double aLength,
                                 double aWidth,
                                 double aFloorHeight);

         void SetLightProperties();

         void SetDrawingSelectedMode();
         void SetDrawingNonSelectedMode();
         void SetDrawingNothingSelectedMode();

         void SetNominalMaterialProperties(double aAlpha = 1.0);
         void SetEngineMaterialProperties(double aAlpha = 1.0);
         void SetHighlightedMaterialProperties(double aAlpha = 1.0);

         void SetRedMaterialProperties();           // Full Red
         void SetRedOrangeMaterialProperties();     // More red than green
         void SetOrangeMaterialProperties();        // More red than green
         void SetYellowOrangeMaterialProperties();  // More red than green
         void SetYellowMaterialProperties();        // Equal red and green
         void SetYellowGreenMaterialProperties();   // More green than red
         void SetGreenMaterialProperties();         // Full Green
         void SetGreenCyanMaterialProperties();     // More green than blue
         void SetCyanMaterialProperties();          // Equal green and blue
         void SetBlueCyanMaterialProperties();      // More blue than green
         void SetBlueMaterialProperties();          // Full Blue
         void SetBlueVioletMaterialProperties();    // More blue than red
         void SetVioletMaterialProperties();        // Equal red and blue
         void SetRedVioletMaterialProperties();     // More red than blue
         void SetDarkGrayMaterialProperties();      // Equal red, green, and blue
         void SetGrayMaterialProperties();          // Equal red, green, and blue
         void SetLightGrayMaterialProperties();     // Equal red, green, and blue

         void SetMaterialProperties();
         void SetAmbientMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha);
         void SetDiffuseMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha);
         void SetSpecularMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha);
         void EnforceLimitsForProperties(GLfloat aAmbient[4]);
         void DarkenMaterialProperties(GLfloat aFactor);
         void LightenMaterialProperties(GLfloat aFactor);
         void RestoreMaterialProperties();

         void PushMaterialProperties();
         void PopMaterialProperties();

         void SetRedArrowLabelColor();
         void SetGreenArrowLabelColor();
         void SetBlueArrowLabelColor();

         void SetNominalWireframeColor();
         void SetSelectedWireframeColor();

         void DrawStenciledFloor(double aLength, double aWidth, double aFloorHeight);

         // Event handlers
         void mousePressEvent(QMouseEvent* event) override;
         void mouseReleaseEvent(QMouseEvent* event) override;
         void mouseMoveEvent(QMouseEvent* event) override;
         void wheelEvent(QWheelEvent* event) override;

         // This called from Draw() and AdjustViewingDistance() to determine things like
         // the size of the region box
         void CalcViewRegionCharacteristics();

         // This is called when the user zooms in/out
         void AdjustEyeDataZoomInOut(double aFactor);

         // Returns true if the vehicle contains landing gear
         bool LandingGearArePresent();

         // This is called by AdjustEyeDataZoomInOut() and AdjustViewingDistance().
         // It will attempt to set mEyeDist to aDesiredViewingDistance, but will
         // enforce max/min viewing distances. Set aInitialize to true when objects
         // are first created.
         void AdjustEyeData(double aDesiredViewingDistance, bool aInitialize = false);

         // This calculates the clipping plane distances, based on current viewing parameters
         void CalcClippingPlaneDistances();

         void SaveToggleSettings();

         double        mViewAz_deg           = 150.0;
         double        mDefaultViewAz_deg    = 150.0;
         double        mViewEl_deg           = 20.0;
         double        mDefaultViewEl_deg    = 20.0;
         double        mLightAz_deg          = 150.0;
         double        mDefaultLightAz_deg   = 150.0;
         double        mLightEl_deg          = 60.0;
         double        mDefaultLightEl_deg   = 60.0;
         int           mMouseX               = 0;
         int           mMouseY               = 0;
         bool          mMouseDown            = false;
         bool          mWheelDown            = false;
         bool          mRgtMouseDown         = false;
         double        mFullFovY_deg         = 40.0;
         double        mEyeDist              = 400.0;
         double        mNearDist             = 0.5;
         double        mFarDist              = 10000.0;
         UtVec3dX      mEyePt;
         UtVec3dX      mLightVec;
         bool          mShowLightVector      = false;
         UtVec3dX      mViewingFocusPoint;
         UtVec3dX      mObjectCenter;

         // User togglable show/hide options

         bool          mShowAxis                = true;
         bool          mShowSelectedObjectCG    = true;
         bool          mShowEngines             = true;
         bool          mShowThrustVectors       = false;
         bool          mShowPointMasses         = false;
         bool          mShowFuelTanks           = false;
         bool          mShowVehicleCG           = true;
         bool          mShowLandingGear         = true;
         bool          mShowSpeedBrakes         = true;
         bool          mShowAllAeroVectors      = false;
         bool          mShowSelectedAeroVectors = false;
         bool          mSaveOptions             = true;
         bool          mShowWireframe           = false;
         bool          mUseDefaults             = false;

         // Normalized points for top surface of airfoil measured aft of leading edge in X
         static std::array<double, 7> mAirfoilPtsX;
         static std::array<double, 7> mAirfoilPtsY;

         // Ogive with normalized X/Y from base
         static std::array<double, 5> mOgivePtsX;
         static std::array<double, 5> mOgivePtsY;

         // Arrow data is in vehicle coords
         double        mArrowheadLength     = 0.0;
         double        mArrowDiameter       = 0.0;
         double        mArrowX_Length       = 0.0;
         double        mArrowY_Length       = 0.0;
         double        mArrowZ_Length       = 0.0;

         // Region data is in viewing coords
         double        mRegionX             = 0.0;
         double        mRegionFloorY        = 0.0;
         double        mRegionCeilingY      = 0.0;
         double        mRegionZ             = 0.0;
         int           mRegionStepSize      = 0;

         // Extent data is in vehicle coords
         double        mExtentMinX          = 0.0;
         double        mExtentMinY          = 0.0;
         double        mExtentMinZ          = 0.0;
         double        mExtentMaxX          = 0.0;
         double        mExtentMaxY          = 0.0;
         double        mExtentMaxZ          = 0.0;
         double        mExtentX             = 0.0;
         double        mExtentY             = 0.0;
         double        mExtentZ             = 0.0;
         double        mMinExtent           = 0.0;
         double        mMaxExtent           = 0.0;
         double        mDistForMaxExtent    = 0.0;

         double        mMinViewDist         = 0.0;
         double        mMaxViewDist         = 0.0;
         double        mMaxDiagonalToCorner = 0.0;

         bool          mDrawingSelectedMode = false;

         // Current material color properties
         GLfloat       mBaseAmbient[4]     = { 0.4f, 0.2f, 0.2f, 1.0f };
         GLfloat       mBaseDiffuse[4]     = { 1.0f, 0.4f, 0.4f, 1.0f };
         GLfloat       mBaseSpecular[4]    = { 1.0f, 0.6f, 0.6f, 1.0f };
         GLfloat       mCurrentAmbient[4]  = { 0.4f, 0.2f, 0.2f, 1.0f };
         GLfloat       mCurrentDiffuse[4]  = { 1.0f, 0.4f, 0.4f, 1.0f };
         GLfloat       mCurrentSpecular[4] = { 1.0f, 0.6f, 0.6f, 1.0f };
         GLfloat       mShininess          = 100.0f;
         GLfloat       mBrightnessFactor   = 1.0f;

         TextureFont*  mTextFont           = nullptr;

         std::stack<MaterialColorData> mMaterialColorStack;

         double        mEpsilon            = std::numeric_limits<double>::epsilon();

         std::vector<GeometryGLFocusPoint> mFocusPoints;

         bool mResetView = true;
   };
}

#endif // !GEOMETRY_GL_WIDGET_HPP
