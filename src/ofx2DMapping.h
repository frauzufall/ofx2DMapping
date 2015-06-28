#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofx2DMappingView.h"

class ofx2DMapping {

public:
    ofx2DMapping();
    ~ofx2DMapping();

    void setup();
    void setup(string mapping_path);
    void update();
    void draw();

    void showControls(bool show);

    ofx2DMappingController* getControl();
    ofx2DMappingView* getControlView();

    void setOutputShape(float x, float y, float width, float height);
    void setControlShape(float x, float y, float width, float height);

    template <class T>
    ofPtr<T> addTemplate(string name) {
        return ctrl.addTemplate<T>(name);
    }

private:

    ofx2DMappingController ctrl;
    ofx2DMappingView view;

    bool show_controls;

};
