#ifndef SLIDERS_H
#define SLIDERS_H

#include <qwidget.h>
#include <qwt_slider.h>
#include "mainwindow.h"
#include "userData.h"

class QLabel;
class QLayout;

class Slider: public QWidget
{
    Q_OBJECT

public:
    static int  sliderNbr;
    Slider( QWidget *parent );
    bool sliderPositionChanged();
    ~Slider();

private Q_SLOTS:
    void setNum( double v );

private:
    int         sliderIndex;
    QwtSlider   * createSlider( QWidget * ) const;
    bool        positionChanged;
    QwtSlider   * d_slider;
    QLabel      * d_label;
};

#endif  // SLIDERS_H
