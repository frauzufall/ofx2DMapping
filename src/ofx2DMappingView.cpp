#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView():ofxGuiPage() {

    zoom = 1;
    zoom_pos = ofPoint(0.5,0.5);
    direct_edit = false;

}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
    this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float x, float y, float w, float h) {

    //MAPPING RECT PANEL
    mapping_forms.setup("MAPPING FORMS", ctrl->getProjector(), &object_list, w, h);
    mapping_forms.rebuild();
    mapping_forms.setMappingBackground(ctrl->getOutput());

    //MAIN OPTIONS PANEL

    main_panel.setup("MAPPING");

    save_btn.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
    save_btn.setup("save");
    main_panel.add(save_btn);

    import_btn.addListener(this, &ofx2DMappingView::importSvg);
    import_btn.setup("import svg");
    main_panel.add(import_btn);

    edit_mode_btn.addListener(this, &ofx2DMappingView::setEditMode);
    edit_mode_btn.setup("direct edit", direct_edit);
    main_panel.add(edit_mode_btn);

    //CALIBRATION OPTIONS

    calibration_options.setup("CALIBRATION OPTIONS");

    calibration_options.add(ctrl->getCalibrating());
    calibration_options.add(ctrl->getCalBorder());
    calibration_options.add(ctrl->getCalGrey());

    main_panel.add(calibration_options);

    //OBJECT LIST PANEL

    list_panel.setup("MAPPING OBJECTS");

    add_buttons_panel.setup("ADD MAPPING OBJECTS");

    vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
    for(uint i = 0; i < options.size(); i++) {
        ofxToggle::Config config;
        ofColor c = options.at(i)->color;
        if(c.getBrightness() < 200){
            c.setBrightness(200);
        }
        config.textColor = c;
        add_buttons_panel.add(options.at(i)->pleaseCopyMe.set("add " + options.at(i)->name, false), config);
        options.at(i)->pleaseCopyMe.addListener(this, &ofx2DMappingView::addedObject);
    }

    list_panel.add(add_buttons_panel);

    //LIST MANIPULATION OPTIONS

    list_options.setup("OBJECT MANIPULATION");

    select_all_btn.addListener(this, &ofx2DMappingView::selectAllObjects);
    select_all_btn.setup("select all");
    list_options.add(select_all_btn);

    deselect_all_btn.addListener(this, &ofx2DMappingView::deselectAllObjects);
    deselect_all_btn.setup("deselect all");
    list_options.add(deselect_all_btn);

    delete_all_btn.addListener(this, &ofx2DMappingView::removeAllObjects);
    delete_all_btn.setup("delete all");
    list_options.add(delete_all_btn);

    list_panel.add(list_options);

    //OBJECT LIST

    object_list.setup("MAPPING OBJECT LIST");
    ofAddListener(object_list.elementRemoved, this, &ofx2DMappingView::removeForm);
    ofAddListener(object_list.elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);

    list_panel.add(object_list);

    ofxGuiPage::setup("Mapping");
    setShowHeader(false);
    add(main_panel);
    add(list_panel);
    add(mapping_forms);

    setShape(x,y,w,h);

    setSubpanelPositions();

    updateObjectList();


}

void ofx2DMappingView::addedObject(bool & clickstart){
    if(clickstart){
        ctrl->update();
        mapping_forms.updateForms();
        updateObjectList();
    }
}

void ofx2DMappingView::updateObjectList() {

    object_list.clear();

    ofx2DMappingProjector *p = ctrl->getProjector();

    for(uint i = 0; i < p->shapeCount(); i++) {

        ofPtr<ofx2DMappingObject> mq = p->getMappingObject(i);
        if(mq) {
            //insert toggles at beginning of list
            mq->editable.setName(mq->name);
            ofColor c = mq->color;
            if(c.getBrightness() < 200){
                c.setBrightness(200);
            }
            ofxToggle::Config config;
            config.textColor = c;
            object_list.add(mq->editable, config, false);
        }
    }
}

void ofx2DMappingView::importSvg() {
    ctrl->importSvg();
    mapping_forms.updateForms();
    object_list.clear();
    updateObjectList();
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

    ofx2DMappingProjector *p = ctrl->getProjector();
    int index = p->shapeCount()-1-data.index;
    if(p->removeShape(index)) {
        p->updateOutlines();
        mapping_forms.updateForms();
    }

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

    ofx2DMappingProjector *p = ctrl->getProjector();

    int index1 = p->shapeCount()-1-data.old_index;
    int index2 = p->shapeCount()-1-data.new_index;

    bool swapped = p->swapShapes(index1,index2);
    if(swapped) {
        mapping_forms.updateForms();
    }

}

void ofx2DMappingView::setSubpanelPositions() {
    float margin = 10;
    ofPoint pos = this->getPosition();
    pos.y += spacing;
    if(bShowHeader){
        pos.y += header += spacingFirstElement;
    }
    main_panel.sizeChangedE.disable();
    list_panel.sizeChangedE.disable();
    mapping_forms.sizeChangedE.disable();
    main_panel.setPosition(
                pos.x+margin,
                pos.y+margin);
    list_panel.setPosition(
                pos.x+margin,
                pos.y+margin+main_panel.getHeight()+margin);
    mapping_forms.setPosition(
                main_panel.getPosition().x + main_panel.getWidth()+margin,
                pos.y+margin);
    main_panel.sizeChangedE.enable();
    list_panel.sizeChangedE.enable();
    mapping_forms.sizeChangedE.enable();
}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
    mapping_forms.setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
    mapping_forms.getShowSource().set(show);
}

void ofx2DMappingView::setEditMode(bool &direct_edit) {
    this->direct_edit = direct_edit;
    mapping_forms.setEditMode(direct_edit);
    setSubpanelPositions();
    //TODO trigger button
}

ofx2DFormMapping *ofx2DMappingView::getFormMapping() {
    return &mapping_forms;
}

void ofx2DMappingView::selectAllObjects() {
    for(uint i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
        ctrl->getProjector()->getMappingObject(i)->editable = true;
    }
}

void ofx2DMappingView::deselectAllObjects() {
    for(uint i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
        ctrl->getProjector()->getMappingObject(i)->editable = false;
    }
}

void ofx2DMappingView::removeAllObjects() {
    ctrl->getProjector()->removeAllShapes();
    ctrl->getProjector()->updateOutlines();
    mapping_forms.updateForms();
    updateObjectList();
}

void ofx2DMappingView::setSize(float width, float height) {
    setShape(this->getPosition().x, this->getPosition().y, width, height);
}

void ofx2DMappingView::setShape(ofRectangle shape) {
    setShape(shape.x, shape.y, shape.width, shape.height);
}

void ofx2DMappingView::setShape(float x, float y, float width, float height) {
    int margin = 10;
    mapping_forms.sizeChangedE.disable();
    mapping_forms.setSize(width-object_list.getWidth()-margin*3, height-main_panel.getHeight()-margin*3);
    mapping_forms.sizeChangedE.enable();
    setSubpanelPositions();
    ofxGuiPage::setShape(x,y,width,height);
}
