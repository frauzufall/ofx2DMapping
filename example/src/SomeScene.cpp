#include "SomeScene.h"

SomeScene::SomeScene()
{
}

void SomeScene::setup(float w, float h) {

    fbo = ofPtr<ofFbo>(new ofFbo());
    fbo->allocate(w,h,GL_RGBA);
    fbo->begin();
    ofClear(0,0,0,0);
    fbo->end();

    myfont.load("arial.ttf", 112);
}

void SomeScene::update() {

    int time_i = ofGetElapsedTimeMillis();

    fbo->begin();

    ofColor c  = ofColor::fromHsb((int)(time_i*0.005)%255, 200, 255);

    ofSetColor(c);
    ofFill();

    ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());

    ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofSetColor(c.getInverted());

    myfont.drawString("OPENFRAMEWORKS", 10, fbo->getHeight()*0.5);

    fbo->end();

}

ofPtr<ofFbo> & SomeScene::getFbo() {
    return fbo;
}
