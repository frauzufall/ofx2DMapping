#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView() {

    control_rect.position = ofPoint(0,0);
    zoom = 1;
    zoom_pos = ofPoint(0.5,0.5);

}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
    this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float w, float h) {

    control_rect.width= w;
    control_rect.height= h;

    direct_edit = false;

    //MAPPING RECT PANEL
    mapping_dst.setup("MAPPING FORMS", ctrl->getProjector(0), &shape_list, w, h);
    mapping_dst.rebuild();
    mapping_dst.setMappingBackground(ctrl->getOutput());
    ofAddListener(ctrl->getProjector(0)->updatedFbo, &mapping_dst, &FormMapping::setMappingBackground);

    //MAIN OPTIONS PANEL

    main_options.setup("MAPPING");

    save_btn.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
    save_btn.setup("save");
    main_options.add(&save_btn);

    import_btn.addListener(this, &ofx2DMappingView::importSvg);
    import_btn.setup("import svg");
    main_options.add(&import_btn);

    edit_mode_btn.addListener(this, &ofx2DMappingView::setEditMode);
    edit_mode_btn.setup("direct edit", direct_edit);
    main_options.add(&edit_mode_btn);

    //ADDING BUTTONS PANEL

    add_buttons_panel.setup("ADD MAPPING OBJECTS");

    add_button_params.clear();
    vector<MappingObject_ptr> options = ctrl->getOptions();
    for(uint i = 0; i < options.size(); i++) {
        add_button_params.push_back(ofParameter<bool>("add " + options.at(i)->name, false));
        add_buttons_panel.add(add_button_params.at(i));
    }

    //CALIBRATION OPTIONS

    calibration_options.setup("CALIBRATION OPTIONS");

    calibration_options.add(ctrl->getCalibrating());
    calibration_options.add(ctrl->getCalBorder());
    calibration_options.add(ctrl->getCalGrey());

    //QUAD LIST

    shape_list.setup("mapping forms");
    ofAddListener(shape_list.elementRemoved, this, &ofx2DMappingView::removeForm);
    ofAddListener(shape_list.elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);
    shape_list.setHeaderBackgroundColor(ofColor::black);

    setSubpanelPositions();

    updateQuadList();


}

void ofx2DMappingView::update() {

    mapping_dst.update();

    for(uint i = 0; i < add_button_params.size(); i++) {
        if(add_button_params.at(i)) {
            MappingObject_ptr obj = ctrl->getOptions().at(i);
            MappingObject_ptr copy = ctrl->getProjector(0)->copyShape(obj);
            ctrl->getProjector(0)->addListeners(copy);
            mapping_dst.updateForms();
            updateQuadList();
            add_button_params.at(i) = false;
        }
    }

}

void ofx2DMappingView::draw() {
    draw(ofPoint(0,0));
}

void ofx2DMappingView::draw(ofPoint pos) {

    if(pos != control_rect.position) {
        control_rect.position = pos;
        mapping_dst.setPosition(control_rect.x, mapping_dst.getPosition().y);
        mapping_dst.rebuild();
        setSubpanelPositions();
    }

    ofPushStyle();

    mapping_dst.draw(show_source);

    shape_list.draw();
    main_options.draw();
    calibration_options.draw();
    add_buttons_panel.draw();

    ofPopStyle();

}

void ofx2DMappingView::updateQuadList() {

    shape_list.clear();

    Projector *p = ctrl->getProjector(0);

    for(uint i = 0; i < p->shapeCount(); i++) {

        MappingObject_ptr mq = p->getShape(i);
        if(mq) {
            string objname = mq->name;
            //insert toggles at beginning of list
            mq->editable.set(objname, true);
            shape_list.add(mq->editable,false);
            shape_list.getListItems().at(0)->setBorderColor(mq->color);
        }
    }
}

void ofx2DMappingView::importSvg() {
    ctrl->importSvg();
    mapping_dst.updateForms();
    shape_list.clear();
    updateQuadList();
}

MappingObject_ptr ofx2DMappingView::addForm(string type, string name, bool at_bottom) {
    MappingObject_ptr mq = ctrl->getProjector(0)->addShape(type, at_bottom);
    mapping_dst.updateForms();
    mq->editable.set(name,true);
    shape_list.add(mq->editable,at_bottom);
    if(at_bottom) {
        shape_list.getListItems().at(shape_list.getListItems().size()-1)->setBorderColor(mq->color);
    }
    else {
        shape_list.getListItems().at(0)->setBorderColor(mq->color);
    }
    return mq;
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

    Projector *p = ctrl->getProjector(0);
    int index = p->shapeCount()-1-data.index;
    ctrl->getProjector(0)->removeListeners(p->getShape(index));
    if(p->removeShape(index)) {
//        if(p->getFirstImageShape() == 0) {
//            addForm("CONTENT_SHAPE", "painting", true);
//        }
        p->updateOutlines();
        mapping_dst.updateForms();
    }

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

    Projector *p = ctrl->getProjector(0);

    int index1 = p->shapeCount()-1-data.old_index;
    int index2 = p->shapeCount()-1-data.new_index;

    bool swapped = p->swapShapes(index1,index2);
    if(swapped) {
        mapping_dst.updateForms();
    }

}

void ofx2DMappingView::setSubpanelPositions() {
    main_options.setPosition(control_rect.x+10, control_rect.y+10);
    calibration_options.setPosition(main_options.getPosition().x + main_options.getWidth() + 10, main_options.getPosition().y);
    mapping_dst.setPosition(main_options.getPosition().x, main_options.getPosition().y+main_options.getHeight()+10);
    add_buttons_panel.setPosition(mapping_dst.getPosition().x + mapping_dst.getWidth()+10, mapping_dst.getPosition().y);
    shape_list.setPosition(add_buttons_panel.getPosition().x, add_buttons_panel.getPosition().y + add_buttons_panel.getHeight()+10);
}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
    mapping_dst.setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
    show_source = show;
}

void ofx2DMappingView::setEditMode(bool &direct_edit) {
    this->direct_edit = direct_edit;
    mapping_dst.setEditMode(direct_edit);
    setSubpanelPositions();
}

FormMapping* ofx2DMappingView::getMappingList() {
    return &mapping_dst;
}
