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

#ifndef UTQTGRADIENTMAP_HPP
#define UTQTGRADIENTMAP_HPP

#include "utilqt_export.h"

#include <QGradientStops>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <QVector>

#include "UtCallback.hpp"
#include "UtColor.hpp"

using UtQtGradientList = QVector<QPair<QString, QGradientStops>>;

//! Maps from a name/string to a list of gradient stops. The stops can be used
//! to create a gradient of any type. Signals are emitted when gradients are
//! added and removed from the map.
class UTILQT_EXPORT UtQtGradientMap : public QObject
{
   Q_OBJECT

public:
   UtQtGradientMap();

   static QGradientStops DefaultGradient();
   static QGradientStops FireGradient();
   static QGradientStops IceGradient();
   static QGradientStops BoneGradient();
   static QGradientStops HSV_Gradient();
   static QGradientStops HalfGradient();
   static QGradientStops DefaultInverseGradient();
   static QGradientStops FireInverseGradient();
   static QGradientStops IceInverseGradient();
   static QGradientStops BoneInverseGradient();
   static QGradientStops HSV_InverseGradient();
   static QGradientStops HalfInverseGradient();
   static QString        cDEFAULT_GRADIENT_KEY;
   static QString        cFIRE_GRADIENT_KEY;
   static QString        cICE_GRADIENT_KEY;
   static QString        cBONE_GRADIENT_KEY;
   static QString        cHSV_GRADIENT_KEY;
   static QString        cHALF_GRADIENT_KEY;
   static QString        cDEFAULT_INVERSE_GRADIENT_KEY;
   static QString        cFIRE_INVERSE_GRADIENT_KEY;
   static QString        cICE_INVERSE_GRADIENT_KEY;
   static QString        cBONE_INVERSE_GRADIENT_KEY;
   static QString        cHSV_INVERSE_GRADIENT_KEY;
   static QString        cHALF_INVERSE_GRADIENT_KEY;

   UtQtGradientList Gradients() const;
   QGradientStops   GradientStops(const QString& aName) const;
   void             SetGradientStops(const QString& aName, const QGradientStops& aGradientStops);
   void             RemoveGradient(const QString& aName);
   bool             Contains(const QString& aName) const;

   UtCallbackListN<void(const std::string&, const QGradientStops&)> OnGradientUpdated;

   static bool IsDefaultGradient(const QString& aString);

   void GetUtStops(const QString& aString, std::map<float, UtColor>& aStops) const;

   static QGradientStops InvertGradient(const QGradientStops& aGradientStops);

signals:
   void GradientUpdated(const QString& aName, const QGradientStops& aGradientStops);
   void GradientRemoved(const QString& aName);

private:
   using MapType = QMap<QString, QGradientStops>;
   MapType mGradientMap;
};

#endif // UTQTGRADIENTMAP_HPP
