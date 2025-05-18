#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#define float double

struct Point{
	float x;
	float y;
	Point(float x, float y) : x(x), y(y) {}
};

struct B3curve{
	Point p1;
	Point p2;
	Point p3;
	Point p4;
	B3curve(Point p1, Point p2, Point p3, Point p4) : p1(p1), p2(p2), p3(p3), p4(p4) {}
};

struct Glyph{
	B3curve** l;
	int n;
	int c;
	float* lb;
	float* rb;
	float w;
	Glyph(int c){
		(*this).c = c;
		(*this).l = new B3curve* [100];
		(*this).n = 0;
		(*this).lb = new float[90];
		std::fill((*this).lb, (*this).lb + 90, 1000.0);
		(*this).rb = new float[90];
		std::fill((*this).rb, (*this).rb + 90, 0.0);
		(*this).w = 0.0;
	}
};

struct Font{
	Glyph** l;
	int n;
	Font(){
		(*this).l = new Glyph* [1000];
		n = 0;
	}
};

void confirm(float x, float& l, float& r, B3curve& cv){
	if(x < 0.0 || x > 1.0){return;}
	float xc = (1.0 - x) * (1.0 - x) * (1.0 - x) * cv.p1.x + 3.0 * (1.0 - x) * (1.0 - x) * x * cv.p2.x + 3.0 * (1.0 - x) * x * x * cv.p3.x + x * x * x * cv.p4.x;
	if(xc <= 0.01 || xc > 1000.0){return;}
	l = std::min(l, xc);
	r = std::max(r, xc);
}

void solvecubic(float a, float b, float c, float d, float& l, float& r, B3curve& cv){
	float ca = a;
	float cb = b;
	float cc = c;
	float cd = d;
	cb /= ca;
	cc /= ca;
	cd /= ca;
	float cp = (3.0 * cc - cb * cb) / 3.0;
	float cq = (2.0 * cb * cb * cb - 9.0 * cb * cc + 27.0 * cd) / 27.0;
	float dc = 4.0 * cp * cp * cp + 27.0 * cq * cq;
	if(cp > -0.001 && cp < 0.001){
		float r1 = std::cbrt(-1.0 * cq) - cb / 3.0;
		confirm(r1, l, r, cv);
	}
	else if(dc <= 0){
		float r1 = 2.0 * std::sqrt(-1.0 / 3.0 * cp) * std::cos(1.0 / 3.0 * std::acos(1.5 * cq / cp * std::sqrt(-3.0 / cp)) - 2.0 / 3.0 * 3.1415926535897932384626433832795028841971 * 1.0) - cb / 3.0;
		float r2 = 2.0 * std::sqrt(-1.0 / 3.0 * cp) * std::cos(1.0 / 3.0 * std::acos(1.5 * cq / cp * std::sqrt(-3.0 / cp)) - 2.0 / 3.0 * 3.1415926535897932384626433832795028841971 * 2.0) - cb / 3.0;
		float r3 = 2.0 * std::sqrt(-1.0 / 3.0 * cp) * std::cos(1.0 / 3.0 * std::acos(1.5 * cq / cp * std::sqrt(-3.0 / cp)) - 2.0 / 3.0 * 3.1415926535897932384626433832795028841971 * 3.0) - cb / 3.0;
		confirm(r1, l, r, cv);
		confirm(r2, l, r, cv);
		confirm(r3, l, r, cv);
	}
	else if(cp < 0){
		float r1 = -2.0 * std::abs(cq) / cq * std::sqrt(-1.0 / 3.0 * cp) * std::cosh(1.0 / 3.0 * std::acosh(-1.5 * std::abs(cq) / cp * std::sqrt(-3.0 / cp))) - cb / 3.0;
		confirm(r1, l, r, cv);
	}
	else{
		float r1 = -2.0 * std::sqrt(cp / 3.0) * std::sinh(1.0 / 3.0 * std::asinh(1.5 * cq / cp * std::sqrt(3.0 / cp))) - cb / 3.0;
		confirm(r1, l, r, cv);
	}
}

void getintercept(float y, B3curve& c, float& l, float& r){
	float ca = (-1.0)*c.p1.y + 3.0*c.p2.y - 3.0*c.p3.y + c.p4.y;
	float cb = 3.0*c.p1.y - 6.0*c.p2.y + 3.0*c.p3.y;
	float cc = (-3.0)*c.p1.y + 3.0*c.p2.y;
	float cd = c.p1.y - y;
	solvecubic(ca, cb, cc, cd, l, r, c);
}

int main(){
	std::ifstream infile("path\\to\\root\\glyphsdata.txt");
	std::ofstream outfile("path\\to\\root\\kerningdata.txt");
	std::ofstream errlog("path\\to\\root\\errorlog.txt");
	Font f;
	std::string s;
	while(infile >> s){
		if(s == "EXTRACTIONCOMPLETE"){break;}
		f.n++;
		f.l[f.n-1] = new Glyph(std::stoi(s.substr(2, s.size() - 2)));
		float w;
		infile >> w;
		(*f.l[f.n-1]).w = w;
		Point p1(0, 0);
		Point p2(0, 0);
		Point p3(0, 0);
		Point p4(0, 0);
		int ps = 11;
		Point startpoint(0,0);
		while(infile >> s){
			if(s == "ENDGLYPH"){break;}
			if(s == "END"){
				ps = 11;
				p4 = startpoint;
				(*f.l[f.n-1]).n++;
				(*f.l[f.n-1]).l[(*f.l[f.n-1]).n-1] = new B3curve(p1, p2, p3, p4);
				p1 = Point(0,0);
				p2 = Point(0,0);
				p3 = Point(0,0);
				p4 = Point(0,0);
				startpoint = Point(0,0);
			}
			else{
				float i = std::stof(s);
				if(ps == 11){
					ps = 12;
					p1 = Point(i, 0);
				}
				else if(ps == 12){
					ps = 21;
					p1 = Point(p1.x, i);
					if(startpoint.x == 0 && startpoint.y == 0){startpoint = p1;}
				}
				else if(ps == 21){
					ps = 22;
					p2 = Point(i, 0);
				}
				else if(ps == 22){
					ps = 31;
					p2 = Point(p2.x, i);
				}
				else if(ps == 31){
					ps = 32;
					p3 = Point(i, 0);
				}
				else if(ps == 32){
					ps = 41;
					p3 = Point(p3.x, i);
				}
				else if(ps == 41){
					ps = 42;
					p4 = Point(i, 0);
				}
				else if(ps == 42){
					ps = 21;
					p4 = Point(p4.x, i);
					(*f.l[f.n-1]).n++;
					(*f.l[f.n-1]).l[(*f.l[f.n-1]).n-1] = new B3curve(p1, p2, p3, p4);
					p1 = p4;
					p2 = Point(0,0);
					p3 = Point(0,0);
					p4 = Point(0,0);
				}
			}
		}
	}
	for(int i=0; i<f.n; i++){
		for(int j=0; j<90; j++){
			for(int k=0; k<(*f.l[i]).n; k++){
				getintercept(900.0 * (float)j / 90.0 - 300.0 + 1000.0, (*(*f.l[i]).l[k]), (*f.l[i]).lb[j], (*f.l[i]).rb[j]);
			}
			if((*f.l[i]).rb[j] - (*f.l[i]).lb[j] < 10.0){
				(*f.l[i]).lb[j] = 1000.0;
				(*f.l[i]).rb[j] = 0.0;
			}
			if((char)((*f.l[i]).c) == '.'){
				errlog << ". " << j << " " << (*f.l[i]).lb[j] << " " << (*f.l[i]).rb[j] << "\n";
			}
		}
	}
	for(int i=0; i<f.n; i++){
		for(int j=0; j<88; j++){
			if(std::abs((*f.l[i]).lb[j+1] - (*f.l[i]).lb[j]) > 100.0 && std::abs((*f.l[i]).lb[j+1] - (*f.l[i]).lb[j+2]) > 100.0){
				(*f.l[i]).lb[j+1] = 1000.0;
			}
		}
		for(int j=0; j<88; j++){
			if(std::abs((*f.l[i]).rb[j+1] - (*f.l[i]).rb[j]) > 100.0 && std::abs((*f.l[i]).rb[j+1] - (*f.l[i]).rb[j+2]) > 100.0){
				(*f.l[i]).rb[j+1] = 0.0;
			}
		}
	}
	for(int i=0; i<f.n; i++){
		float* sb = new float[90];
		for(int j=0; j<90; j++){sb[j] = (*f.l[i]).lb[j];}
		for(int j=0; j<90; j++){
			for(int k=std::max(0, j-2); k<=std::min(89, j+2); k++){
				(*f.l[i]).lb[j] = std::min((*f.l[i]).lb[j], sb[k]);
			}
		}
		for(int j=0; j<90; j++){sb[j] = (*f.l[i]).rb[j];}
		for(int j=0; j<90; j++){
			for(int k=std::max(0, j-2); k<=std::min(89, j+2); k++){
				(*f.l[i]).rb[j] = std::max((*f.l[i]).rb[j], sb[k]);
			}
		}
	}
	for(int i=0; i<f.n; i++){
		for(int j=0; j<f.n; j++){
			int vo = 0;
			float v = -1000.0;
			for(int k=0; k<90; k++){
				if((*f.l[i]).rb[k] == 0.0 || (*f.l[j]).lb[k] == 1000.0){continue;}
				v = std::max(v, 50.0f - (*f.l[j]).lb[k] - (*f.l[i]).w + (*f.l[i]).rb[k]);
				vo++;
			}
			if(vo < 5){
				v = 0.0;
				std::string errstr = "    kerning ignored: ";
				errstr[0] = (char)((*f.l[i]).c);
				errstr[2] = (char)((*f.l[j]).c);
				errlog << errstr << std::to_string(vo) << "\n";
			}
			outfile << std::to_string((*f.l[i]).c) << " " << std::to_string((*f.l[j]).c) << " " << std::to_string((int)v) << "\n";
		}
	}
}