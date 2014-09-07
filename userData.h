#ifndef USERDATA_H
#define USERDATA_H

#include "mainwindow.h"
#include <qwt_curve_fitter.h>

extern bool loadSettings();
extern void saveInterpolatedCurvePoints(QPolygonF &fittedPolygonF, MainWindow::splineType splineType );
extern void saveBasicCurvePoints();
extern void saveInterpolatedCurveCoeffs( const QVector<double> &yCoeff,
                                         const QVector<double> &aCoeff,
                                         const QVector<double> &bCoeff,
                                         const QVector<double> &cCoeff );

#endif // USERDATA_H
