#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_rpcserver.h"
#include "sf_component_context.h"
#include "sf_msg_bus_server.h"

#include <list>
#include <memory>
#include <string>
#include <functional>
#include <shared_mutex>

namespace skyfire
{
    class sf_component_server : public sf_nocopy<sf_object>
    {
        // 组件注册信号
        SF_REG_SIGNAL(component_reged, const std::string&);
        // 组件反注册信号
        SF_REG_SIGNAL(component_unreged, const std::string&);
        // 消息到来
        SF_REG_SIGNAL(msg_coming, std::string, byte_array);

    private:
        std::map<std::string, byte_array> public_area__;
        std::shared_ptr<sf_rpcserver> rpc_server__ = sf_rpcserver::make_server();
        std::shared_ptr<sf_msg_bus_server> msg_bus_server__ = sf_msg_bus_server::make_server();
        std::map<std::string, sf_component_context_t> component_context__;
        std::recursive_mutex mu_public__;
        std::recursive_mutex mu_private__;
        std::string data_dir__;

        bool has_component_name__(const std::string &name);
        bool has_component_token__(const std::string &token);
        std::string get_token_by_name__(const std::string &name);

        std::string reg_component__(std::string name);

        std::list<std::string> get_component_list__(std::string token);

        void clear_private_area__(std::string token);
        byte_array get_private_data__(std::string token,std::string key);
        void set_private_data__(std::string token,std::string key, byte_array value);
        void delete_private_data__(std::string token,std::string key);
        bool has_private_data__(std::string token,std::string key) ;

        bool save_private_data__(std::string token);
        void load_private_data__(std::string token);

        void unreg_component__(std::string token);
        void clear_public_area__(std::string token);
        byte_array get_public_data__(std::string token, std::string key);
        void set_public_data__(std::string token, std::string key, byte_array value);
        void delete_public_data__(std::string token, std::string key);
        bool has_public_data__(std::string token, std::string key) ;
        addr_info_t get_msg_bus_server_addr__(std::string token);


        sf_component_server(const std::string& data_dir);
    public:

        static std::shared_ptr<sf_component_server> make_server(const std::string& data_dir);

        bool listen(const std::string& ip, unsigned short port);
        std::map<std::string,sf_component_context_t> get_component_context();


        void unreg_component(std::string name);
        void clear_public_area();
        byte_array get_public_data(std::string key);
        void set_public_data(std::string key, byte_array value);
        void delete_public_data(std::string key);
        bool has_public_data(std::string key) ;
        addr_info_t get_msg_bus_server_addr();

        bool save_data();
        void load_data();

        bool save_public_data();
        void load_public_data();

        void send_msg(const std::string& type,const byte_array &data);
    };
}