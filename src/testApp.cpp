#include "testApp.h"
using namespace std;
//--------------------------------------------------------------
void testApp::setup(){
    
	looping = false;
	started = false;
	fullscreen = false;
	count = 0;
	
	// Load initial settings
	loadSettings("settings.xml");
	ofSetVerticalSync(true);
	
	// open an outgoing connection to HOST:PORT
	sender1.setup( host1, port1 );
	sender2.setup( host2, port2 );
    sender3.setup( host3, port3 );
	sender4.setup( host4, port4 );
    sender5.setup( host5, port5 );
	sender6.setup( host6, port6 );
	
	// Load movie files
    movie1.setPixelFormat(OF_PIXELS_RGBA);
	movie1.loadMovie("movies/movie.mov");

    currentMovie = movie1;
    
    //play first movie
    currentMovie.play();
	
	// Turns looping off
	if (!looping) {
		currentMovie.setLoopState(OF_LOOP_NONE);
	} else {
		currentMovie.setLoopState(OF_LOOP_NORMAL);
	}
	
    //set background
    ofBackground(0, 0, 0);
    
    //set initial xstep and ystep
    yStep = 10;
    xStep = 3;
    
    //allocate fbofloat
    fboFloat.allocate(currentMovie.getWidth(), currentMovie.getHeight(), GL_RGBA32F_ARB);
    
    //set draw rutt etra = false
    drawRuttEtra = false;
    
    //clear the fbo
    fboFloat.begin();
	ofClear(0,0,0,0);
    fboFloat.end();
    
    //load bloom shader
    bloomShader.load("shaders/bloom.vert", "shaders/bloom.frag");
    
    //set debug to false
    debug = false;
    
    //timeline setup
    timeline.setup(); //registers events
    timeline.setDurationInSeconds(60); //sets time
    timeline.setLoopType(OF_LOOP_NORMAL); //turns the timeline to loop
    
    //add a tracks, etc
    timeline.addCurves("yStep", ofRange(5, 100));
    timeline.addSwitches("drawRuttEtra");
}

//--------------------------------------------------------------
void testApp::update(){
    //enable alpha blending (move to setup?)
    ofEnableAlphaBlending();

    //update current movie
    currentMovie.update();

    //if we get a new frame and if we're drawing rutt etra, get the pixels from the movie
    if(currentMovie.isFrameNew()){
        
        if(drawRuttEtra){
        
        vidPixels = currentMovie.getPixelsRef();
        
        }
  
    }
    
    //synching video stuff. remove for hallway installation. we're synching rutt etra!
	float p = currentMovie.getPosition();
    
    
	//printf("%f\n",p);
	
	// Broadcast current position information of movie
	if ((!started || count % howOften == 0)) {
		ofxOscMessage m;
		m.setAddress( "/movie/position" );
		m.addFloatArg(p);
		sender1.sendMessage(m);
		sender2.sendMessage(m);
		started = true;
	}
    
     
	count++;

    //if rutt etra, draw our mesh into the rutt etra
    if(drawRuttEtra){
        fboFloat.begin();
        drawMesh();
        fboFloat.end();
        fboFloat.getTextureReference(0);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //draw debug screen and timeline
   
    
    //timeline code
    
    yStep = timeline.getValue("yStep");
    
    if(timeline.isSwitchOn("drawRuttEtra")){
        drawRuttEtra = true;
    }else{
        drawRuttEtra = false;
    }


//uncomment to hide cursor on fullscreen
//	if (fullscreen) {
//		ofHideCursor();
//	}
    
      
    // if draw rutt etra is true, draw the rutt etra into an fbo
    if(drawRuttEtra){

     //   ofSetColor(0,0,0);
        bloomShader.begin();
        bloomShader.setUniformTexture("texture", fboFloat.getTextureReference(), 1);
        fboFloat.draw(0,0);
        bloomShader.end();

     
}
    else if(!drawRuttEtra){
        //draw the movie
        currentMovie.draw(0,0,currentMovie.getWidth(),currentMovie.getHeight());
    } 
    
    //draw debug screen and timeline
    if(debug) {
        drawDebug();
    }
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    if(key=='w'){
        if(yStep<100)       
            yStep++;
        
        ofxOscMessage yStepMessage;
        yStepMessage.setAddress( "/movie/yStepMessage" );
        
        yStepMessage.addIntArg(yStep);
        sender1.sendMessage(yStepMessage);
        sender2.sendMessage(yStepMessage);
    }
    
    if(key=='s'){
        if(yStep>1)
            yStep--;
        ofxOscMessage yStepMessage;
        yStepMessage.setAddress( "/movie/yStepMessage" );
        
        yStepMessage.addIntArg(yStep);
        sender1.sendMessage(yStepMessage);
        sender2.sendMessage(yStepMessage);
    }
    
    
    if(key=='d'){
        if(xStep<100)       
            xStep++;
        
        ofxOscMessage xStepMessage;
        xStepMessage.setAddress( "/movie/xStepMessage" );
        
        xStepMessage.addIntArg(yStep);
        sender1.sendMessage(xStepMessage);
        sender2.sendMessage(xStepMessage);
        
    }
    
    if(key=='a'){
        if(xStep>1)
            xStep--;
        
        ofxOscMessage xStepMessage;
        xStepMessage.setAddress( "/movie/xStepMessage" );
        
        xStepMessage.addIntArg(yStep);
        sender1.sendMessage(xStepMessage);
        sender2.sendMessage(xStepMessage);
    }
    
    if(key=='r'){
        
        drawRuttEtra = !drawRuttEtra;
        
        ofxOscMessage ruttEtraMessage;
        ruttEtraMessage.setAddress( "/movie/ruttEtraMessage" );

        ruttEtraMessage.addIntArg(drawRuttEtra);
        sender1.sendMessage(ruttEtraMessage);
        sender2.sendMessage(ruttEtraMessage);
        
    }

    if(key=='d'){
        debug = !debug;
        
    }  

}



void testApp::loadSettings(string fileString){
	
	string host_address1;
	string host_address2;
    string host_address3;
	string host_address4;
    string host_address5;
	string host_address6;
    
	string filename;
	
	//--------------------------------------------- get configs
    ofxXmlSettings xmlReader;
	bool result = xmlReader.loadFile(fileString);
	if(!result) printf("error loading xml file\n");
	
	host_address1 = xmlReader.getValue("settings:client1:address","test",0);
	port1 = xmlReader.getValue("settings:client1:port",5204,0);
	host1 = (char *) malloc(sizeof(char)*host_address1.length());
	strcpy(host1, host_address1.c_str());
	
	host_address2 = xmlReader.getValue("settings:client2:address","test",0);
	port2 = xmlReader.getValue("settings:client2:port",5204,0);
	host2 = (char *) malloc(sizeof(char)*host_address2.length());
	strcpy(host2, host_address2.c_str());
    
    host_address3 = xmlReader.getValue("settings:client3:address","test",0);
	port3 = xmlReader.getValue("settings:client3:port",5204,0);
	host3 = (char *) malloc(sizeof(char)*host_address3.length());
	strcpy(host3, host_address3.c_str());
	
	host_address4 = xmlReader.getValue("settings:client4:address","test",0);
	port4 = xmlReader.getValue("settings:client4:port",5204,0);
	host4 = (char *) malloc(sizeof(char)*host_address4.length());
	strcpy(host4, host_address4.c_str());
    
    host_address5 = xmlReader.getValue("settings:client5:address","test",0);
	port5 = xmlReader.getValue("settings:client5:port",5204,0);
	host5 = (char *) malloc(sizeof(char)*host_address5.length());
	strcpy(host5, host_address5.c_str());
	
	host_address6 = xmlReader.getValue("settings:client6:address","test",0);
	port6 = xmlReader.getValue("settings:client6:port",5204,0);
	host6 = (char *) malloc(sizeof(char)*host_address6.length());
	strcpy(host6, host_address6.c_str());
	
	filename = xmlReader.getValue("settings:movie:","test",0);
	movieFile = (char *) malloc(sizeof(char)*filename.length());
	strcpy(movieFile, filename.c_str());
	
	howOften = xmlReader.getValue("settings:howoften",60,0);
	
	int w = xmlReader.getValue("settings:dimensions:width", 640, 0);
	int h = xmlReader.getValue("settings:dimensions:height", 480, 0);
	
	movieWidth = xmlReader.getValue("settings:dimensions:movieWidth", 640, 0);
	movieHeight = xmlReader.getValue("settings:dimensions:movieHeight", 480, 0);
	
	movieX = xmlReader.getValue("settings:dimensions:movieX", 640, 0);
	movieY = xmlReader.getValue("settings:dimensions:movieY", 480, 0);
	
	ofSetWindowShape(w, h);
	
	if(xmlReader.getValue("settings:loop", "false", 0).compare("true") == 0) {
		looping = true;
	}
	
	if(xmlReader.getValue("settings:go_fullscreen", "false", 0).compare("true") == 0) {
		fullscreen = true;
		ofSetFullscreen(true);
	}
}

void testApp::drawMesh(){
    //most of this is based off the fbo trails example :)
    
    //we clear the fbo if c is pressed.
	//this completely clears the buffer so you won't see any trails
	if( ofGetKeyPressed('c') ){
		ofClear(0,0,0, 0);
	}
	
	//some different alpha values for fading the fbo
	//the lower the number, the longer the trails will take to fade away.
	fadeAmnt = 40;
	if(ofGetKeyPressed('1')){
		fadeAmnt = 1;
	}else if(ofGetKeyPressed('2')){
		fadeAmnt = 5;
	}else if(ofGetKeyPressed('3')){
		fadeAmnt = 15;
	}
    
    ofPushStyle();
    ofFill();
	ofSetColor(0,0,0, fadeAmnt);
	ofRect(0,0,currentMovie.getWidth(),currentMovie.getHeight());
    ofPopStyle();
    
    ofMesh mesh;
    
    int rowCount = 0;
    for (int y = 0; y<currentMovie.height; y+=yStep){
        //   ofNoFill();
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        
        if (rowCount % 2 == 0) {
            for (int x = 0; x < currentMovie.width; x += xStep){
                ofColor curColor = vidPixels.getColor(x, y);
                mesh.addColor(ofColor(curColor, 255));
                // mesh.addVertex(ofVec3f(x, y, z+curColor.getBrightness() * 0.3 + fftSmoothed[y]*30.f));
                if(y<currentMovie.getWidth()/2){
                    mesh.addVertex(ofVec3f(x, y, curColor.getBrightness() * 0.3));
                } else {
                    mesh.addVertex(ofVec3f(x, y, curColor.getBrightness() * 0.3));
                }
            }
        } else {
            for (int x = currentMovie.width-1; x >= 0; x -= xStep){
                ofColor curColor = vidPixels.getColor(x, y);
                mesh.addColor(ofColor(curColor, 255));
                if(y<currentMovie.getWidth()/2){
                    mesh.addVertex(ofVec3f(x, y, curColor.getBrightness() * 0.3 ));
                } else {
                    mesh.addVertex(ofVec3f(x, y, curColor.getBrightness() * 0.3));
                }
            }
        }
        rowCount++;
        
    }
    mesh.draw();
}

void testApp::drawDebug(){
    
    ofSetColor(255);
    string msg = "fps: " + ofToString(ofGetFrameRate()) + "\n"
    + "yStep: " + ofToString(yStep) + "\n"
    + " xStep: " + ofToString(xStep) + "\n";
    ofDrawBitmapString(msg, 10, 20);
    timeline.draw();

    
}

