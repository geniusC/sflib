
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_tcpserver.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

//
// Created by skyfire on 2018/8/27 0027.
//
#define SF_DEBUG
#include "sf_logger.hpp"
#include "sf_tcpserver.hpp"

using namespace skyfire;



int main()
{
    auto server = sf_tcpserver::make_server(true);
    if(!server->listen("0.0.0.0",9988))
    {
        sf_debug("listen on 9988 error");
        return -1;
    }
    sf_bind_signal(server,raw_data_coming,[=](SOCKET sock, const byte_array& data){
        //sf_debug("recv",to_string(data));
    },true);
    sf_eventloop loop;
    loop.exec();
}