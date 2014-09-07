#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtWidgets/QMainWindow>
#include <qwt_plot_curve.h>
#include <QPushButton>
#include <qlayout.h>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>

#include <qwt_curve_fitter.h>

extern QVector<double> xval_;
extern QVector<double> yval_;
extern int splineSize;

class Slider;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    enum splineType
    {
        B_SPLINE,
        BEZIER_SPLINE
    };

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void saveInterCurvePts();
    void saveBasicCurvePts();
    void lineEditChanged();
    void sliderPointsNbrChanged( int );
    void spinBoxBasicPointsNbrChanged( int );
    void checkBoxShowCubicCurveClicked( int );
    void checkBoxShowBezierCurveClicked( int );
    void saveInterCurveCoeffs();
    void changeDecimalSeparCoeffs(QString str);

private:
    void tabCurveManagement();
    void timerEvent(QTimerEvent *);
    QVector<QwtPlotCurve*>    curves;
    QwtPlot         * m_main_plot;
    QLineEdit       * lineEdit;
    QSlider         * sliderPointsNbr;
    QHBoxLayout     * vSliderLayout;
    QVector<Slider*>    vectorSlider;
    QVector<QWidget*>   vecStretchWidg;
    QTabWidget      * tabWidget;
    QSpinBox        * spinBoxBasicPointsNbr;
    QWidget         * sliderWidget;
    bool            interpolParameterChanged;  // flag to replot curve only if nbr. of points changed
};

#endif // MAINWINDOW_H
