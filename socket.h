#ifndef SOCKET_H
#define SOCKET_H

#include "utils.h"

class Socket {
  public:
    Socket();
    virtual ~Socket();

    bool isConnected() { return fConnected; }

    int write(void* data, size_t size);
    int read(void (*onRead)(int cid, const void* data, size_t size));

  protected:
    struct header {
      int type;
    };
    /**
    * Create a socket and return its file descriptor. Returns -1 on failure
    */
    int createSocket();

    /**
    * Close the socket specified by the socket file descriptor argument. Will
    * update fMaxfd and working set properly
    */
    void closeSocket(int sockfd);

    /**
    * Called when a broken or terminated connection has been detected. Closes
    * the socket file descriptor and removes it from the master set by default.
    * Override to handle broken connections differently
    */
    virtual void onFailedConnection(int sockfd);

    /**
     * Set the socket specified by the socket file descriptor as nonblocking
     */
    void setNonBlocking(int sockfd);

    /**
    * Add the socket specified by the socket file descriptor to the master
    * file descriptor set, which is used to in the select() to detect new data
    * or connections
    */
    void addToMasterSet(int sockfd);

    bool    fConnected;
    bool    fReady;
    int     fMaxfd;
    int     fPort;
    int     fSockfd;

    /**
    * fMasterSet contains all the file descriptors to be used for read/write.
    * For clients, this only contains the client socket. For servers, this
    * contains all the file descriptors associated with established connections
    * to clients
    */
    fd_set  fMasterSet;
};
#endif