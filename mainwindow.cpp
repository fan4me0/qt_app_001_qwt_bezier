// C++ Standard includes
#include <iterator>
#include <algorithm>
#include <vector>
// Qt includes
#include <QtGui>
#include <QPushButton>
#include <QSlider>
#include <qlayout.h>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>
#include "qlabel.h"
// Qwt includes
#include <qwt_symbol.h>
#include <qwt_curve_fitter.h>
#include <qwt_plot.h>
#include <qwt_spline.h>
#include <qwt_plot_grid.h>
#include <qwt_slider.h>
#include <qwt_abstract_slider.h>

// App includes
#include "mainwindow.h"
#include "sliders.h"
#include "userData.h"


#define BEZIER_

#if defined(BEZIER_)
    #include <qwt_bezier_spline.h>
#endif

using namespace std;


static const int sliderPointsNbrMaxLimit = 200000;
static const int sliderPointsNbrMinLimit = 10;    // defined by Qwt library in 'QwtSplineCurveFitter::setSplineSize'
static const int maxSliderCount = 19;

static const int initialKnotNumber = 12;
static const double initialKnotValues[initialKnotNumber] =
{
    0.000,  // point 0
    0.712,  // point 1
    0.000,  // point 2
    0.000,  // point 3
    0.913,  // point 4
    1.000,  // point 5
    1.000,  // point 6
    0.000,  // point 7
    0.000,  // point 8
    0.500,  // point 9
    0.500,  // point 10
    0.500   // point 11
};

int splineSize = initialKnotNumber;
QVector<double> xval_;
QVector<double> yval_;


MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    if( loadSettings() != false )
    {
        for( int i = 0; i < initialKnotNumber; ++i )
        {
            xval_.push_back( i );
            yval_.push_back( initialKnotValues[i] );
        }
    }

    //-----------------------------------------------------------------
    // 'Set points' tab widgets/layout
    //-----------------------------------------------------------------
    vSliderLayout = new QHBoxLayout( );

    for ( int i = 0 ; i < xval_.size(); ++i )
    {
        Slider * sl = new Slider( this );
        vectorSlider.push_back( sl );
        vSliderLayout->addWidget( vectorSlider[i], 1, Qt::AlignHCenter );
        if( i != ( xval_.size() - 1 ) )
        {
            vecStretchWidg.push_back( new QWidget );
            vSliderLayout->addWidget( vecStretchWidg[i], 1 );
        }
    }

    // creat widget for the sliders to add to tab widget
    QFrame * sliderWidget = new QFrame;
    sliderWidget->setLayout( vSliderLayout );
    sliderWidget->setFrameStyle( QFrame::Box );

    //-----------------------------------------------------------------
    // curve plot area
    //-----------------------------------------------------------------
    QString label;
    label.sprintf( "Example of different interpolation methods (blue:Natural cubic spline / red:Bezier spline )" );
    QwtText text(label);
    m_main_plot = new QwtPlot(text, parent);
    m_main_plot->setCanvasBackground(QColor( Qt::white ));
    m_main_plot->setAxisScale( QwtPlot::xBottom, 0, ( xval_.size() - 1 ) );
    m_main_plot->setAxisTitle( QwtPlot::xBottom, "knots [-]" );
    m_main_plot->setAxisScale( QwtPlot::yLeft, -0.3, 1.3 );
    m_main_plot->setAxisTitle( QwtPlot::yLeft, "amplitude" );
    m_main_plot->enableAxis(QwtPlot::yRight,true);
    m_main_plot->setAxisScale( QwtPlot::yRight, -0.3, 1.3 );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( QPen( Qt::black, 0.0, Qt::DotLine ) );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( m_main_plot );

    QwtPlotCurve * spline = new QwtPlotCurve;
    spline->setPen( QPen( QColor( Qt::blue ), 4 ) );
    spline->setStyle( QwtPlotCurve::Lines );
    spline->setCurveAttribute( QwtPlotCurve::Fitted, true );
    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse, QBrush( Qt::white ), QPen( Qt::black, 4 ), QSize( 8, 8 ) );
    spline->setSymbol(symbol);
    curves.push_back(spline);

    QwtSplineCurveFitter * curveFitter = new QwtSplineCurveFitter();
    curveFitter->setSplineSize( splineSize );
    curves[MainWindow::B_SPLINE]->setCurveFitter( curveFitter );


    curves[MainWindow::B_SPLINE]->setSamples(xval_,yval_);    // update basic points - 'setSamples()' does not provide deep copy
    curves[MainWindow::B_SPLINE]->attach(m_main_plot);

    // === Bezier ==============================================
#if defined(BEZIER_)
    spline = new QwtPlotCurve;
    spline->setPen( QPen( QColor( Qt::red ), 4 ) );
    spline->setStyle( QwtPlotCurve::Lines );
    spline->setCurveAttribute( QwtPlotCurve::Fitted, true );
    curves.push_back(spline);

    QwtBezierSplineCurveFitter * bezierCurveFitter = new QwtBezierSplineCurveFitter();
    bezierCurveFitter->setSplineSize( splineSize );
    curves[MainWindow::BEZIER_SPLINE]->setCurveFitter( bezierCurveFitter );

    curves[MainWindow::BEZIER_SPLINE]->setSamples(xval_,yval_);    // update basic points - 'setSamples()' does not provide deep copy
    curves[MainWindow::BEZIER_SPLINE]->attach(m_main_plot);
#endif

    //-----------------------------------------------------------------
    // 'Curve Management' tab widgets/layout
    //-----------------------------------------------------------------
    // Hlayout : curveManageLoutAddRemSlider --------------------------
    //-----------------------------------------------------------------
    QSpinBox * spinBoxBasicPointsNbr = new QSpinBox;
    spinBoxBasicPointsNbr->setRange( 3, maxSliderCount );      // at least 3 sliders
    spinBoxBasicPointsNbr->setValue( xval_.size() );
    QLabel * labelBasicPointsNbr = new QLabel("Number of knots:");
    connect( spinBoxBasicPointsNbr, SIGNAL( valueChanged(int) ), SLOT( spinBoxBasicPointsNbrChanged( int ) ) );

    QHBoxLayout * curveManageLoutAddRemSlider = new QHBoxLayout( );
    curveManageLoutAddRemSlider->addWidget( labelBasicPointsNbr,0,Qt::AlignTop );
    curveManageLoutAddRemSlider->addWidget( spinBoxBasicPointsNbr,0,Qt::AlignTop );
    QFrame *groupBox = new QFrame();
    groupBox->setFrameStyle( QFrame::StyledPanel );
    groupBox->setLayout( curveManageLoutAddRemSlider );

    //-----------------------------------------------------------------
    // Vlayout : curveManageLoutSecond --------------------------------
    //-----------------------------------------------------------------
    QCheckBox * checkBoxShowCurve = new QCheckBox;
    checkBoxShowCurve->setText("Show Natural cubic spline");
    checkBoxShowCurve->setCheckState(Qt::Checked);
    connect( checkBoxShowCurve, SIGNAL( stateChanged(int) ), SLOT( checkBoxShowCubicCurveClicked( int ) ) );

    QVBoxLayout * curveManageLoutSecond = new QVBoxLayout( );
    curveManageLoutSecond->addWidget(groupBox);
    curveManageLoutSecond->addWidget(checkBoxShowCurve);

    checkBoxShowCurve = new QCheckBox;
    checkBoxShowCurve->setText("Show Bezier spline");
    checkBoxShowCurve->setCheckState(Qt::Checked);
    connect( checkBoxShowCurve, SIGNAL( stateChanged(int) ), SLOT( checkBoxShowBezierCurveClicked( int ) ) );
    curveManageLoutSecond->addWidget(checkBoxShowCurve);

    //-----------------------------------------------------------------
    // Hlayout : curveManageLoutAddRemSaveData ------------------------
    //-----------------------------------------------------------------
    QHBoxLayout * curveManageLoutAddRemSaveData = new QHBoxLayout( );
    curveManageLoutAddRemSaveData->addLayout(curveManageLoutSecond,1);
    curveManageLoutAddRemSaveData->addLayout(new QVBoxLayout,10); // now dummy lout to get compact look

    //-----------------------------------------------------------------
    // Hlayout : curveManageLoutInterpolPts ---------------------------
    //-----------------------------------------------------------------
    // edit line changing the nbr. of interpolated points
    QLabel *labelPointsNbr = new QLabel("Number of spline samples :");
    QString ltext;
    ltext.setNum( splineSize );
    lineEdit = new QLineEdit(ltext);
    labelPointsNbr->setBuddy(lineEdit);
    connect(lineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(lineEditChanged()));

    // slider changing the nbr. of interpolated points
    sliderPointsNbr = new QSlider(Qt::Horizontal);
    sliderPointsNbr->setRange(sliderPointsNbrMinLimit,sliderPointsNbrMaxLimit);
    sliderPointsNbr->setTickPosition(QSlider::TicksBelow);
    sliderPointsNbr->setTickInterval(5000);
    sliderPointsNbr->setValue(splineSize);
    connect(sliderPointsNbr, SIGNAL(valueChanged(int)),this, SLOT(sliderPointsNbrChanged(int)));

    // assign to Hlayout
    QHBoxLayout *curveManageLoutInterpolPts = new QHBoxLayout();
    curveManageLoutInterpolPts->addWidget(labelPointsNbr);
    curveManageLoutInterpolPts->addWidget(lineEdit,1);
    curveManageLoutInterpolPts->addWidget(sliderPointsNbr,10);

    QFrame *qframeSetPoints = new QFrame();
    qframeSetPoints->setFrameStyle( QFrame::StyledPanel );
    qframeSetPoints->setLayout(curveManageLoutInterpolPts);

    //-----------------------------------------------------------------
    // Vlayout : curveManageLoutMain holding louts :
    // curveManageLoutAddRemSaveData,curveManageLoutInterpolPts
    //-----------------------------------------------------------------
    QVBoxLayout * curveManageLoutMain = new QVBoxLayout( );
    curveManageLoutMain->addLayout(curveManageLoutAddRemSaveData);
    curveManageLoutMain->addWidget(qframeSetPoints);

    QFrame * curveSettingsWidget = new QFrame;
    curveSettingsWidget->setLayout(curveManageLoutMain);
    curveSettingsWidget->setFrameStyle(QFrame::Box);

    //-----------------------------------------------------------------
    // 'Store data' tab widgets/layout
    //-----------------------------------------------------------------
    // Vlayout : storeDataTabLoutSavePoints ------------------------------
    //-----------------------------------------------------------------
    QVBoxLayout * storeDataTabLoutSavePoints = new QVBoxLayout( );
    // button: store interpolated curve
    QComboBox *floatSeparatorComboBox = new QComboBox;
    floatSeparatorComboBox->addItem("Decimal separator: dot   (e.g. 2.708)");
    floatSeparatorComboBox->addItem("Decimal separator: colon (e.g. 2,708)");
    QPushButton *buttonGenCurveData = new QPushButton("Store samples for all splines");
    connect( buttonGenCurveData, SIGNAL( clicked() ), SLOT( saveInterCurvePts() ) );
    QHBoxLayout * hLoutGenCurveData = new QHBoxLayout( );
    hLoutGenCurveData->addWidget(floatSeparatorComboBox,1);
    hLoutGenCurveData->addWidget(buttonGenCurveData,2);
    QFrame * genCurveDataFrame = new QFrame;
    genCurveDataFrame->setLayout(hLoutGenCurveData);
    genCurveDataFrame->setFrameStyle(QFrame::Box);
    storeDataTabLoutSavePoints->addWidget(genCurveDataFrame);

    // button: store interpolated curve coefficients
    QComboBox *floatSeparatorComboBoxPointCoeffs = new QComboBox;
    floatSeparatorComboBoxPointCoeffs->addItem("Decimal separator: dot   (e.g. 2.708)");
    floatSeparatorComboBoxPointCoeffs->addItem("Decimal separator: colon (e.g. 2,708)");
    connect(floatSeparatorComboBoxPointCoeffs, SIGNAL(activated(QString)),
                 SLOT(changeDecimalSeparCoeffs(QString)));
    QPushButton *buttonGenCurveDataCoeffs = new QPushButton("Store Natural cubic spline coefficients");
    connect( buttonGenCurveDataCoeffs, SIGNAL( clicked() ), SLOT( saveInterCurveCoeffs() ) );
    QHBoxLayout * hLoutGenCurveCoeffs = new QHBoxLayout( );
    hLoutGenCurveCoeffs->addWidget(floatSeparatorComboBoxPointCoeffs,1);
    hLoutGenCurveCoeffs->addWidget(buttonGenCurveDataCoeffs,2);
    QFrame * genPointsCoeffsFrame = new QFrame;
    genPointsCoeffsFrame->setLayout(hLoutGenCurveCoeffs);
    genPointsCoeffsFrame->setFrameStyle(QFrame::Box);
    storeDataTabLoutSavePoints->addWidget(genPointsCoeffsFrame);

    // button: store basic curve points
    QPushButton *buttonStoreBasicCurveData = new QPushButton("Store knots/spline samples");
    connect( buttonStoreBasicCurveData, SIGNAL( clicked() ), SLOT( saveBasicCurvePts() ) );
    storeDataTabLoutSavePoints->addWidget(buttonStoreBasicCurveData,1);
    storeDataTabLoutSavePoints->addWidget(new QFrame, 10);

    QHBoxLayout * hLoutMainStoreDataTab = new QHBoxLayout( );
    hLoutMainStoreDataTab->addLayout(storeDataTabLoutSavePoints,1);
    hLoutMainStoreDataTab->addLayout(new QVBoxLayout( ),10); // dummy place holder
    QFrame * curveStoreDataFrame = new QFrame;
    curveStoreDataFrame->setLayout(hLoutMainStoreDataTab);
    curveStoreDataFrame->setFrameStyle(QFrame::Box);

    //-----------------------------------------------------------------
    // set tab widget
    //-----------------------------------------------------------------
    tabWidget = new QTabWidget();
    tabWidget->addTab(sliderWidget, "Set Points");
    tabWidget->addTab(curveSettingsWidget, "Curve Management");
    tabWidget->addTab(curveStoreDataFrame, "Store data");

    //-----------------------------------------------------------------
    // set main layout
    //-----------------------------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout( this );
    mainLayout->addWidget(tabWidget,1);
    mainLayout->addWidget(m_main_plot,4);
}

// SIGNAL: edit line value changed -> nbr. of points in interpolated curve
void MainWindow::lineEditChanged()
{
    QString  valuePoints = lineEdit->text();
    splineSize = valuePoints.toInt();
    if(splineSize > sliderPointsNbrMaxLimit)
    {
        sliderPointsNbr->setValue(sliderPointsNbrMaxLimit);
        QString text;
        text.setNum( sliderPointsNbrMaxLimit );
        lineEdit->setText(text);
    }
    else
    {
        if(splineSize < sliderPointsNbrMinLimit)
        {
            QString text;
            text.setNum( sliderPointsNbrMinLimit );
            lineEdit->setText(text);
            sliderPointsNbr->setValue(splineSize);
        }
        else
        {
            // no limitation
            sliderPointsNbr->setValue(splineSize);
        }

    }
    interpolParameterChanged = true;
}

// SIGNAL: function to add/remove sliders via 'spinBoxBasicPointsNbr'
void MainWindow::spinBoxBasicPointsNbrChanged(int value)
{
    int temp = xval_.size() - value;
    interpolParameterChanged = true;
    if((temp > 1) || (temp < -1))
    {
        // request for more than 1 slider add/remove, not allowed
        return;
    }
    if( value < xval_.size() )
    {
        int vecSldrSz = vectorSlider.size();
        int vecQWdgSz = vecStretchWidg.size();
        Slider * pSlider = vectorSlider.at(vecSldrSz-1);
        QWidget * pQWidget = vecStretchWidg.at(vecQWdgSz-1);

        vSliderLayout->removeWidget(pSlider);
        vSliderLayout->removeWidget(pQWidget);

        delete pQWidget;
        vecStretchWidg.remove(vecQWdgSz-1);

        delete pSlider;
        vectorSlider.remove(vecSldrSz-1);

        xval_.pop_back();
        yval_.pop_back();
        m_main_plot->setAxisScale( QwtPlot::xBottom, 0, ( xval_.size() - 1 ) );

        curves[MainWindow::B_SPLINE]->setSamples( xval_, yval_ );    // update basic points - 'setSamples()' does not provide deep copy
#if defined(BEZIER_)
        curves[MainWindow::BEZIER_SPLINE]->setSamples( xval_, yval_ );    // update basic points - 'setSamples()' does not provide deep copy
#endif
        m_main_plot->replot();
    }
    else
    {
        if( value > xval_.size() )
        {
            xval_.push_back( xval_.size() );
            yval_.push_back( 0.5 );
            Slider * pSlider = new Slider( this );
            QWidget * pQWidget = new QWidget;

            vectorSlider.push_back(pSlider);
            vecStretchWidg.push_back(pQWidget);
            vSliderLayout->addWidget(pQWidget,1);
            vSliderLayout->addWidget(pSlider,1);

            m_main_plot->setAxisScale( QwtPlot::xBottom, 0, ( xval_.size() - 1 ) );

            curves[MainWindow::B_SPLINE]->setSamples( xval_, yval_ );    // update basic points - 'setSamples()' does not provide deep copy
#if defined(BEZIER_)
            curves[MainWindow::BEZIER_SPLINE]->setSamples( xval_, yval_ );    // update basic points - 'setSamples()' does not provide deep copy
#endif
            m_main_plot->replot();
        }
    }
}

// SIGNAL: slider position changed -> nbr. of points in interpolated curve
void MainWindow::sliderPointsNbrChanged(int value)
{
    splineSize = value;
    QString text;
    text.setNum( value );
    lineEdit->setText(text);
    static_cast<QwtSplineCurveFitter*>(curves[MainWindow::B_SPLINE]->curveFitter())->setSplineSize( splineSize );
#if defined(BEZIER_)
    static_cast<QwtBezierSplineCurveFitter*>(curves[MainWindow::BEZIER_SPLINE]->curveFitter())->setSplineSize( splineSize );
#endif
}

// SIGNAL: todo
void MainWindow::checkBoxShowCubicCurveClicked(int value)
{
    if(Qt::Checked == value)
    {
        curves[MainWindow::B_SPLINE]->setStyle( QwtPlotCurve::Lines );   // show the curve
    }
    else
    {
        curves[MainWindow::B_SPLINE]->setStyle( QwtPlotCurve::Dots );    // hide the curve
    }
    m_main_plot->replot();
}

void MainWindow::checkBoxShowBezierCurveClicked(int value)
{
#if defined(BEZIER_)
    if(Qt::Checked == value)
    {
        curves[MainWindow::BEZIER_SPLINE]->setStyle( QwtPlotCurve::Lines );   // show the curve
    }
    else
    {
        curves[MainWindow::BEZIER_SPLINE]->setStyle( QwtPlotCurve::Dots );    // hide the curve
    }
    m_main_plot->replot();
#endif
}

// SIGNAL: QWidget implicit timer signal - replot curve if parameter changed
void MainWindow::timerEvent(QTimerEvent * timerId)
{
    // Q_A: how to react with reploting of the curve only if there was slider change
    //      how to communicate between objects (apart from the global flag) so
    //      it will be event driven
    bool sliderChanged = false;
    for( QVector< Slider* >::iterator iter = vectorSlider.begin();
         iter != vectorSlider.end(); ++iter )
    {
        if( (*iter)->sliderPositionChanged() == true )
            sliderChanged = true;
    }

    if((sliderChanged == true) || (interpolParameterChanged == true))
    {
        interpolParameterChanged = false;

        curves[MainWindow::B_SPLINE]->setSamples(xval_,yval_);   // update basic points - 'setSamples()' does not provide deep copy
#if defined(BEZIER_)
        curves[MainWindow::BEZIER_SPLINE]->setSamples(xval_,yval_);   // update basic points - 'setSamples()' does not provide deep copy
#endif
        m_main_plot->replot();
    }
}

// SIGNAL: save interpolated curve  that connects basic pointsin 'RampRotorAlignCurve.h' file
void MainWindow::saveInterCurvePts()
{
    QPolygonF myPolygonF;
    QPolygonF fittedPolygonF;
    for ( int i = 0 ; i < xval_.size(); ++i )
    {
        myPolygonF.push_back( QPointF( static_cast<qreal>( xval_[i] ), static_cast<qreal>( yval_[i] ) ) );
    }

    fittedPolygonF = static_cast<QwtSplineCurveFitter*>(curves[MainWindow::B_SPLINE]->curveFitter())->fitCurve( myPolygonF );
    // save the data into text file
    saveInterpolatedCurvePoints(fittedPolygonF, MainWindow::B_SPLINE );

#if defined(BEZIER_)
    fittedPolygonF = static_cast<QwtBezierSplineCurveFitter*>(curves[MainWindow::BEZIER_SPLINE]->curveFitter())->fitCurve( myPolygonF );
    // save the data into text file
    saveInterpolatedCurvePoints(fittedPolygonF, MainWindow::BEZIER_SPLINE );
#endif
}

// SIGNAL: save basic curve points in "settings_backup.ini"
void MainWindow::saveBasicCurvePts()
{
    // save the data into text file
    saveBasicCurvePoints();
}

// SIGNAL: todo
void MainWindow::saveInterCurveCoeffs()
{
    QPolygonF myPolygonF;
    for ( int i = 0 ; i < xval_.size(); ++i )
    {
        myPolygonF.push_back( QPointF( static_cast<qreal>( xval_[i] ), static_cast<qreal>( yval_[i] ) ) );
    }

    QwtSplineCurveFitter * ptr_bspline = static_cast<QwtSplineCurveFitter*>(curves[MainWindow::B_SPLINE]->curveFitter());
    ptr_bspline->spline().setPoints(myPolygonF);

    saveInterpolatedCurveCoeffs( yval_,
                                 ptr_bspline->spline().coefficientsA(),
                                 ptr_bspline->spline().coefficientsB(),
                                 ptr_bspline->spline().coefficientsC() );
    ptr_bspline->spline().reset();
}

// TODO: finish usage of 'floatSeparator' to generate files with requested separator
void MainWindow::changeDecimalSeparCoeffs(QString str)
{
    enum { FLOAT_SEPARATOR_DOT, FLOAT_SEPARATOR_COLON};
    char floatSeparator = FLOAT_SEPARATOR_DOT;
    string std_str = str.toStdString();
    string s_colon("colon");
    string s_dot("dot");

    string::size_type i = std_str.find(s_colon);
    if(i == string::npos)
    {
        string::size_type i = std_str.find(s_dot);
        if(i == string::npos)
        {
            qDebug("Uuuups - sth. is wrong...");
        }
        else
        {
            qDebug("Float separator DOT");
            floatSeparator = FLOAT_SEPARATOR_DOT;
        }
    }
    else
    {
        qDebug("Float separator COLON");
        floatSeparator = FLOAT_SEPARATOR_COLON;
    }

}

MainWindow::~MainWindow()
{
    for(QVector<QwtPlotCurve*>::iterator iter = curves.begin(); iter != curves.end(); ++iter)
    {
        delete *iter;
    }
    curves.clear();

}
