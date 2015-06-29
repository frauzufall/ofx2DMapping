#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxXmlSettings.h"

class ofx2DMappingHelper {

public:

    static ofPolyline ofPathToOfPolyline(ofPath path, bool straight_lines) {

        ofPolyline polyline;

        vector<ofPath::Command> commands;

        vector<ofPath::Command>& pathCommands = path.getCommands();
        for (uint j=0; j<pathCommands.size(); j++) {
            commands.push_back(pathCommands[j]);
        }

        polyline.clear();
            int curveResolution = ofGetStyle().curveResolution;
            //maybe a better way?
            ofPath tempPath;
            int arcResolution = tempPath.getCircleResolution();

        for(uint i=0; i<commands.size(); i++) {
            switch(commands[i].type) {
                case ofPath::Command::moveTo:
                    polyline.addVertex(commands[i].to);
                    break;
                case ofPath::Command::lineTo:
                    polyline.addVertex(commands[i].to);
                    break;
                case ofPath::Command::curveTo:
                    if(straight_lines)
                        polyline.addVertex(commands[i].to);
                    else
                        polyline.curveTo(commands[i].to, curveResolution);
                    break;
                case ofPath::Command::bezierTo:
                    if(straight_lines)
                        polyline.addVertex(commands[i].to);
                    else
                        polyline.bezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
                    break;
                case ofPath::Command::quadBezierTo:
                    if(straight_lines)
                        polyline.addVertex(commands[i].to);
                    else
                        polyline.quadBezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
                    break;
                case ofPath::Command::arc:
                    if(straight_lines)
                        polyline.addVertex(commands[i].to);
                    else
                        polyline.arc(commands[i].to,commands[i].radiusX,commands[i].radiusY,commands[i].angleBegin,commands[i].angleEnd, arcResolution);
                    break;
                case ofPath::Command::arcNegative:
                    if(straight_lines)
                        polyline.addVertex(commands[i].to);
                    else
                        polyline.arcNegative(commands[i].to,commands[i].radiusX,commands[i].radiusY,commands[i].angleBegin,commands[i].angleEnd, arcResolution);
                    break;
                case ofPath::Command::close:
                    polyline.close();
                    break;
                default: break;
            }
        }

        return polyline;

    }

    static void saveLineAsSvg(string path, ofPolyline line, float width, float height, ofColor strokecolor = ofColor(0)) {

        ofxXmlSettings xml;

        xml.clear();

        xml.addTag("svg");
        xml.addAttribute("svg","id","svg2",0);
        xml.addAttribute("svg","xmlns:rdf","http://www.w3.org/1999/02/22-rdf-syntax-ns#",0);
        xml.addAttribute("svg","xmlns","http://www.w3.org/2000/svg",0);
        xml.addAttribute("svg","width",width,0);
        xml.addAttribute("svg","height",height,0);
        xml.addAttribute("svg","version","1.1",0);
        xml.addAttribute("svg","xmlns:cc","http://creativecommons.org/ns#",0);
        xml.addAttribute("svg","xmlns:dc","http://purl.org/dc/elements/1.1/",0);
        xml.pushTag("svg", 0);
    //        xml.addTag("metadata");
    //        xml.addAttribute("metadata","id","metadata7");
    //        xml.pushTag("metadata");
    //        xml.popTag();
            xml.addTag("g");
            xml.addAttribute("g", "id", "layer1",0);
            xml.pushTag("g");

                int i = 0;

                xml.addTag("path");
                stringstream id_sstr;
                id_sstr << "path" << i;
                xml.addAttribute("path", "id", id_sstr.str(), i);
                stringstream path_sstr;
                path_sstr << "m";
                ofPoint last_p;
                for(uint k = 0; k < line.size(); k++) {
                    ofPoint cur_p = ofPoint(line[k].x, line[k].y);
                    if(k == 0)
                        path_sstr << cur_p.x << "," << cur_p.y;
                    if(k > 0)
                        path_sstr << "," << cur_p.x-last_p.x << "," << cur_p.y-last_p.y;
                    last_p = cur_p;
                }
                //path_sstr << "z";

                xml.addAttribute("path", "d", path_sstr.str(), i);
                xml.addAttribute("path", "fill", "none", i);
                xml.addAttribute("path", "stroke", getColorAsHex(strokecolor), i);

            xml.popTag();

        xml.popTag();

        ofLogNotice("Helper: saveLineAsSvg()", "saving svg to " + path);

        xml.saveFile(path);
    }

    static string getColorAsHex(ofColor c) {
        return getColorAsHex(c.r,c.g,c.b);
    }

    static string getColorAsHex(int r, int g, int b) {
        stringstream ss;
        ss << "#" << getHexCode(r) << getHexCode(g) << getHexCode(b);
        return ss.str();
    }

    static string getHexCode(unsigned char c) {

       stringstream ss;
       ss << uppercase << setw(2) << setfill('0') << std::hex;
       ss << +c;

       return ss.str();
    }

};

