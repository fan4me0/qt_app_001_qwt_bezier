#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
    fstream in_curve;
    if(argv[1] != NULL)
    {
        in_curve.open(argv[1],ios::in);
    }
    else
    {
        cout << "Give me input data..." << endl;
        return (0);
    }
    fstream out_curve;
    fstream out_curve_excl;

    out_curve.open("RampRotorAlignCurve.h",ios::out);
    out_curve_excl.open("RampRotorAlignCurveExcel.txt",ios::out);

    stringstream   ssline;
    ssline.setf(ios::fixed,ios::floatfield);
    ssline.precision(5);

    string line;
    vector<string> align_curve;

    while(!in_curve.eof()) {
        getline(in_curve,line);
        istringstream is_my(line);
        float m_f;
        is_my >> m_f;
        ssline << m_f;
        align_curve.push_back(ssline.str());
        ssline.str("");                            // clear ssline
    }

    int tbl_indx = 0;
    for(auto indx = align_curve.begin(); (indx+1) != align_curve.end(); indx++, tbl_indx++)
    {
        out_curve << "(int16)(RMP_FINAL_ALIGN_DIRECT_CURRENT * (" << *indx << ")),\t\t// [" << tbl_indx << "]\n";
        out_curve_excl << *indx << "\n";
    }

    out_curve_excl.close();
    out_curve.close();
    in_curve.close();
    return 0;
}