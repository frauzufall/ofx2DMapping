#pragma once

#include "ofx2DMappingObject.h"

class ofx2DMappingPoint : public ofx2DMappingObject {

    public:

    ofPoint     pos;

    ofx2DMappingPoint() {
        this->nature = "POINT";
    }

    ofx2DMappingPoint(const ofx2DMappingPoint& obj) : ofx2DMappingObject(obj) {
        this->pos = obj.pos;
    }

    ofPtr<ofx2DMappingObject> clone() const {
        return ofPtr<ofx2DMappingObject>(new ofx2DMappingPoint(*this));
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        ofx2DMappingObject::loadXml(xml);
        pos = this->getPoint(xml);
    }

    void saveXml(ofxXmlSettings_ptr xml) {
        ofx2DMappingObject::saveXml(xml);
        xml->addValue("x", pos.x);
        xml->addValue("y", pos.y);
    }

    void update(float w, float h){}

    void draw(float w, float h){}

    void drawArea(float w, float h){}

};
