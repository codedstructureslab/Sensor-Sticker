#include "Arduino.h"
#include "APA.h"

APA_Node::APA_Node(){
}

void APA_Node::attachNextNode(APA_Node *next_APA_Node){
}

uint8_t APA_Node::getId(){
	return id;
}

void APA_Node::update(){
	if(payload -> status == 0){
		parseMessage(readMessage(this));
	}
}

void APA_Node::parseMessage(char *tempbuf){
	int divider_loc = 0;
	int index = 0;
	while(divider_loc == 0 && index < MESSAGE_LENGTH){
		if(tempbuf[index] == '|'){
			divider_loc = index;
		}
	}

}