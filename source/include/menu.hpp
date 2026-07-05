#ifndef MENU_HPP
#define MENU_HPP

#include "enum.h"

extern void pictureNew();
extern void pictureRevert();
extern void pictureLoad();
extern void pictureSave();
extern void pictureSaveAs();
extern void spareToPage();
extern void flipPageSpare();
extern void pageDelete();
extern void pageSize();
extern void pageView();

extern void pictureUndo();
extern void setPaintMode(ColorMode paintMode);
extern void setMeasurements(Measurement measurementType);
extern void paintModeToggleMulticycle();
extern void fontTogglePlain();
extern void fontToggleBold();
extern void fontToggleItalic();
extern void fontToggleUnderline();
extern void fontSetSize(int size);
extern void pictureReplaceBgFg();
extern void brushReplaceBgFg();
extern void pictureCopyBgFg();
extern void brushCopyBgFg();
extern void toggleCycle();
extern void toggleAntialias();
extern void toggleTranslucent();
extern void toggleColorize();
extern void brushRemap();
extern void pictureRemap();
extern void toggleInfoBar();
extern void toggleInfoBarCoordinates();
extern void toggleInfoBarInPixels();
extern void defaultPalette();
extern void restorePalette();
extern void paletteFromBrush();
extern void brushRestore();
extern void brushOutline();
extern void brushHalveHorz();
extern void brushHalveVert();
extern void brushDoubleVert();
extern void brushDoubleHorz();
extern void brushStretchHorz();
extern void brushStretchVert();
extern void brushBendVert();
extern void brushBendHorz();
extern void brushShearVert();
extern void brushShearHorz();
extern void brushFlipVert();
extern void brushFlipHorz();
extern void brushLoad();
extern void brushSave();
extern void brushRotate(int angle);
extern void toggleFastFeedback();
extern void toggleSquareAspect();
extern void toggleVideoPatterns();
extern void toggleHalfTones();
extern void toggleBrushHandle();
extern void toggleExcludeEdge();
extern void pictureBackup();
extern void sparePreferences();
extern void toggleStencil();
extern void stencilMake();
extern void stencilDiscard();
extern void stencilReverse();
extern void stencilLockFg();
extern void stencilLoad();
extern void stencilSave();
extern void stencilBackgroundFix();
extern void stencilBackgroundFree();

#endif 