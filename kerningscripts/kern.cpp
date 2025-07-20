#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <iostream>

struct Point{
    double x;
    double y;
    Point(){
        x = 0.0;
        y = 0.0;
    }
    Point(int x, int y){
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

struct Glyph{
    static constexpr int verticalSampleSize = 90;
    int unicode;
    double width;
    std::array<double, Glyph::verticalSampleSize>* leftBound;
    std::array<double, Glyph::verticalSampleSize>* rightBound;
    std::vector<BezierCurve>* curves;
    Glyph(){
        unicode = -1;
        width = 0;
        leftBound = new std::array<double, Glyph::verticalSampleSize>();
        rightBound = new std::array<double, Glyph::verticalSampleSize>();
        curves = new std::vector<BezierCurve>();
    }
    Glyph(int c, int w){
        unicode = c;
        width = w;
        leftBound = new std::array<double, Glyph::verticalSampleSize>();
        rightBound = new std::array<double, Glyph::verticalSampleSize>();
        curves = new std::vector<BezierCurve>();
    }
};

struct Font{
    static constexpr int minimumUnicode = 33;
    static constexpr int maximumUnicode = 126;
    static constexpr int glyphCount = Font::maximumUnicode - Font::minimumUnicode + 1;
    std::array<Glyph, Font::glyphCount>* glyphs;
    Font(){
        glyphs = new std::array<Glyph, Font::glyphCount>();
    }
};

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
                (*(*myFont).glyphs)[currentGlyph.unicode - Font::minimumUnicode] = currentGlyph;
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
}
