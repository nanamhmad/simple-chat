#include "client.h"
#include "chatclient.h"

ChatClient::ChatClient () {
  debugf("created client");
  this->fClient = NULL;
}

ChatClient::~ChatClient () {
  this->disconnect();
}

bool ChatClient::connectToServer(string hostname, int port) {
  printf("%s\n", hostname.c_str());
  this->disconnect();
  this->fClient = new Client(hostname.c_str(), port);
  if (this->fClient->connectToServer() == 0) {
    debugf("connected %s at port %d", hostname.c_str(), port);
    return true;
  }
  return false;
}

void ChatClient::disconnect() {
  if (this->fClient != NULL) {
    debugf("disconnecting client");
    this->fClient->closeAll();
    delete this->fClient;
  }
}

ChatClient* tempClient;

void processPacket(int cli, header h, const void* data) {
  if (h.size > 0) {
    if (h.type == kLogin) {
      tempClient->setName((const char*)data, h.size);
      tempClient->setId(h.targetId);
      printf("Welcome %s! Your id is %d", (char*)data, tempClient->fId);
    } else if (h.type == kClientListResponse) {
      vector<string> clients = split(string((char*)data, h.size), '|');
      printf("%d other clients are connected:\n", (int)clients.size());
      for (int i = 0; i < clients.size(); ++i) {
        printf("\t%s\n", clients[i].c_str());
      }
    } else if (h.type == kClientConnectRequest) {
      printf("%s (Y/N)\n", (char*)data);
    }
  }
}

void ChatClient::setName(const char* name, size_t size) {
  this->fName.assign(name, size);
}

void ChatClient::setId(int id) {
  this->fId = id;
}

void ChatClient::read() {
  tempClient = this;
  this->fClient->readData(processPacket);
}

void ChatClient::getAvailableClients() {
  if (this->fClient != NULL) {
    this->fClient->writeData(header(0, kClientListRequest, 7), (void*)"request");
  }
}

void ChatClient::connectToClient(int id) {
  if (this->fClient != NULL) {
    string msg(this->fName);
    msg += " would like to chat with you!";
    this->fClient->writeData(header(id, kClientConnectRequest, msg.length()), (void*)msg.c_str());
  }
}