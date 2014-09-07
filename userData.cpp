// C++ Standard includes
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
// Qwt includes
#include <qwt_curve_fitter.h>
// App includes
#include "mainwindow.h"
#include "userData.h"

using namespace std;

static void storeNewData();

static void storeNewData()
{
    fstream out_basicCurveNew;

    //open file for output
    out_basicCurveNew.open("settings_backup.ini",ios::out); // op_cl_0_1

    stringstream   ssline;
    ssline.setf(ios::fixed,ios::floatfield);
    ssline.precision(3);

    out_basicCurveNew << "[points]" << endl;
    for( int i = 0; i < xval_.size(); ++i )
    {
        ssline << yval_[i];
        out_basicCurveNew << ssline.str() << "  // point " << i << endl;
        ssline.str("");
    }

    out_basicCurveNew << "[number_of_points]" << endl;
    out_basicCurveNew << splineSize;

    out_basicCurveNew.close(); // op_cl_0_1
}

//----------------------------------------------------
// Load basic (initial) curve points controlled by the sliders
// from the "curve_point.ini" file
//----------------------------------------------------
bool loadSettings()
{
    enum {READ_POINTS = 1, READ_NBR_OF_POINTS = 2};
    string line;
    ifstream myfile ("settings_backup.ini",ios::in);
    if (myfile.is_open())
    {
        int i = 0;
        int sz = 0;
        int read = 0;

        while (! myfile.eof() )
        {
            getline (myfile,line);
            if(line == "[points]")
            {
                read = READ_POINTS;
                getline (myfile,line);
            }
            if(line == "[number_of_points]")
            {
                read = READ_NBR_OF_POINTS;
                getline (myfile,line);
            }

            switch (read)
            {
                case READ_POINTS:
                {
                    if((sz = line.size()) == 0)
                        break;
                    #if defined(__gnu_linux__)
                        //line.replace(line.find("."),1,","); // specific for gnu/linux
                    #endif
                        yval_.push_back( atof( line.c_str() ) );
                        xval_.push_back( i );
                    i++;
                    break;
                }
                case READ_NBR_OF_POINTS:
                {
                    if((sz = line.size()) == 0)
                        break;
                    splineSize = atoi(line.c_str());
                    break;
                }
                default:
                    break;
            } // switch
        }  // while()

        myfile.close();
        return false; // setting load OK
    }
    else
    {
        // if no "curve_point.ini" file present return false - default point values
        return true;
    }
    //----------------------------------------------------
    // todo: load some other data
    //----------------------------------------------------

}

void saveInterpolatedCurveCoeffs( const QVector<double> &yCoeff,
                                  const QVector<double> &aCoeff,
                                  const QVector<double> &bCoeff,
                                  const QVector<double> &cCoeff )
{
    ofstream out_curve_coeffs;
    out_curve_coeffs.open( "b_spline_coeffs.txt", ios::out );

    stringstream   ssline;
    ssline.setf(ios::fixed,ios::floatfield);
    ssline.precision(6);

    out_curve_coeffs << "//  y(x)  =    Y + A*(x)    +     B*(x*x)    +   C*(x*x*x)" << endl;
    out_curve_coeffs << "//  Y                A               B               C" << endl;
    for( int iter = 0 ; iter != aCoeff.size() ; ++iter )
    {
        ssline << yCoeff[iter] << "\t" << aCoeff[iter] << "\t"
               << bCoeff[iter] << "\t" << cCoeff[iter];

        out_curve_coeffs << ssline.str() << "\t" ;
        out_curve_coeffs << "// spline coefficients between basic points " << iter << " and " << iter+1 << endl;
        ssline.str("") ;
    }
    out_curve_coeffs.close();
}

void saveInterpolatedCurvePoints( QPolygonF &fittedPolygonF, MainWindow::splineType splineType )
{
    ofstream out_curve;

    switch (splineType)
    {
        case MainWindow::B_SPLINE:
        {
            out_curve.open("b_spline_samples.txt",ios::out);
            break;
        };
        case MainWindow::BEZIER_SPLINE:
        {
            out_curve.open("bezier_spline_samples.txt",ios::out);
            break;
        }
        default:
        {
            break;
        }
    }

    stringstream   ssline;
    ssline.setf(ios::fixed,ios::floatfield);
    ssline.precision(6);

    for(QVector<QPointF>::iterator indx = fittedPolygonF.begin(); (indx) != fittedPolygonF.end(); indx++ )
    {
        ssline << indx->y();
        out_curve << ssline.str() << "\n";
        ssline.str("") ;
    }
    out_curve.close();
}

void saveBasicCurvePoints()
{
    fstream out_basicCurveNew; // input/output
    ofstream out_basicCurveOrg;
    int sz;
    out_basicCurveNew.open("settings_backup.ini",ios::in); // op_0_0

    if (out_basicCurveNew.is_open())
    {   // IF_S_00
        out_basicCurveOrg.open("settings_backup_org.ini",ios::out); // op_1_0

        string line;
        while (! out_basicCurveNew.eof() )
        {
            getline (out_basicCurveNew,line);
            if((sz = line.size()) > 0)
            {
                out_basicCurveOrg << line << endl;
            }
            else
            {
                break; // do not save empty line
            }
        }
        out_basicCurveNew.close(); // cl_0_0
        out_basicCurveOrg.close(); // cl_1_0
        // store actual basic points
        storeNewData();
    }
    else
    {
        // file does not exist yet, creat it with the default data
        storeNewData();
    }  // IF_E_00
}
