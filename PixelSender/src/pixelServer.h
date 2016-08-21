#include "ofxNetwork.h"
#include "ofMain.h"

#define NUM_BYTES_IN_IMG (320 * 240 * 3)

enum serverState{
	NONE, READY, SENDING, COMPLETE	
};

class pixelServer{
	public:
	
	//-----------------------------------------------------
	pixelServer(){
		state		= NONE;
		totalBytes	= NUM_BYTES_IN_IMG;
	}

	//-----------------------------------------------------
	int getState(){
		return state;
	}

	//-----------------------------------------------------
	string getStateStr(){
		if(state == NONE)return "NONE";
		else
		if(state == READY)return "READY";
		else
		if(state == SENDING)return "SENDING";
		else
		if(state == COMPLETE)return "COMPLETE";
		
		return "ERR";
	}
	
	//-----------------------------------------------------
	float getPctSent(){
		return ( (float)numSentBytes / totalBytes ) * 100;
	}	

	//-----------------------------------------------------
	void setup(int port = 11999){
	   //setup the server to listen on 11999 
	   TCP.setup(port);
	   state = READY;
	} 

	//-----------------------------------------------------
	void sendPixels(unsigned char * pixelData, int totalBytes_) {
		if( state == NONE ) return;
				
		memcpy(pixels, pixelData, totalBytes);
		state			= SENDING;
		numSentBytes	= 0;
    totalBytes = totalBytes_;
	} 

	//-----------------------------------------------------
	void update(int numToSend = 1024){
		
		if( state == SENDING && numSentBytes < totalBytes ){
			if( numToSend + numSentBytes > totalBytes ){
				numToSend -= ( (numToSend + numSentBytes) - totalBytes );
			}
		
			TCP.sendRawBytesToAll( (char *)&pixels[numSentBytes], numToSend);
			numSentBytes += numToSend;
		}
		
		if( numSentBytes >= totalBytes ){
			state = COMPLETE;
		}
	}


	serverState state;
	int numSentBytes;
	int totalBytes;
	
	ofxTCPServer TCP;
	
	unsigned char pixels[NUM_BYTES_IN_IMG];
};