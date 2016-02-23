#pragma once

#include "ofx2DMappingShape.h"

class ofx2DMappingColorShape : public ofx2DMappingShape {

    public:

    ofx2DMappingColorShape() {
        this->nature = "COLOR_SHAPE";
    }

    ofx2DMappingColorShape(const ofx2DMappingColorShape& obj) : ofx2DMappingShape(obj) {
    }

    ofPtr<ofx2DMappingObject> clone() const {
        return ofPtr<ofx2DMappingObject>(new ofx2DMappingColorShape(*this));
    }

    void draw(float w, float h) {
        ofFill();
        ofSetColor(this->color);
        ofBeginShape();
            for (int j = 0; j < this->polyline.size(); j++){
            ofVertex(this->polyline[j].x*w, this->polyline[j].y*h);
        }
        ofEndShape(true);
    }
};
