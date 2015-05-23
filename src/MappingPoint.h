#pragma once

#include "MappingObject.h"

class MappingPoint : public MappingObject {

    public:

    ofPoint     pos;

    MappingPoint() {
        this->nature = "POINT";
    }

    void copy(ofPtr<MappingPoint> obj)  {
        MappingObject::copy(obj);
        this->pos = obj->pos;
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingObject::loadXml(xml);
        pos = this->getPoint(xml);
    }
};
