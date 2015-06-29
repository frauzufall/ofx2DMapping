#include "Star.h"

Star::Star() {

    //this constructor is used while creating the template object when you call mapping.getControl()->addTemplate<Star>("star");

    this->nature = "STAR";
    color = ofColor::fromHsb(ofRandom(0,255),200,200);
    speed = ofRandom(0,100);
}

Star::Star(const Star& obj) : ofx2DMappingPoint(obj) {

    //the copy constructor is called every time a new object of the template is added to your scene
    //obj is the template object

    color = ofColor::fromHsb(ofRandom(0,255),200,200);
    speed = ofRandom(0,100);

    //to get all the stars the same speed and color, do this:
    //speed = obj.speed
    //color = obj.color

}

ofPtr<ofx2DMappingObject> Star::clone() const {
    return ofPtr<ofx2DMappingObject>(new Star(*this));
}

void Star::saveXml(ofxXmlSettings_ptr xml) {
    ofx2DMappingPoint::saveXml(xml);

    //add any values of the object you want to save

    xml->addValue("speed", speed);
}

void Star::loadXml(ofxXmlSettings_ptr xml) {
    ofx2DMappingPoint::loadXml(xml);

    //equivalent to saveXml

    xml->getValue("speed", 10, 0);
}

void Star::update(float w, float h) {

}

void Star::draw(float w, float h) {
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


