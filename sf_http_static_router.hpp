

/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_static_router.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"
#pragma ide diagnostic ignored "bugprone-lambda-function-name"

#include "sf_http_static_router.h"
#include "sf_stdc++.h"
#include "sf_msvc_safe.h"

namespace skyfire
{
    using namespace std::literals;

    inline std::shared_ptr<sf_http_router>
    make_static_router(std::string path, const std::vector<std::string> &methods, std::string charset, bool deflate,
                       unsigned long long int max_file_size, int priority)
    {
        std::string pattern = ".+";
        if (!path.empty())
        {
	        const auto ch = path[path.length() - 1];
            if (ch == '\\' || ch == '/')
            {
                path.pop_back();
            }
        }
        std::function<void(const sf_http_request &, sf_http_response &, const std::string &)> callback =
                [=](const sf_http_request &req, sf_http_response &res, const std::string &raw_url)
                {
                    // sf_debug(raw_url);
                    std::string url;
                    std::unordered_map<std::string, std::string> param;
                    std::string frame;
                    sf_parse_url(raw_url, url, param, frame);
                    auto abs_path = path + url;

                    sf_http_header header;
                    byte_array body_data;

                    header.set_header("Date", sf_make_http_time_str());

                    auto _401_res = [&]
                    {
                        res.set_status(401);
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                        body_data = to_byte_array(
                                "<p>" + url + " bad request! (" + std::to_string(max_file_size) +
                                ")</p>");
                    };
                    auto _404_res = [&]
                    {
                        res.set_status(404);
                        body_data = to_byte_array("<p>" + url + " not found</p>");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                    };
                    auto _416_res = [&]
                    {
                        res.set_status(416);
                        body_data = to_byte_array("<p>" + url + " Requested Range Not Satisfiable</p>");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                    };


                    if (sf_is_dir(abs_path))
                    {
                        _404_res();
                    } else
                    {
                        auto file_size = sf_get_file_size(abs_path);
                        if (file_size == -1)
                        {
                            _404_res();
                        } else
                        {
                            if (req.get_header().has_key("Range"))
                            {
                                sf_debug("Range found");
                                auto range_header = req.get_header().get_header_value("Range", "");
                                auto range_content_list = sf_split_string(range_header, "=");
                                if (range_content_list.size() < 2)
                                {
                                    _401_res();
                                } else
                                {
                                    auto range_list = sf_split_string(range_content_list[1], ",");
                                    for (auto &p:range_list)
                                    {
                                        p = sf_string_trim(p);
                                    }
                                    if (range_list.size() > 1)
                                    {
                                        std::vector<sf_http_response::multipart_info_t> multipart_info_vec;
                                        bool error_flag = false;
                                        for(auto& range_str:range_list)
                                        {
#ifdef _MSC_VER
											long long start = LLONG_MAX;
#else
											long long start = LONG_LONG_MAX;
#endif // _MSC_VER
                                            long long end = -1;
#ifdef _MSC_VER
                                            if (scanf_s(range_str.c_str(), "%lld-%lld", &start, &end) < 1)
#else
											if (sscanf(range_str.c_str(), "%lld-%lld", &start, &end) < 1)
#endif
                                            {
                                                error_flag = true;
                                                _416_res();
                                                break;
                                            }
                                            sf_http_response::multipart_info_t tmp_part;
                                            tmp_part.type = sf_http_response::multipart_info_t::multipart_info_type ::file;
                                            tmp_part.file_info = sf_http_response::response_file_info_t{abs_path,start,end};
                                            multipart_info_vec.emplace_back(tmp_part);
                                        }
                                        if(!error_flag)
                                        {
                                            res.set_header(header);
                                            res.set_multipart(multipart_info_vec);
                                            return;
                                        }
                                    } else
                                    {
#ifdef _MSC_VER
	                                    auto start = LLONG_MAX;
#else
										auto start = LONG_LONG_MAX;
#endif // _MSC_VER
                                    	long long end=-1;
#ifdef _MSC_VER
										if (sscanf_s(range_list[0].c_str(), "%lld-%lld", &start, &end) < 1)
#else
										if (sscanf(range_list[0].c_str(), "%lld-%lld", &start, &end) < 1)
#endif // _MSC_VER
                                        {
                                            _401_res();
                                        } else
                                        {
                                            res.set_header(header);
                                            res.set_file(sf_http_response::response_file_info_t{abs_path,start,end});
                                            return;
                                        }
                                    }
                                }
                            } else
                            {
                                sf_debug("big file", abs_path);
                                res.set_header(header);
                                res.set_file(sf_http_response::response_file_info_t{abs_path,0,-1});
                                return;
                            }
                        }
                    }

                    auto accept = req.get_header().get_header_value("Accept-Encoding", "");
                    auto accept_list = sf_split_string(accept, ",");
                    for (auto &p:accept_list)
                        p = sf_string_trim(p);

                    if (deflate && std::find_if(accept_list.begin(), accept_list.end(), [=](const std::string &str)
                    {
                        return sf_equal_nocase_string(str, "deflate");
                    }) != accept_list.end())
                    {
                        body_data = sf_deflate_compress(body_data);
                        header.set_header("Content-Encoding", "deflate");
                    }
                    res.set_header(header);
                    res.set_body(body_data);
                };
        return std::make_shared<sf_http_router>(pattern, callback, methods, priority);
    }
}

#pragma clang diagnostic pop