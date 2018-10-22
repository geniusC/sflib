
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_router_shared_ptr_compare.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_router_shared_ptr_compare.h"

namespace skyfire
{

    template<typename RouterType>
    bool sf_router_shared_ptr_compare__<RouterType>::operator()(const std::shared_ptr<RouterType> &router1,
                                                                const std::shared_ptr<RouterType> &router2) const {
        return router1->get_priority() < router2->get_priority();
    }
} // namespace skyfire