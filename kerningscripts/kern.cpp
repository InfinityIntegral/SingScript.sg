#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <iostream>
#include <queue>
#include <cmath>
#include <algorithm>
#include <limits>

struct Point{
    double x;
    double y;
    Point(){
        x = 0.0;
        y = 0.0;
    }
    Point(double x, double y){
        (*this).x = x;
        (*this).y = y;
    }
};

struct BezierCurve{
    Point point1;
    Point point2;
    Point point3;
    Point point4;
    BezierCurve(){
        point1 = Point();
        point2 = Point();
        point3 = Point();
        point4 = Point();
    }
    BezierCurve(Point p1, Point p2, Point p3, Point p4){
        (*this).point1 = p1;
        (*this).point2 = p2;
        (*this).point3 = p3;
        (*this).point4 = p4;
    }
};

struct Line{
    Point point1;
    Point point2;
    Line(){
        point1 = Point();
        point2 = Point();
    }
    Line(Point p1, Point p2){
        (*this).point1 = p1;
        (*this).point2 = p2;
    }
};

struct Glyph{
    static constexpr int verticalSampleSize = 1000;
    int unicode;
    double width;
    std::array<double, Glyph::verticalSampleSize>* leftBound;
    std::array<double, Glyph::verticalSampleSize>* rightBound;
    std::vector<BezierCurve>* curves;
    std::vector<Line>* rasterisedCurves;
    Glyph(){
        unicode = -1;
        width = 0;
        leftBound = new std::array<double, Glyph::verticalSampleSize>();
        rightBound = new std::array<double, Glyph::verticalSampleSize>();
        curves = new std::vector<BezierCurve>();
        rasterisedCurves = new std::vector<Line>();
    }
    Glyph(int c, double w){
        unicode = c;
        width = w;
        leftBound = new std::array<double, Glyph::verticalSampleSize>();
        rightBound = new std::array<double, Glyph::verticalSampleSize>();
        curves = new std::vector<BezierCurve>();
        rasterisedCurves = new std::vector<Line>();
    }
};

struct Font{
    static constexpr int minimumUnicode = 33;
    static constexpr int maximumUnicode = 126;
    static constexpr int glyphCount = Font::maximumUnicode - Font::minimumUnicode + 1;
    static constexpr double flatnessBound = 0.1;
    static constexpr double descentSize = 300.0f;
    static constexpr double ascentSize = 600.0f;
    static constexpr double emSize = Font::ascentSize + Font::descentSize;
    std::array<Glyph, Font::glyphCount>* glyphs;
    Font(){
        glyphs = new std::array<Glyph, Font::glyphCount>();
    }
};

double distanceSquaredToLine(Point p1, Point p2, Point x){
    Point ap = Point(x.x - p1.x, x.y - p1.y);
    Point ab = Point(p2.x - p1.x, p2.y - p1.y);
    double distAB = ab.x * ab.x + ab.y * ab.y;
    if(std::abs(distAB) < Font::flatnessBound){return 0.0;}
    double t = (ap.x * ab.x + ap.y * ab.y) / distAB;
    if(t < 0.0){t = 0.0;}
    else if(t > 1.0){t = 1.0;}
    Point nearestPoint = Point(p1.x + t * ab.x, p1.y + t * ab.y);
    return (x.x - nearestPoint.x) * (x.x - nearestPoint.x) + (x.y - nearestPoint.y) * (x.y - nearestPoint.y);
}

bool curveIsFlat(BezierCurve c){
    if(distanceSquaredToLine(c.point1, c.point4, c.point2) < Font::flatnessBound * Font::flatnessBound && distanceSquaredToLine(c.point1, c.point4, c.point3) < Font::flatnessBound * Font::flatnessBound){return true;}
    return false;
}

double normaliseY(double y){
    return ((y + Font::descentSize) / Font::emSize * static_cast<double>(Glyph::verticalSampleSize));
}

double unnormaliseY(double y){
    return (y / static_cast<double>(Glyph::verticalSampleSize) * Font::emSize - Font::descentSize);
}

int main(){
    Font* myFont = new Font();
    
    {
        std::ifstream inFile("glyphsdata.txt");
        Glyph nullGlyph(0, 0);
        Glyph currentGlyph = nullGlyph;
        Point nullPoint(0.0, 0.0);
        Point contourStart = nullPoint;
        BezierCurve nullCurve(nullPoint, nullPoint, nullPoint, nullPoint);
        BezierCurve currentCurve = nullCurve;
        int lastFilledPointIndex = 0;
        while(true){
            std::string nextData;
            inFile >> nextData;
            if(nextData == "EXTRACTIONCOMPLETE"){
                break;
            }
            else if(nextData == "ENDGLYPH"){
                (*(*myFont).glyphs).at(currentGlyph.unicode - Font::minimumUnicode) = currentGlyph;
                currentGlyph = nullGlyph;
            }
            else if(nextData == "END"){
                if(lastFilledPointIndex == 3){
                    currentCurve.point4 = contourStart;
                    (*currentGlyph.curves).push_back(currentCurve);
                    currentCurve = nullCurve;
                    lastFilledPointIndex = 0;
                }
                else if(lastFilledPointIndex == 1){
                    currentCurve.point4 = contourStart;
                    double intermediateX = (2.0 * currentCurve.point1.x + currentCurve.point4.x) / 3.0;
                    double intermediateY = (2.0 * currentCurve.point1.y + currentCurve.point4.y) / 3.0;
                    currentCurve.point2 = Point(intermediateX, intermediateY);
                    intermediateX = (currentCurve.point1.x + 2.0 * currentCurve.point4.x) / 3.0;
                    intermediateY = (currentCurve.point1.y + 2.0 * currentCurve.point4.y) / 3.0;
                    currentCurve.point3 = Point(intermediateX, intermediateY);
                    (*currentGlyph.curves).push_back(currentCurve);
                    currentCurve = nullCurve;
                    lastFilledPointIndex = 0;
                }
                else{
                    std::cout << "ERROR 001: open contour found\nCurrent glyph Unicode: " << currentGlyph.unicode;
                    return -1;
                }
            }
            else if(nextData.length() > 2 && nextData.substr(0, 2) == "U+"){
                int codePoint = std::stoi(nextData.substr(2));
                inFile >> nextData;
                currentGlyph = Glyph(codePoint, std::stoi(nextData));
            }
            else{
                double x = std::stod(nextData);
                inFile >> nextData;
                double y = std::stod(nextData);
                inFile >> nextData;
                bool isOnCurve = true;
                if(nextData == "0"){isOnCurve = false;}
                if(lastFilledPointIndex == 0){
                    if(isOnCurve == false){
                        std::cout << "ERROR 002: open contour found\nCurrent glyph Unicode: " << currentGlyph.unicode;
                        return -1;
                    }
                    currentCurve.point1 = Point(x, y);
                    lastFilledPointIndex = 1;
                    contourStart = Point(x, y);
                }
                else if(lastFilledPointIndex == 1){
                    if(isOnCurve == false){
                        currentCurve.point2 = Point(x, y);
                        lastFilledPointIndex = 2;
                    }
                    else{
                        currentCurve.point4 = Point(x, y);
                        double intermediateX = (2.0 * currentCurve.point1.x + currentCurve.point4.x) / 3.0;
                        double intermediateY = (2.0 * currentCurve.point1.y + currentCurve.point4.y) / 3.0;
                        currentCurve.point2 = Point(intermediateX, intermediateY);
                        intermediateX = (currentCurve.point1.x + 2.0 * currentCurve.point4.x) / 3.0;
                        intermediateY = (currentCurve.point1.y + 2.0 * currentCurve.point4.y) / 3.0;
                        currentCurve.point3 = Point(intermediateX, intermediateY);
                        (*currentGlyph.curves).push_back(currentCurve);
                        currentCurve = nullCurve;
                        currentCurve.point1 = Point(x, y);
                        lastFilledPointIndex = 1;
                    }
                }
                else if(lastFilledPointIndex == 2){
                    if(isOnCurve == true){
                        std::cout << "ERROR 003: open contour found\nCurrent glyph Unicode: " << currentGlyph.unicode;
                        return -1;
                    }
                    currentCurve.point3 = Point(x, y);
                    lastFilledPointIndex = 3;
                }
                else if(lastFilledPointIndex == 3){
                    if(isOnCurve == false){
                        std::cout << "ERROR 004: open contour found\nCurrent glyph Unicode: " << currentGlyph.unicode;
                        return -1;
                    }
                    currentCurve.point4 = Point(x, y);
                    (*currentGlyph.curves).push_back(currentCurve);
                    currentCurve = nullCurve;
                    currentCurve.point1 = Point(x, y);
                    lastFilledPointIndex = 1;
                }
            }
        }
        std::cout << "data loaded\n";
    }
    
    {
        for(int i=0; i<Font::glyphCount; i++){
            Glyph& currentGlyph = (*(*myFont).glyphs).at(i);
            std::queue<BezierCurve>* unrasterisedCurves = new std::queue<BezierCurve>();
            for(int i=0; i<(*currentGlyph.curves).size(); i++){
                (*unrasterisedCurves).push((*currentGlyph.curves).at(i));
            }
            currentGlyph.curves = nullptr;
            while((*unrasterisedCurves).size() > 0){
                BezierCurve oldCurve = (*unrasterisedCurves).front();
                (*unrasterisedCurves).pop();
                Point a = Point((oldCurve.point1.x + oldCurve.point2.x) / 2.0, (oldCurve.point1.y + oldCurve.point2.y) / 2.0);
                Point b = Point((oldCurve.point2.x + oldCurve.point3.x) / 2.0, (oldCurve.point2.y + oldCurve.point3.y) / 2.0);
                Point c = Point((oldCurve.point3.x + oldCurve.point4.x) / 2.0, (oldCurve.point3.y + oldCurve.point4.y) / 2.0);
                Point d = Point((a.x + b.x) / 2.0, (a.y + b.y) / 2.0);
                Point e = Point((b.x + c.x) / 2.0, (b.y + c.y) / 2.0);
                Point f = Point((d.x + e.x) / 2.0, (d.y + e.y) / 2.0);
                BezierCurve newCurve1 = BezierCurve(oldCurve.point1, a, d, f);
                BezierCurve newCurve2 = BezierCurve(f, e, c, oldCurve.point3);
                if(curveIsFlat(newCurve1) == true){(*currentGlyph.rasterisedCurves).push_back(Line(newCurve1.point1, newCurve1.point4));}
                else{(*unrasterisedCurves).push(newCurve1);}
                if(curveIsFlat(newCurve2) == true){(*currentGlyph.rasterisedCurves).push_back(Line(newCurve2.point1, newCurve2.point4));}
                else{(*unrasterisedCurves).push(newCurve2);}
            }
        }
        std::cout << "rasterised glyphs\n";
    }
    
    {
        for(int i=0; i<Font::glyphCount; i++){
            Glyph& currentGlyph = (*(*myFont).glyphs).at(i);
            for(int j=0; j<Glyph::verticalSampleSize; j++){
                (*currentGlyph.leftBound).at(j) = currentGlyph.width;
                (*currentGlyph.rightBound).at(j) = currentGlyph.width;
            }
            for(int j=0; j<(*currentGlyph.rasterisedCurves).size(); j++){
                Line& currentLine = (*currentGlyph.rasterisedCurves).at(j);
                int boundBelow = static_cast<int>(std::floor(normaliseY(std::min(currentLine.point1.y, currentLine.point2.y))));
                int boundAbove = static_cast<int>(std::ceil(normaliseY(std::max(currentLine.point1.y, currentLine.point2.y))));
                if(boundBelow < 0){boundBelow = 0;}
                if(boundAbove > Glyph::verticalSampleSize - 1){boundAbove = Glyph::verticalSampleSize - 1;}
                for(int k=boundBelow; k<=boundAbove; k++){
                    double t = (unnormaliseY(static_cast<double>(k)) - currentLine.point1.y) / (currentLine.point2.y - currentLine.point1.y);
                    double x = currentLine.point1.x + t * (currentLine.point2.x - currentLine.point1.x);
                    if(x < std::min(currentLine.point1.x, currentLine.point2.x)){x = std::min(currentLine.point1.x, currentLine.point2.x);}
                    else if(x > std::max(currentLine.point1.x, currentLine.point2.x)){x = std::max(currentLine.point1.x, currentLine.point2.x);}
                    if(x < (*currentGlyph.leftBound).at(k)){(*currentGlyph.leftBound).at(k) = x;}
                    if(currentGlyph.width - x < (*currentGlyph.rightBound).at(k)){(*currentGlyph.rightBound).at(k) = currentGlyph.width - x;}
                }
            }
        }
        std::cout << "computed bounds\n";
    }
}
