#include "Scene2.h"

Scene2::Scene2()
{
}

void Scene2::setup(float w, float h) {

    fbo = ofPtr<ofFbo>(new ofFbo());
    fbo->allocate(w,h,GL_RGBA);
    fbo->begin();
    ofClear(0,0,0,0);
    fbo->end();

    myfont.loadFont("arial.ttf", 112);
}

void Scene2::update() {

    int time_i = ofGetElapsedTimeMillis();

    fbo->begin();

    ofColor c  = ofColor::fromHsb((int)(time_i*0.005)%255, 200, 255);

    ofSetColor(c);
    ofFill();

    ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());

    ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofSetColor(c.getInverted());

    myfont.drawString("IN LOVE WITH", 10, fbo->getHeight()*0.5);

    fbo->end();

}

ofPtr<ofFbo> & Scene2::getFbo() {
    return fbo;
}
