#pragma once

#include "MappingObject.h"
#include "MappingContentShape.h"
#include "MappingFbo.h"
#include "MappingColorShape.h"
#include "MappingShape.h"
#include "MappingImage.h"
#include "MappingPoint.h"
#include "ofxSvg.h"

typedef ofPtr<ofxSVG> ofxSVG_ptr;
typedef ofPtr< vector<ofPolyline> > ofPolylines_ptr;
typedef ofPtr< vector<ofPtr<ofPath>> > ofPaths_ptr;
typedef ofPtr<MappingObject> MappingObject_ptr;
typedef ofPtr<MappingShape> MappingShape_ptr;
typedef ofPtr<MappingContentShape> MappingContentShape_ptr;
typedef ofPtr<MappingColorShape> MappingColorShape_ptr;
typedef ofPtr<MappingImage> MappingImage_ptr;
typedef ofPtr<MappingFbo> MappingFbo_ptr;
typedef ofPtr<MappingPoint> MappingPoint_ptr;

class Projector {

public:

    Projector(float w, float h);

    ~Projector();

    void update();
    void                        updateOutlines();
    void                        updateOutline(int shape_id);

    ofPoint relative(ofPoint orig);

    MappingObject_ptr addShape(MappingObject_ptr obj, bool swap = false);
    MappingObject_ptr addShape(string type, string name, bool swap = false);
    MappingObject_ptr copyShape(MappingObject_ptr obj, bool swap = false);

    bool removeShape(int id);
    void removeAllShapes();

    bool swapShapes(int index1, int index2);

    MappingObject_ptr getShape(int id);

    uint shapeCount();

    MappingObject_ptr getFirstImageShape();

    template <class T>
    vector<ofPtr<T>> getShapesByClass() {
        vector<ofPtr<T>> res;
        res.clear();
        ofPtr<T> mo;
        for(uint i = 0; i < shapeCount(); i++) {
            mo = dynamic_pointer_cast<T>(getShape(i));
            if(mo) {
                res.push_back(mo);
            }
        }
        return res;
    }

    ofMatrix4x4 getMatrixOfImageAtPoint(ofPoint p);

    bool pointVisibleInShape(ofPoint p, MappingContentShape_ptr mq);

    bool isLeft(ofPoint a, ofPoint b, ofPoint c);

    ofPoint getStartPoint();

    void setStartPoint(ofPoint p);

    void                        saveMappingAsSvg(string path);

    ofxSVG_ptr                  svg();
    ofPolylines_ptr             outlines();
    ofPolylines_ptr             outlinesRaw();
    ofPaths_ptr                 paths();

    void                        reloadSvg(string file);
    void                        importSvg(string file);
    void                        reloadLinesFromRaw();

    ofParameter<float>			outputWidth();
    ofParameter<float>			outputHeight();

private:

    float                       output_w, output_h;

    ofxSVG_ptr                  _svg;
    ofPaths_ptr                 _paths;
    ofPolylines_ptr             _outlines;
    ofPolylines_ptr             _outlines_raw;

    vector<MappingObject_ptr> shapes;
    ofPoint                 start_point;
    ofPoint                 plane[4];

    ofFbo_ptr               input_fbo;

};


