#ifndef ENUM_HPP
#define ENUM_HPP

#define MENU_WIDTH  (8*5)
#define MENU_HEIGHT (8*4)
#define BUTTON_EXTRA 40
#define BUTTON_ROWS 11
#define BUTTON_WIDTH  24
#define BUTTON_HEIGHT 23
#define TOOLBAR_WIDTH (BUTTON_WIDTH*2)
#define TOOLBAR_HEIGHT ((BUTTON_HEIGHT*BUTTON_ROWS)+BUTTON_EXTRA)
#define COLOR_WIDTH 8
//#define COLOR_HEIGHT 8

#define GRADIENT_CELL_W 8
#define GRADIENT_CELL_H 8
#define GRADIENT_SEL_W  4

#define UNDO_LEVELS      16

#define SWIDTH		640
#define SHEIGHT		480

extern int VWIDTH;
extern int VHEIGHT;

typedef enum E_BrushType {
	BT_POINT_1,
	BT_POINT_2,
	BT_POINT_3,
	BT_POINT_4,
	BT_POINT_5,
	BT_SQUARE_1,
	BT_SQUARE_2,
	BT_SQUARE_3,
	BT_SQUARE_4,
	BT_SQUARE_5,
	BT_SPECIAL_5DOTS,
	BT_SPECIAL_3DOTS,
	BT_SPECIAL_2DOTS,
	BT_SPECIAL_VERT,
	BT_SPECIAL_HORZ,
	BT_COUNT
}BrushType;

typedef enum E_DashMode {
	DM_SOLID,
	DM_ODDEVEN,
	DM_SPARE
}DashMode;

typedef enum E_ColorMode {
	CM_PAINT,
	CM_SINGLECOLOR,
	CM_REPLACE,
	CM_SMEAR,
	CM_SHADE,
	CM_CYCLE,
	CM_SMOOTH,
	CM_MULTICYCLE,
	CM_COUNT
}ColorMode;

typedef enum E_ToolType {
	T_BRUSH,
	T_LINE,
	T_SQUARE,
	T_POLY,
	T_FLOOD,
	T_COPY,
	T_GRID,
	T_ZOOM,
	T_PIPE,
	T_UNDO,
	T_GRADIENT,

	T_DASH,
	T_BEZIER,
	T_CIRCLE,
	T_SHAPE,
	T_SPRAY,
	T_TEXT,
	T_PERSPECTIVE,
	T_HAND,
	T_SYMMETRY,
	T_CLEAR,
	T_PATTERNS,
	T_COUNT
}ToolType;


typedef enum E_FillType {
	FT_COLORPATTERN,
	FT_PERSPECTIVE,
	FT_BRUSHWRAP,
	FT_LEVEL,
	FT_GRADIENT_LINEAR_STRAIGHT,
	FT_GRADIENT_LINEAR_SHAPED,
	FT_GRADIENT_LINEAR_RIDGES,
	FT_GRADIENT_RADIAL_CIRCULAR,
	FT_GRADIENT_RADIAL_CONTOURS,
	FT_GRADIENT_RADIAL_HIGHLIGHT,
}FillType;



typedef enum E_Measurement {
	MT_INCHES,
	MT_CENTIMETERS,
	MT_POINTS,
	MT_COUNT
}Measurement;

typedef struct s_SignedPoint {
	int x;
	int y;
}SignedPoint;

typedef struct s_Menu {
	int columns;
	int rows;
	int item_width;
	int item_height;
	void *bitmap;
}Menu;

typedef struct s_ColorStop {
	int dither_pattern;
	int color;
	int alt_color;
	float position;		//0.0-1.0
}ColorStop;

#endif