#pragma once

#include "MappingShape.h"

class MappingColorShape : public MappingShape {

    public:

    MappingColorShape() {
        this->nature = "COLOR_SHAPE";
    }

    MappingColorShape(const MappingColorShape& obj) : MappingShape(obj) {
    }

    ofPtr<MappingObject> clone() const {
        return ofPtr<MappingObject>(new MappingColorShape(*this));
    }

    void draw(float w, float h) {
        ofFill();
        ofSetColor(this->color);
        ofBeginShape();
            for (uint j = 0; j < this->polyline.size(); j++){
            ofVertex(this->polyline[j].x*w, this->polyline[j].y*h);
        }
        ofEndShape(true);
    }
};
