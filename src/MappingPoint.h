#pragma once

#include "MappingObject.h"

class MappingPoint : public MappingObject {

    public:

    ofPoint     pos;

    MappingPoint() {
        this->nature = "POINT";
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingObject::loadXml(xml);
        pos = this->getPoint(xml);
    }
};
