/* @file:test.thrift */

namespace cpp thrift.example

struct twitterReturnStruct{
    1:string strMethod,
    2:string strSend,
    3:string strGet
}

service Twitter{
    
    twitterReturnStruct sendLongString(1:string data);
    twitterReturnStruct sendString(1:string data);
    bool testBool(1:string data);
}