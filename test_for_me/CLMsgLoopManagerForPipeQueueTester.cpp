#include <gtest/gtest.h>
#include "LibExecutive.h"

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;
static int g_for_msg1 = 0;
static int g_for_msg2 = 0;
static int g_for_msg1_dese = 0;
static int g_for_msg2_dese = 0;
static int g_for_msg1_se = 0;
static int g_for_msg2_se = 0;

class CLMsg1ForCLMsgLoopManagerForPipeQueue : public CLMessage
{
public:
	CLMsg1ForCLMsgLoopManagerForPipeQueue() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue()
	{
		g_for_msg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue : public CLMessage
{
public:
	CLMsg2ForCLMsgLoopManagerForPipeQueue() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue()
	{
		g_for_msg2++;
	}

	long i;
	int j;
};

class CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *pMsg1 = new CLMsg1ForCLMsgLoopManagerForPipeQueue;

		int *pValue = &(pMsg1->i);

		dataVec.ReadData((char*)(pValue), sizeof(long), sizeof(int));
		pValue = &(pMsg1->j);
		dataVec.ReadData((char*)(pValue), sizeof(long) + sizeof(int), sizeof(int));

		*pMsg = pMsg1;

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg1_dese++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *pMsg2 = new CLMsg2ForCLMsgLoopManagerForPipeQueue;

		long *pValue = &(pMsg2->i);

		dataVec.ReadData((char*)(pValue), sizeof(long), sizeof(long));
		int *pValue1 = &(pMsg2->j);
		dataVec.ReadData((char*)(pValue1), sizeof(long) + sizeof(long), sizeof(int));

		*pMsg = pMsg2;

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg2_dese++;
	}
};

class CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue *>(pMsg);		

		int length = sizeof(unsigned long) + sizeof(int) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + sizeof(unsigned long));
		*pi = p->i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(int));
		*pj = p->j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg1_se++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue *>(pMsg);		

		int length = sizeof(unsigned long) + sizeof(long) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		long *pi = (long *)(pBuf + sizeof(unsigned long));
		*pi = p->i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(long));
		*pj = p->j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg2_se++;
	}
};

class CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLMessage *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p, new CLDataPostResultNotifier).IsSuccess());

		p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p, new CLDataPostResultNotifier).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		CLMessage *p1 = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p1, new CLDataPostResultNotifier).IsSuccess());

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, PrivateQueueForSelfPostMsg)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForPipeQueue(new CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_IN_PROCESS);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 2);
}

class CLThreadForTestingCLMsgLoopManagerForPipeQueue_PrivateQueueForPostMsg : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLMessage *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p, new CLDataPostResultNotifier).IsSuccess());

		p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p, new CLDataPostResultNotifier).IsSuccess());

		return CLStatus(0, 0);
	}
};

class CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLThread *p = new CLThread(new CLThreadForTestingCLMsgLoopManagerForPipeQueue_PrivateQueueForPostMsg, false);
		EXPECT_TRUE(p->Run().IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, PrivateQueueForfPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	CLMessageLoopManager *pM = new CLMsgLoopManagerForPipeQueue(new CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_IN_PROCESS);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}
/////
class CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLMessagePoster sender(new CLDataPosterChannelByNamedPipeMaintainer(test_pipe_name, true), new CLMultiMsgSerializer(), new CLProtoEncapForDefaultMsgFormat(), new CLEvent(test_pipe_name, true));
		sender.Initialize(0);
		EXPECT_TRUE(sender.RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());
		EXPECT_TRUE(sender.RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());

		sender.PostMessage(new CLMsg1ForCLMsgLoopManagerForPipeQueue, new CLDataPostResultNotifier(true));
		sender.PostMessage(new CLMsg2ForCLMsgLoopManagerForPipeQueue,new CLDataPostResultNotifier(true));

		sender.UnInitialize(0);

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, SharedQueueForSelfPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;
	g_for_msg1_dese = 0;
	g_for_msg2_dese = 0;
	g_for_msg1_se = 0;
	g_for_msg2_se = 0;

	CLMsgLoopManagerForPipeQueue *pM = new CLMsgLoopManagerForPipeQueue(new CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_BETWEEN_PROCESS);
	pM->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer);
	pM->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 2);
	EXPECT_EQ(g_for_msg2, 2);
	EXPECT_EQ(g_for_msg1_dese, 1);
	EXPECT_EQ(g_for_msg2_dese, 1);
	EXPECT_EQ(g_for_msg1_se, 1);
	EXPECT_EQ(g_for_msg2_se, 1);
}

class CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, true);
		EXPECT_TRUE((process->Run((void *)"../test_for_exe_forgzh/test_for_processProvider/main")).IsSuccess());

		return CLStatus(0, 0);
	}
 
	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, SharedQueueForProcessPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;
	g_for_msg1_dese = 0;
	g_for_msg2_dese = 0;
	g_for_msg1_se = 0;
	g_for_msg2_se = 0;

	CLMsgLoopManagerForPipeQueue *pM = new CLMsgLoopManagerForPipeQueue(new CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_BETWEEN_PROCESS);
	pM->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer);
	pM->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
	EXPECT_EQ(g_for_msg1_dese, 1);
	EXPECT_EQ(g_for_msg2_dese, 1);
	EXPECT_EQ(g_for_msg1_se, 0);
	EXPECT_EQ(g_for_msg2_se, 0);
}
