#pragma once

#include "MappingPoint.h"

class Star : public MappingPoint {

    public:

    Star() {
        this->nature = "STAR";
        color = ofColor::fromHsb(ofRandom(0,255),200,200);
        speed = ofRandom(0,100);
    }

    Star(const Star& obj) : MappingPoint(obj) {
        color = ofColor::fromHsb(ofRandom(0,255),200,200);
        speed = ofRandom(0,100);
        //to get all the stars the same speed and color, do this:
        //speed = obj.speed
        //color = obj.color
    }

    ofPtr<MappingObject> clone() const {
        return ofPtr<MappingObject>(new Star(*this));
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingPoint::loadXml(xml);
        xml->getValue("speed", 10, 0);
    }

    void saveXml(ofxXmlSettings_ptr xml) {
        MappingPoint::saveXml(xml);
        xml->addValue("speed", speed);
    }

    void update(float w, float h){}

    void draw(float w, float h){
        ofPushMatrix();
        ofTranslate(this->pos.x*w, this->pos.y*h);
        ofRotateZ(ofGetElapsedTimef()*speed);
        ofSetColor(color);
        ofFill();
        ofSetPolyMode(OF_POLY_WINDING_NONZERO);
        ofBeginShape();
          ofVertex(100,35);
          ofVertex(-85,35);
          ofVertex(65,-75);
          ofVertex(5,100);
          ofVertex(-50,-75);
        ofEndShape();
        ofPopMatrix();
    }

    void drawArea(float w, float h){}

private:
    float speed;

};
