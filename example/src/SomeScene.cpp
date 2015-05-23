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
}

void SomeScene::update() {

    float time_f = ofGetElapsedTimef();
    int time_i = ofGetElapsedTimeMillis();

    fbo->begin();

    ofSetColor(ofColor::fromHsb((int)(time_i*0.02)%255, 255, 255));
    ofSetLineWidth(10);

    ofDrawLine(
                fbo->getWidth()/2,
                fbo->getHeight()/2,
                fbo->getWidth()/2+sin(time_f)*(fbo->getWidth()),
                fbo->getHeight()/2+cos(time_f)*(fbo->getWidth())
    );

    fbo->end();

}

ofPtr<ofFbo> SomeScene::getFbo() {
    return fbo;
}
