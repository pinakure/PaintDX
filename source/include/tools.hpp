#ifndef TOOLS_HPP
#define TOOLS_HPP

void toolSquare(bool keyup, int x, int y);
void toolLine(bool keyup, int x, int y);
void toolPipe(bool keyup, int x, int y);
void toolZoom(bool keyup, int x, int y);
void toolCopy(bool keyup, int x, int y);
void toolFlood(bool keyup, int x, int y);
void toolHand(bool keyup, int x, int y);
void toolPoly(bool keyup, int x, int y);
void toolCircle(bool keyup, int x, int y);
void toolShape(bool keyup, int x, int y);
void toolBezier(bool keyup, int x, int y);
void toolBrush(bool keyup, int x, int y);
void toolPerspective(bool keyup, int x, int y);
void toolText(bool keyup, int x, int y);
void toolSpray(bool keyup, int x, int y);



struct Point {
	int x;
	int y;
};

#endif