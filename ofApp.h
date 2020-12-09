#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "pack2.h"
#include "ofxHersheyFont.h"
class ofApp : public ofBaseApp{

	public:
		
		void setup();
		void update();
		void drawNonOverlapping();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void loadJson();
		void loadBoards();
		void makeBoard(bool first);
		void packRects();
		float drawPacked(int index);
		ofxImGui::Gui gui;
		ofxHersheyFont font;
		ImVec4 backgroundColor;
		std::vector<std::string> fileNames;
		std::vector<ofFile> files;
		std::vector<std::string> blankIDs;
		ofDirectory jsonDirectory;
		std::vector<std::tuple<int,float,float>> blankData;
		std::vector<std::pair<float, float>> boards;
		std::vector<std::pair<float, float>> bigBoards;
		std::vector<std::pair<float, float>> smallBoards;
		std::vector<std::string> boardStrings;
		std::vector<std::string> packedStrings;
		std::vector<int> packedIndices;
		bool loadedJson;
		bool addBoard;
		bool packed;
		pack2::cPackEngine E;


		
};
