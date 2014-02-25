#ifndef CLSocket_H
#define CLSocket_H

#include "DefinitionForConst.h"
#include "CLStatus.h"

class CLBaseSocket;
class CLIOVectors;
class CLSocketAddress;

class CLSocket
{
public:
	CLSocket(const char *pstrServiceOrPort, int SocketType = SOCKET_TYPE_TCP, bool bBlock = false, const char *pstrHostNameOrIP = 0, int backlog = LISTEN_BACKLOG);
	CLSocket(int SocketFd, bool bBlock);
	CLSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, int SocketType = SOCKET_TYPE_TCP, bool bBlock = false);
	virtual ~CLSocket();

	int GetSocket();

	CLStatus Accept(CLSocket **ppSocket);

	CLStatus Connect();
	void NotifyConnectResults(bool bResults);

	CLStatus Read(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress = 0);
	CLStatus Write(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress = 0);

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	CLBaseSocket *m_pSocket;
};

#endif