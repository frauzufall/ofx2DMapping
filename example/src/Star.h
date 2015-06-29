#pragma once

#include "ofx2DMappingPoint.h"

class Star : public ofx2DMappingPoint {

    public:

    Star();

    Star(const Star& obj);

    ofPtr<ofx2DMappingObject> clone() const;

    void loadXml(ofxXmlSettings_ptr xml);

    void saveXml(ofxXmlSettings_ptr xml);

    void update(float w, float h);

    void draw(float w, float h);

private:
    float speed;

};
