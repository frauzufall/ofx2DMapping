#pragma once

#include "ofx2DMappingObject.h"
#include "ofx2DMappingContentShape.h"
#include "ofx2DMappingFbo.h"
#include "ofx2DMappingColorShape.h"
#include "ofx2DMappingShape.h"
#include "ofx2DMappingImage.h"
#include "ofx2DMappingPoint.h"
#include "ofxSvg.h"

typedef ofPtr< vector<ofPolyline> > ofPolylines_ptr;
typedef ofPtr< vector<ofPtr<ofPath>> > ofPaths_ptr;

class ofx2DMappingProjector {

public:

    ofx2DMappingProjector(float w, float h);

    ~ofx2DMappingProjector();

    void update();
    void                        updateOutlines();
    void                        updateOutline(int shape_id);

    ofPoint relative(ofPoint orig);

    ofPtr<ofx2DMappingObject> addShape(ofPtr<ofx2DMappingObject> obj, bool swap = false);
    ofPtr<ofx2DMappingObject> addShape(string type, string name, bool swap = false);
    ofPtr<ofx2DMappingObject> copyShape(ofPtr<ofx2DMappingObject> obj, bool swap = false);

    bool removeShape(int id);
    void removeAllShapes();

    bool swapShapes(int index1, int index2);

    ofPtr<ofx2DMappingObject> getMappingObject(int id);

    uint shapeCount();

    ofPtr<ofx2DMappingObject> getFirstImageShape();

    template <class T>
    vector<ofPtr<T>> getShapesByClass();

    ofMatrix4x4 getMatrixOfImageAtPoint(ofPoint p);

    bool pointVisibleInShape(ofPoint p, ofPtr<ofx2DMappingContentShape> mq);

    bool isLeft(ofPoint a, ofPoint b, ofPoint c);

    ofPoint getStartPoint();

    void setStartPoint(ofPoint p);

    void                        exportSvg(string path);

    ofPtr<ofxSVG>                  svg();
    ofPolylines_ptr             outlines();
    ofPolylines_ptr             outlinesRaw();
    ofPaths_ptr                 paths();

    bool reloadSvg(string file);
    void                        importSvg(string file);
    void                        reloadLinesFromRaw();

    float			outputWidth();
    float			outputHeight();
    void setOutputSize(float w, float h);

    ofPoint inOutput(ofPoint orig);

    ofParameter<bool> &getUsingCam();
    ofPoint inCameraView(ofPoint orig);
    ofPoint (&getCamera())[4];
    void setCamera(ofPoint (&arr)[4]);

private:

    float output_w, output_h;

    ofPtr<ofxSVG>                  _svg;
    ofPaths_ptr                 _paths;
    ofPolylines_ptr             _outlines;
    ofPolylines_ptr             _outlines_raw;

    vector<ofPtr<ofx2DMappingObject>> shapes;
    ofPoint                 start_point;
    ofPoint                 plane[4];

    ofFbo_ptr               input_fbo;

    ofPoint                 camera[4];
    ofMatrix4x4             camera_homography;

    ofParameter<bool>           use_cam;

};

template <class T>
vector<ofPtr<T>> ofx2DMappingProjector::getShapesByClass() {
    vector<ofPtr<T>> res;
    res.clear();
    ofPtr<T> mo;
    for(uint i = 0; i < shapeCount(); i++) {
        mo = dynamic_pointer_cast<T>(getMappingObject(i));
        if(mo) {
            res.push_back(mo);
        }
    }
    return res;
}
