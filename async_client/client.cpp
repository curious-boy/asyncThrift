#include <stdio.h>
//#include <getopt.h>
//#include <event.h>
#include "event2/event.h"
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include <async/TEvhttpClientChannel.h>
#include "Twitter.h"

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::async;
using namespace ::boost;
using namespace ::thrift::example;
using ::boost::bind;

//#pragma comment(lib,"libthriftnb.lib")


class testClient : public TwitterCobClient
{
public:
	testClient(boost::shared_ptr< ::apache::thrift::async::TAsyncChannel> channel, TProtocolFactory* protocolFactory)
		: TwitterCobClient(channel, protocolFactory)
	{
		bRet = false;
	};

	//返回的结果处理

	virtual void completed__(bool success)
	{
		if (success)
		{

			printf("respone : %s \n", stuRes.strMethod.c_str());   // 输出返回结果
			printf("send: %s \tGet: %s \n",stuRes.strSend.c_str(),stuRes.strGet.c_str());

			if (bRet)
			{
				printf("testBool success\n");
			}
		}
		else
		{
			printf("failed to respone\n");
		}
		fflush(0);
	};

	void testBoolHandle(bool bRet)
	{
		if (bRet)
		{
			printf("testBoolHandle\n");
		}
	}

	thrift::example::twitterReturnStruct stuRes;
	bool bRet;
};

// callback function

static void my_recv_sendString(TwitterCobClient* client)
{
	client->recv_sendString(dynamic_cast<testClient *>(client)->stuRes);
};

static void my_recv_sendLongString(TwitterCobClient* client)
{
	client->recv_sendLongString(dynamic_cast<testClient *>(client)->stuRes);
};

// 元类型的返回结果，除string类型外，需要在这个函数中进行返回结果的处理
static void my_recv_testBool(TwitterCobClient* client)
{
	bool bRet = client->recv_testBool();
	
	if (bRet)
	{
		printf("handle something after recv_testBool\n");
	}
}

static void sendString(testClient& client)   
{
	boost::function<void(TwitterCobClient* client)> cob = boost::bind(&my_recv_sendString, _1);
	client.sendString(cob, "hello");   // 发送并注册回调函数
	printf("sendString end\n");
}

static void sendLongString(testClient& client)   
{
	boost::function<void(TwitterCobClient* client)> cob = boost::bind(&my_recv_sendLongString, _1);
	client.sendLongString(cob,"lhello");
	printf("sendString end\n");
}

static void testBool(testClient& client)
{
	boost::function<void(TwitterCobClient* client)> cob = boost::bind(&my_recv_testBool, _1);
	client.testBool(cob, "");   // 发送并注册回调函数
	printf("testBool end\n");
}

static void DoSimpleTest(const std::string& host, int port)
{
	printf( "running DoSimpleTest( %s, %d) ...\n",host.c_str(), port);
	event_base* base = event_base_new();
	boost::shared_ptr< ::apache::thrift::async::TAsyncChannel>  channel1( new TEvhttpClientChannel( host, "/", host.c_str(), port, base  ) );

	testClient client1( channel1,  new TBinaryProtocolFactory() );
	sendLongString(client1);   // 发送第一个请求
	printf("sendLongString(client1) was sended!\n");
	boost::shared_ptr< ::apache::thrift::async::TAsyncChannel>  channel2( new TEvhttpClientChannel( host, "/", host.c_str(), port, base  ) );
	testClient client2( channel2,  new TBinaryProtocolFactory() );
	sendString(client2);  // 发送第二个请求
	printf("sendString(client2) was sended!\n");

	boost::shared_ptr< ::apache::thrift::async::TAsyncChannel>  channel3( new TEvhttpClientChannel( host, "/", host.c_str(), port, base  ) );
	testClient client3( channel3,  new TBinaryProtocolFactory() );
	testBool(client3);  // 发送第二个请求
	printf("testBool(client3) was sended!\n");

	event_base_dispatch(base);
	event_base_free(base);
	printf( "done DoSimpleTest().\n" );
}

// 初始化socket
bool initSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2,1);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0)
	{
		return false;
	}
	return true;
}

int main( int argc, char* argv[] )
{
	initSocket();
	DoSimpleTest( "192.168.2.179", 9090 );
	return 0;
}