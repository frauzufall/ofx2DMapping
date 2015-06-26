#pragma once

#include "MappingObject.h"

class MappingPoint : public MappingObject {

    public:

    ofPoint     pos;

    MappingPoint() {
        this->nature = "POINT";
    }

    MappingPoint(const MappingPoint& obj) : MappingObject(obj) {
        this->pos = obj.pos;
    }

    ofPtr<MappingObject> clone() const {
        return ofPtr<MappingObject>(new MappingPoint(*this));
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingObject::loadXml(xml);
        pos = this->getPoint(xml);
    }

    void saveXml(ofxXmlSettings_ptr xml) {
        MappingObject::saveXml(xml);
        xml->addValue("x", pos.x);
        xml->addValue("y", pos.y);
    }

    void update(float w, float h){}

    void draw(float w, float h){}

    void drawArea(float w, float h){}

};
