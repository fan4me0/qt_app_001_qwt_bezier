// Qt includes
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwt_slider.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <qwt_scale_draw.h>
// Qwt includes
#include "sliders.h"
#include "mainwindow.h"
#include "userData.h"

int Slider::sliderNbr = 0;

Slider::Slider( QWidget *parent ): QWidget( parent )
{
    sliderIndex = sliderNbr;
    sliderNbr++;
    d_slider = createSlider( this );

    d_label = new QLabel( "0", this );
    d_label->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    d_label->setFixedWidth( d_label->fontMetrics().width( "10000.9" ) );
    d_label->setFixedHeight(10);
    connect( d_slider, SIGNAL( valueChanged( double ) ), SLOT( setNum( double ) ) );

    QBoxLayout *layout;
    layout = new QVBoxLayout( this );
    layout->addWidget( d_slider );
    layout->addWidget( d_label );
    layout->setMargin(0);

    d_slider->setValue( yval_[sliderIndex] );
    emit setNum( yval_[sliderIndex] );   // set int value of display
}

Slider::~Slider()
{
    sliderNbr--;
}
#include <QSize>
QwtSlider *Slider::createSlider( QWidget *parent ) const
{
    QwtSlider *slider = NULL;

    slider = new QwtSlider( Qt::Vertical, parent );
    slider->setTrough(true);
    slider->setScalePosition( QwtSlider::TrailingScale );
    slider->setScale(QwtScaleDiv( 0.0, 1.0 ));
    slider->setTotalSteps(1000);
    slider->setHandleSize( QSize(15, 12) );
    slider->setMaximumHeight(120);
    slider->setScaleMaxMinor( 10 );

    return slider;
}

void Slider::setNum( double v )
{
    QString text;
    text.setNum( v, 'f', 3 );

    d_label->setText( text );
    yval_[sliderIndex] = v;
    positionChanged = true;
}

bool Slider::sliderPositionChanged( void )
{
    bool temp = positionChanged;
    positionChanged = false;
    return temp;
}
