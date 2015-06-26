#pragma once

#include "MappingPoint.h"

class Star : public MappingPoint {

    public:

    Star();

    Star(const Star& obj);

    ofPtr<MappingObject> clone() const;

    void loadXml(ofxXmlSettings_ptr xml);

    void saveXml(ofxXmlSettings_ptr xml);

    void update(float w, float h);

    void draw(float w, float h);

private:
    float speed;

};
