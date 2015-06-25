#pragma once

#include "ofMain.h"

class SomeScene
{
public:
    SomeScene();
    void setup(float w, float h);
    void update();
    ofPtr<ofFbo> &getFbo();

private:
    ofPtr<ofFbo> fbo;
    ofTrueTypeFont myfont;
};
