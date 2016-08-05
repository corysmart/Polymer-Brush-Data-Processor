/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                         //
//  main.cpp                                                                                               //
//  Ober Group Data Processor                                                                              //
//                                                                                                         //
//  Created by Cory Bethrant on 7/11/16.                                                                   //
//  Copyright © 2016 Cornell University. All rights reserved.                                              //
//                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <new>

using namespace std;



int main(){
    
    // Initialize Variables //
    int numberoflines = 0;
    int numberoffiles = 0;
    int slopeNeutralCount = 0;
    int slopeNegativeCount = 0;
    int slopePositiveCount = 0;
    int rightborderPOSint = 0;
    int leftborderPOSint = 0;
    int patternamount = 0;
    int profileamount = 0;
    int j = 0;
    int k = -1;
    int d = 0;
    string unitType;
    string pattern;
    string sample;
    string customsensitivity;
    string brlg = "------------------------------------------------------------";
    string br = "-------------------------------------------";
    bool rightborder = false;
    bool onPattern = false;
    bool patternDone = false;
    bool TrendNegative = false;
    bool TrendPositive = false;
    
    bool * slopepositiveFLAG;
    bool * slopenegativeFLAG;
    string * filename;
    double * x;
    double * y;
    double * slope;
    double * firstDery;
    double * secDery;
    
    double leftborderPOS = 0.0;
    double rightborderPOS = 0.0;
    double baseline = 0.0;
    double height = 0.0;
    double width = 0.0;
    double slopeTotal = 0.0;
    double slopeAVG = 0.0;
    double integral = 0.0;
    double sensitivity = 4;

    
    // Print Copyright Info //
    cout << brlg << '\n' <<  "Ober Group Data Processor for Polymer Brushes" << '\n' << "Created by Cory Bethrant on 7/11/16." << '\n' << "Copyright © 2016 Cornell University. All rights reserved." << '\n' << brlg << '\n' << '\n';
    
    // Datafile Amount Prompt //
    cout << "Please enter the amount of pattern lines you wish to be processed: ";
    cin >> patternamount;
    cout << "Please enter the amount of profiles for each pattern line (1/2/4/6/8): ";
    cin >> profileamount;
    if (profileamount > 8){
        cout << "Profile Amount is Too High! Please reenter the amount of profiles for each pattern line (Max is 8): ";
        cin >> profileamount;
    }
    numberoffiles = patternamount * profileamount;
    filename = new string[numberoffiles];
    
    // Sensitivity Prompt //
    cout << "Would you like a custom sensitivity(Y/N)? (Default is 4)";
    cin >> customsensitivity;
    if (customsensitivity == "y" || customsensitivity == "Y" || customsensitivity == "Yes" || customsensitivity == "YES" || customsensitivity == "yes"){
        cout << "Enter custom sensitivity: " << '\n';
        cin >> sensitivity;
           }
    
    // Find File Names and Initiate Excel File
    cout << "Please enter the sample name: ";
    cin >> sample;
    for (int w = 0; w < patternamount; w++){
        if (w == 0) cout << "Please enter the name of the line you would like to process: ";
        else cout << "Please enter the name of the next line you would like to process (If you would like to go back just type a b/B): ";
        cin >> pattern;
        if (pattern == "b" || pattern == "B"){
            w--;
            w--;
        }
        switch (profileamount) {
            case 1: filename[w] = sample + "_" + pattern + "0000";
                break;
            case 2: filename[(w*2)] = sample + "_" + pattern + "0000";
                filename[(w*2)+1] = sample + "_" + pattern + "0001";
                break;
            case 4: filename[(w*4)] = sample + "_" + pattern + "0000";
                filename[(w*4)+1] = sample + "_" + pattern + "0000(2)";
                filename[(w*4)+2] = sample + "_" + pattern + "0001";
                filename[(w*4)+3] = sample + "_" + pattern + "0001(2)";
                break;
            case 6: filename[(w*6)] = sample + "_" + pattern + "0000";
                filename[(w*6)+1] = sample + "_" + pattern + "0000(2)";
                filename[(w*6)+2] = sample + "_" + pattern + "0000(3)";
                filename[(w*6)+3] = sample + "_" + pattern + "0001";
                filename[(w*6)+4] = sample + "_" + pattern + "0001(2)";
                filename[(w*6)+5] = sample + "_" + pattern + "0001(3)";
                break;
            case 8: filename[(w*8)] = sample + "_" + pattern + "0000";
                filename[(w*8)+1] = sample + "_" + pattern + "0000(2)";
                filename[(w*8)+2] = sample + "_" + pattern + "0000(3)";
                filename[(w*8)+3] = sample + "_" + pattern + "0000(4)";
                filename[(w*8)+4] = sample + "_" + pattern + "0001(1)";
                filename[(w*8)+5] = sample + "_" + pattern + "0001(2)";
                filename[(w*8)+6] = sample + "_" + pattern + "0001(3)";
                filename[(w*8)+7] = sample + "_" + pattern + "0001(4)";
                break;
        }
    }
    ofstream MyExcelFile;
    MyExcelFile.open("Output.csv");
    MyExcelFile << "Pattern Line, Width, Height, Integral" << endl;
    // Begin Data Proccess //
    cout << "Press Enter to begin processing data..." << '\n' << '\n';
    cin.get();
    
    // Find size of datafile //
    while (d < numberoffiles){
        ifstream f(filename[d]);
        string line;
        for (int i = 0; getline(f, line); i++){
            numberoflines = i;
        }
        cout << "Filename: " << filename[d] << '\n';
        cout << "Length of Gwyddion data file: " << numberoflines << '\n' << '\n';
        f.clear();
        f.seekg(0);
        // Allocate memory //
        slopepositiveFLAG = new bool[numberoflines];
        slopenegativeFLAG = new bool[numberoflines];
        x = new double[numberoflines];
        y = new double[numberoflines];
        slope = new double[numberoflines];
        firstDery = new double[numberoflines];
        secDery = new double[numberoflines];
        if (slopepositiveFLAG == nullptr || slopenegativeFLAG == nullptr || slope == nullptr || firstDery == nullptr || secDery == nullptr || x == nullptr || y == nullptr){
            cout << "Error: memory could not be allocated" << '\n' << "Program Terminated. Email Cory Bethrant @ cory.bethrant@gmail." << '\n' << '\n' << "Press Enter to Close Program."<<'\n';
            delete[] filename;
            delete[] x;
            delete[] y;
            delete[] slope;
            delete[] firstDery;
            delete[] secDery;
            delete[] slopepositiveFLAG;
            delete[] slopenegativeFLAG;
            cin.get();
            return 0;
        }
        
        else
            
            // Unit type determination based on filename string //
            unitType = "m";
        
        
        
        // Import Data //
        
        for (int i = 0; i <= numberoflines; i++){
            f >> x[i] >> y[i];
        }
        
        for (int i = 0; i < numberoflines; i++){
            
            // Perform Calculations //
            slopepositiveFLAG[i] = false;
            slopenegativeFLAG[i] = false;
            slope[i] = ((y[i+1]-y[i])/(x[i+1]-x[i]));
            slopeTotal += slope[i];
            slopeAVG = slopeTotal / 5;
            if ((i+1) % 5 == 0){
                
                // Calculate Data Chunk Information //
                slopeAVG = (slopeTotal/(i+1));
                slopeTotal = 0;
            }
            
            // Print Current Data Set Values and Set Flags //
            cout << "Data Set " << i+1 << '\n' << "x1:  " << x[i] << unitType << '\t' << "y1:  " << y[i] << unitType << '\n' << "x2:  " << x[i+1] << unitType << '\t' << "y2:  " << y[i+1] << unitType << '\n';
            
            
            if (x[i+1] == y[i+1] && x[i] == y[i]){
                cout << "The two data points are the same." << '\n' << '\n';
            }
            
            else if ((y[i+1]-y[i])==0){
                cout << "The slope of Data Set " << i+1 << " is Undefined." << '\n' << '\n';
            }
            
            else if (slope[i] > 0.03){
                cout << "The slope of Data Set " << i+1 << " is Positive." << '\n';
                slopepositiveFLAG[i] = true;
                cout << "The slope of Data Set " << i+1 << " equals: " << slope[i]<< '\n' << '\n';
            }
            
            else if (slope[i] < -0.03){
                cout << "The slope of Data Set " << i+1 << " is Negative." << '\n';
                slopenegativeFLAG[i] = true;
                cout << "The slope of Data Set " << i+1 << " equals: " << slope[i]<< '\n' << '\n';
            }
            
            else {
                cout << "The slope of Data Set " << i+1 << " is Neutral." << '\n';
                slopepositiveFLAG[i] = false;
                slopenegativeFLAG[i] = false;
                cout << "The slope of Data Set " << i+1 << " equals: " << slope[i]<< '\n' << '\n';
            }
            
            // Determine if Pattern Border Has Been Reached //
            if (slope[i] > sensitivity && onPattern == false && patternDone == false){
                cout << "Left Pattern Border Reached." << '\n' << '\n';
                for (int f = 1; f < numberoflines; f++){
                    if (slopepositiveFLAG[i-f] == false && slopepositiveFLAG[i-f-1] == false){
                        break;
                    }
                    if (slopepositiveFLAG[i-f] == true){
                        j++;
                    }
                }
                leftborderPOS = x[i-j-1];
                leftborderPOSint = i - j - 1;
                onPattern = true;
                slopePositiveCount = 0;
                slopeNegativeCount = 0;
                slopeNeutralCount = 0;
                j = 0;
            }
            
            
            if (slope[i] < -sensitivity && onPattern == true && patternDone == false && rightborder == false){
                cout << "Right Pattern Border Reached." << '\n' << '\n';
                rightborder = true;
                rightborderPOS = x[i];
                patternDone = true;
                slopePositiveCount = 0;
                slopeNegativeCount = 0;
                slopeNeutralCount = 0;
            }
            if (onPattern == true && rightborder == true){
                if (slopenegativeFLAG[i] == true){
                    ;
                }
                else {
                    onPattern = false;
                    rightborderPOS = 0;
                    rightborderPOS = x[i];
                    rightborderPOSint = i;
                    k = -1;
                }
            }
            
            // Calculate Derivatives //
            
            double t = 10^-7;
            
            firstDery[i] = (   4.0 / 3.0 * (y[i + 1] - y[i - 1]) / (2.0 * t)
                            - 1.0 / 3.0 * (y[i + 2] - y[i - 2]) / (4.0 * t) );
            
            secDery[i] = (   4.0 / 3.0 * (firstDery[i + 1] - firstDery[i - 1]) / (2.0 * t)
                          - 1.0 / 3.0 * (firstDery[i + 2] - firstDery[i - 2]) / (4.0 * t) );
            
            
            
            // Print Derivative Information //
            if (i < 2 || i > numberoflines - 2){
                cout << "First Derivative can't be calculated currently. " << '\n';
            }
            else {
                cout << "First Derivative is: " << firstDery[i] << '\n';
            }
            if (i < 4 || i > numberoflines - 4){
                cout << "Second Derivative can't be calculated currently. " << '\n' << '\n' << '\n';
            }
            else {
                cout << "Second Derivative is: " << secDery[i] << '\n' << '\n' << '\n' ;
            }
            
            // Determine Data Trend for Data Chunk //
            if ((i+1) % 5 == 0 && slopeAVG > 0.5 && i > 0){
                cout << br << '\n' << "Data Trend for last 5 points is Positive." << '\n' << "Average   Slope: " << slopeAVG << '\n' << br << '\n' << '\n';
                TrendPositive = true;
                if (slopeAVG > 0.5){
                    slopePositiveCount++ ;
                    if (slopeAVG > 2){
                        slopePositiveCount = 3;
                    }
                }
                else if (slopeAVG < -0.5){
                    slopeNegativeCount++ ;
                    if (slopeAVG > 2){
                        slopeNegativeCount = 3;
                    }
                }
                else {
                    slopeNeutralCount++ ;
                    if (slopeAVG > 2){
                        slopeNeutralCount = 3;
                    }
                }
                slopeAVG = 0;
            }
            
            else if ((i+1) % 5 == 0 && slopeAVG < -0.5 && i > 0) {
                cout << br << '\n' << "Data Trend for last 5 points is Negative." << '\n' << "Average Slope: " << slopeAVG << '\n' << br << '\n' << '\n';
                slopeTotal = 0;
                TrendNegative = true;
                if (slopeAVG > 0.5){
                    slopePositiveCount++ ;
                    if (slopeAVG > 2){
                        slopePositiveCount = 3;
                    }
                }
                else if (slopeAVG < -0.5){
                    slopeNegativeCount++ ;
                    if (slopeAVG < -2){
                        slopeNegativeCount = 3;
                    }
                }
                else {
                    slopeNeutralCount++ ;
                    if (slopeAVG > 2){
                        slopeNeutralCount = 3;
                    }
                }
                slopeAVG = 0;
            }
            else if ((i+1)% 5 == 0 && i > 0){
                cout << br << '\n' << "Data Trend for last 5 points is Neutral." << '\n' << "Average Slope: " << slopeAVG << '\n' << br << '\n' << '\n';
                slopeTotal = 0;
                if (slopeAVG > 0.5){
                    slopePositiveCount++ ;
                    if (slopeAVG > 2){
                        slopePositiveCount = 3;
                    }
                }
                else if (slopeAVG < -0.5){
                    slopeNegativeCount++ ;
                    if (slopeAVG > 2){
                        slopeNegativeCount = 3;
                    }
                }
                else {
                    slopeNeutralCount++ ;
                    if (slopeAVG > 2){
                        slopeNeutralCount = 3;
                    }
                }
                slopeAVG = 0;
            }
            
        }
        
        double max = 0;
        
        for(int i = leftborderPOSint; i < rightborderPOSint; i++){
            if(y[i] > max)
                max = y[i];
            // End of Data Loop //
        }
        
        baseline = (y[leftborderPOSint] + y[rightborderPOSint])/2;
        
        // Find Integral //
        integral = 0.0;
        for(int i = leftborderPOSint; i <= rightborderPOSint; i++){
            integral += ((((y[i]+y[i+1])/2)-baseline)*(x[i+1]-x[i]));
        }
        
        // Final Calculations //
        height = max - baseline;
        width = rightborderPOS - leftborderPOS;
        
        // Error Catching (If error is detected line is rescanned with lower sensitivity) //
        if (width <=0 || integral <= 0 || height <= 0 || leftborderPOSint <= 0){
            patternDone = false;
            rightborder = false;
            onPattern = false;
            patternDone = false;
            TrendNegative = false;
            TrendPositive = false;
            leftborderPOS = 0.0;
            rightborderPOS = 0.0;
            baseline = 0.0;
            height = 0.0;
            width = 0.0;
            slopeTotal = 0.0;
            slopeAVG = 0.0;
            integral = 0.0;
            rightborderPOSint = 0;
            leftborderPOSint = 0;
            j = 0;
            k = -1;
            for(int q = 2; q < numberoflines; q++){
                for(int v = 1; (width <=0 || integral <= 0 || height <= 0 || leftborderPOSint <= 0) && v < (sensitivity / .0001); v++){
                    if (slope[q] > (sensitivity - (v * .0001)) && onPattern == false && patternDone == false){
                        for (int f = 1; f < numberoflines; f++){
                            if (slopepositiveFLAG[q-f] == false){
                                break;
                            }
                            if (slopepositiveFLAG[q-f] == true){
                                j++;
                            }
                            leftborderPOS = x[q-j-1];
                            leftborderPOSint = q - j - 1;
                            onPattern = true;
                            slopePositiveCount = 0;
                            slopeNegativeCount = 0;
                            slopeNeutralCount = 0;
                            j = 0;
                        }
                    }
                }
            }
            
            for(int q = numberoflines-2; q >= 0; q--){
                for(int v = 1; (width <=0 || integral <= 0 || height <= 0 || rightborderPOS == 0) && v < (sensitivity / .0001); v++){
                    k = -1;
                    slopePositiveCount = 0;
                    slopeNegativeCount = 0;
                    slopeNeutralCount = 0;
                    if (slope[q] < -(sensitivity - (v * .0001)) && onPattern == true && patternDone == false && rightborder == false){
                        rightborder = true;
                        rightborderPOS = x[q];
                        if (onPattern == true && rightborder == true){
                            if (slopenegativeFLAG[q] == true){
                                ;
                            }
                            else {
                                onPattern = false;
                                patternDone = true;
                                rightborderPOS = 0;
                                rightborderPOS = x[q];
                                rightborderPOSint = q;
                                k = -1;
                            }
                        }
                        rightborderPOS = 0;
                        rightborderPOS = x[q];
                        rightborderPOSint = q;
                        k = -1;
                        slopePositiveCount = 0;
                        slopeNegativeCount = 0;
                        slopeNeutralCount = 0;
                    }
                    
                    for(int i = leftborderPOSint; i < rightborderPOSint; i++){
                        if(y[i] > max)
                            max = y[i];
                        // End of Data Loop //
                    }
                }
                
                baseline = (y[leftborderPOSint] + y[rightborderPOSint])/2;
                if (leftborderPOSint == 0){
                    leftborderPOSint = 1;
                }
                // Find Integral //
                for(int i = leftborderPOSint; i <= rightborderPOSint; i++){
                    integral += ((((y[i]+y[i+1])/2)-baseline)*(x[i+1]-x[i]));
                }
                
                // Final Calculations //
                height = max - baseline;
                width = rightborderPOS - leftborderPOS;
            }
        }
        // Find Integral //
        integral = 0.0;
        for(int i = leftborderPOSint; i <= rightborderPOSint; i++){
            integral += ((((y[i]+y[i+1])/2)-baseline)*(x[i+1]-x[i]));
        }
        // Output //
        cout << brlg << '\n' << "Final Statistics for Pattern: " << filename[d] << '\n' << "Left Border: x = " << leftborderPOS << unitType << '\n' << "Right Border: x = " << rightborderPOS << unitType << '\n' << '\n' << "Baseline Value: y = " << baseline << unitType <<'\n' << "Max: y = " << max << unitType << '\n' << "Width: x = " << width << unitType << '\n' << "Height: y = " << height << unitType << '\n' << "Integral of Pattern is: " << integral << unitType << '\n' << '\n';
        
        // Output Data to Excel File //
        MyExcelFile << filename[d] << "," << width << "," << height << "," << integral << endl;
        
        // Reset Loop //
        patternDone = false;
        rightborder = false;
        onPattern = false;
        patternDone = false;
        TrendNegative = false;
        TrendPositive = false;
        leftborderPOS = 0.0;
        rightborderPOS = 0.0;
        baseline = 0.0;
        height = 0.0;
        width = 0.0;
        slopeTotal = 0.0;
        slopeAVG = 0.0;
        integral = 0.0;
        numberoflines = 0;
        slopeNeutralCount = 0;
        slopeNegativeCount = 0;
        slopePositiveCount = 0;
        rightborderPOSint = 0;
        leftborderPOSint = 0;
        j = 0;
        k = -1;
        f.clear();
        f.seekg(0);
        f.close();
        d++;
    }
    
    MyExcelFile.close();
    
    // Delete All Dynamically Allocated Memory //
    
    delete[] filename;
    cout << "Thanks for using this program :)" << '\n' << '\n' << "Press Enter to Close...";
    cin.get();
    
    return 0;
    
}
