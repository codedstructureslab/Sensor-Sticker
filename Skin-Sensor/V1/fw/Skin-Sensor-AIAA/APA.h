#include "Arduino.h"

#define MESSAGE_LENGTH 255


struct APA_Message{
	//Status integer
	// 0: empty
	// 1: input (to be routed) 
	// 2: output (to be sent)	
	uint8_t status; 
	char *path;
	char *message;
};

class APA_Node{
	public:
		APA_Node();
		void begin(void);
		void update();
		void attachNextNode(APA_Node *next_APA_Node);
		uint8_t getId();
	private:
		uint8_t id;
		APA_Node *next_node;
		APA_Message *payload;
		void parseMessage(char *tempbuf);
		void route();
};

char* readMessage(APA_Node *node);
