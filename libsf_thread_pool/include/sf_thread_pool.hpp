
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_thread_pool.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_thread_pool 线程池
 */

#pragma once

#include "sf_thread_pool.h"

namespace skyfire
{

    inline sf_thread_pool::sf_thread_pool(size_t thread_count) {
        add_thread__(thread_count);
    }

    inline void sf_thread_pool::thread_run__(sf_thread_pool *this__) {
        while (true)
        {
            while ((!this__->is_exit__) && (this__->is_pause__ || this__->task_deque__.empty()))
            {
                ++this__->busy_thread_num__;
                std::unique_lock<std::mutex> lck_cv(this__->mu_thread__cv__);
                this__->thread_cv__.wait(lck_cv);
                --this__->busy_thread_num__;
            }
            if (this__->is_exit__)
            {
                break;
            }
            {
                auto flag_empty = true;
                std::function<void()> task;
                {
                    std::lock_guard<std::mutex> lck(this__->mu_task_deque__);
                    if (!this__->task_deque__.empty())
                    {
                        flag_empty = false;
                        task = this__->task_deque__.front();
                        this__->task_deque__.pop_front();
                    }
                }
                if (!flag_empty)
                {
                    task();
                    this__->wait_finish_cv__.notify_all();
                }
            }
        }
    }

    inline void sf_thread_pool::add_thread__(size_t num) {
        if (num < 1)
        {
            num = 1;
        }
        thread_count__ += num;
        for (auto i = 0; i < num; ++i)
        {
            auto p_thread = std::make_shared<std::thread>(thread_run__, this);
            thread_vec__.push_back(p_thread);
        }
    }

    inline void sf_thread_pool::wait_all_task_finished() {
        while (!task_deque__.empty())
        {
            std::unique_lock<std::mutex> lck_cv(mu_wait_finish__);
            wait_finish_cv__.wait(lck_cv);
        }
    }

    inline void sf_thread_pool::clear_task() {
        std::lock_guard<std::mutex> lck(mu_task_deque__);
        task_deque__.clear();
    }

    inline void sf_thread_pool::clear_thread() {
        is_pause__ = false;
        is_exit__ = true;
        thread_cv__.notify_all();
        for (auto &p : thread_vec__)
        {
            p->join();
        }
        thread_vec__.clear();
    }

    inline size_t sf_thread_pool::get_busy_thread_count() const {
        return static_cast<size_t>(busy_thread_num__);
    }

    inline size_t sf_thread_pool::get_thread_count() const {
        return thread_count__;
    }

    inline void sf_thread_pool::add_thread(size_t thread_num) {
        add_thread__(thread_num);
    }

    inline void sf_thread_pool::resume() {
        is_pause__ = false;
        thread_cv__.notify_all();
    }

    template<typename Func, typename... T>
    void sf_thread_pool::add_task(Func func, T &&... args) {
        std::lock_guard<std::mutex> lck(mu_task_deque__);
        task_deque__.push_back(std::bind(func, std::forward<T>(args)...));
        thread_cv__.notify_all();
    }

    inline sf_thread_pool::~sf_thread_pool() {
        clear_thread();
    }

    inline void sf_thread_pool::add_task(std::function<void()> func) {
        std::lock_guard<std::mutex> lck(mu_task_deque__);
        task_deque__.push_back(func);
        thread_cv__.notify_all();
    }

    inline void sf_thread_pool::pause() {
        is_pause__ = true;
    }


}
