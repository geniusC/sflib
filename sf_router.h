
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_router.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


namespace skyfire
{
    /**
     *  @brief 路由
     */
    class sf_router
    {
        public:
	    virtual ~sf_router() = default;
	    /**
         * 获取优先级
         * @return 优先级
         */
        virtual int get_priority() const;
    };

}
#pragma clang diagnostic pop