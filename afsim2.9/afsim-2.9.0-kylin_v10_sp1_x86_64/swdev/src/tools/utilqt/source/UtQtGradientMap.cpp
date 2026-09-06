// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtGradientMap.hpp"

QString UtQtGradientMap::cDEFAULT_GRADIENT_KEY = "Default";
QString UtQtGradientMap::cFIRE_GRADIENT_KEY    = "Fire";
QString UtQtGradientMap::cICE_GRADIENT_KEY     = "Ice";
QString UtQtGradientMap::cBONE_GRADIENT_KEY    = "Bone";
QString UtQtGradientMap::cHSV_GRADIENT_KEY     = "HSV";
QString UtQtGradientMap::cHALF_GRADIENT_KEY    = "Half";

QString UtQtGradientMap::cDEFAULT_INVERSE_GRADIENT_KEY = "Default Inverse";
QString UtQtGradientMap::cFIRE_INVERSE_GRADIENT_KEY    = "Fire Inverse";
QString UtQtGradientMap::cICE_INVERSE_GRADIENT_KEY     = "Ice Inverse";
QString UtQtGradientMap::cBONE_INVERSE_GRADIENT_KEY    = "Bone Inverse";
QString UtQtGradientMap::cHSV_INVERSE_GRADIENT_KEY     = "HSV Inverse";
QString UtQtGradientMap::cHALF_INVERSE_GRADIENT_KEY    = "Half Inverse";

UtQtGradientMap::UtQtGradientMap()
{
   mGradientMap[cDEFAULT_GRADIENT_KEY]         = DefaultGradient();
   mGradientMap[cFIRE_GRADIENT_KEY]            = FireGradient();
   mGradientMap[cICE_GRADIENT_KEY]             = IceGradient();
   mGradientMap[cBONE_GRADIENT_KEY]            = BoneGradient();
   mGradientMap[cHSV_GRADIENT_KEY]             = HSV_Gradient();
   mGradientMap[cHALF_GRADIENT_KEY]            = HalfGradient();
   mGradientMap[cDEFAULT_INVERSE_GRADIENT_KEY] = DefaultInverseGradient();
   mGradientMap[cFIRE_INVERSE_GRADIENT_KEY]    = FireInverseGradient();
   mGradientMap[cICE_INVERSE_GRADIENT_KEY]     = IceInverseGradient();
   mGradientMap[cBONE_INVERSE_GRADIENT_KEY]    = BoneInverseGradient();
   mGradientMap[cHSV_INVERSE_GRADIENT_KEY]     = HSV_InverseGradient();
   mGradientMap[cHALF_INVERSE_GRADIENT_KEY]    = HalfInverseGradient();
}

//! A reasonable, colorful default gradient. This is based off of the gradient
//! used in CME's PlotView.
QGradientStops UtQtGradientMap::DefaultGradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::darkBlue));
   gradientStops.append(qMakePair(0.1, Qt::blue));
   gradientStops.append(qMakePair(0.25, Qt::cyan));
   gradientStops.append(qMakePair(0.5, Qt::green));
   gradientStops.append(qMakePair(0.75, Qt::yellow));
   gradientStops.append(qMakePair(0.9, Qt::red));
   gradientStops.append(qMakePair(1.0, Qt::darkRed));
   return gradientStops;
}

//! A reasonable, colorful default gradient. This is based off of the gradient
//! used in CME's PlotView.
QGradientStops UtQtGradientMap::FireGradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::black));
   gradientStops.append(qMakePair(0.375, Qt::red));
   gradientStops.append(qMakePair(0.75, Qt::yellow));
   gradientStops.append(qMakePair(1.0, Qt::white));
   return gradientStops;
}

//! A reasonable, colorful default gradient.
QGradientStops UtQtGradientMap::IceGradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::darkBlue));
   gradientStops.append(qMakePair(0.375, QColor(0, 80, 196)));
   gradientStops.append(qMakePair(0.75, QColor(100, 200, 200)));
   gradientStops.append(qMakePair(1.0, Qt::white));
   return gradientStops;
}

//! A reasonable, colorful default gradient. This is based off of the gradient
//! used in CME's PlotView.
QGradientStops UtQtGradientMap::BoneGradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::black));
   gradientStops.append(qMakePair(0.36, QColor(81, 81, 113)));
   gradientStops.append(qMakePair(0.74, QColor(166, 198, 198)));
   gradientStops.append(qMakePair(1.0, Qt::white));
   return gradientStops;
}

//! A reasonable, colorful default gradient. This is based off of the gradient
//! used in CME's PlotView.
QGradientStops UtQtGradientMap::HSV_Gradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::red));
   gradientStops.append(qMakePair(0.17, Qt::yellow));
   gradientStops.append(qMakePair(0.33, Qt::green));
   gradientStops.append(qMakePair(0.5, Qt::cyan));
   gradientStops.append(qMakePair(0.66, Qt::blue));
   gradientStops.append(qMakePair(0.83, Qt::magenta));
   gradientStops.append(qMakePair(1.0, Qt::red));
   return gradientStops;
}

//! A reasonable, gradient with a sharp transition at 50%.
QGradientStops UtQtGradientMap::HalfGradient()
{
   QGradientStops gradientStops;
   gradientStops.append(qMakePair(0.0, Qt::transparent));
   gradientStops.append(qMakePair(0.01, Qt::black));
   gradientStops.append(qMakePair(0.4999, Qt::black));
   gradientStops.append(qMakePair(0.5001, Qt::white));
   gradientStops.append(qMakePair(1.0, Qt::white));
   return gradientStops;
}

//! The inverse of the Default gradient.
QGradientStops UtQtGradientMap::DefaultInverseGradient()
{
   return InvertGradient(DefaultGradient());
}

//! The inverse of the Fire gradient.
QGradientStops UtQtGradientMap::FireInverseGradient()
{
   return InvertGradient(FireGradient());
}

//! The inverse of the Ice gradient.
QGradientStops UtQtGradientMap::IceInverseGradient()
{
   return InvertGradient(IceGradient());
}

//! The inverse of the Bone gradient.
QGradientStops UtQtGradientMap::BoneInverseGradient()
{
   return InvertGradient(BoneGradient());
}

//! The inverse of the HSV gradient.
QGradientStops UtQtGradientMap::HSV_InverseGradient()
{
   return InvertGradient(HSV_Gradient());
}

//! The inverse of the Half gradient.
QGradientStops UtQtGradientMap::HalfInverseGradient()
{
   return InvertGradient(HalfGradient());
}

//! Returns the gradients in a list. The default is at the head of the list.
UtQtGradientList UtQtGradientMap::Gradients() const
{
   UtQtGradientList gradientList;
   for (UtQtGradientMap::MapType::const_iterator i = mGradientMap.begin(); i != mGradientMap.end(); ++i)
   {
      if (i.key() == cDEFAULT_GRADIENT_KEY)
      {
         gradientList.push_front(qMakePair(i.key(), i.value()));
      }
      else
      {
         gradientList.push_back(qMakePair(i.key(), i.value()));
      }
   }
   return gradientList;
}

//! Returns the stops for the given gradient name.
QGradientStops UtQtGradientMap::GradientStops(const QString& aName) const
{
   return mGradientMap[aName];
}

//! Sets the stops for the given gradient name. Emits a signal for the change.
void UtQtGradientMap::SetGradientStops(const QString& aName, const QGradientStops& aGradientStops)
{
   mGradientMap[aName] = aGradientStops;
   emit GradientUpdated(aName, aGradientStops);
   OnGradientUpdated(aName.toStdString(), aGradientStops);
}

//! Removes the gradient for the given name. Emits a signal for the change.
void UtQtGradientMap::RemoveGradient(const QString& aName)
{
   mGradientMap.remove(aName);
   emit GradientRemoved(aName);
}

//! Checks if a gradient with the given name exists in the map.
bool UtQtGradientMap::Contains(const QString& aName) const
{
   return mGradientMap.contains(aName);
}

bool UtQtGradientMap::IsDefaultGradient(const QString& aString)
{
   if (aString == cDEFAULT_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cFIRE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cICE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cBONE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cHSV_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cDEFAULT_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cFIRE_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cICE_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cBONE_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cHSV_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   else if (aString == cHALF_INVERSE_GRADIENT_KEY)
   {
      return true;
   }
   return false;
}

void UtQtGradientMap::GetUtStops(const QString& aString, std::map<float, UtColor>& aStops) const
{
   aStops.clear();
   auto&& grad = mGradientMap.find(aString);
   if (grad != mGradientMap.end())
   {
      for (auto&& stop : grad.value())
      {
         aStops[stop.first] = UtColor(stop.second.red(), stop.second.green(), stop.second.blue(), stop.second.alpha());
      }
   }
}

QGradientStops UtQtGradientMap::InvertGradient(const QGradientStops& aGradientStops)
{
   QGradientStops retval{};
   for (auto& stop : aGradientStops)
   {
      retval.push_front(qMakePair(1.0 - stop.first, stop.second));
   }
   return retval;
}
