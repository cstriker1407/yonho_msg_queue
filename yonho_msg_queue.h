/*
一个消息队列实现
1. written by cstriker1407@yeah.net   http://cstriker1407.info/blog/
2. Follow BSD
*/

#ifndef YONHO_MSG_QUEUE_H
#define YONHO_MSG_QUEUE_H

#include <stdio.h>
#include <string.h>
#define yh_memset memset

#define yh_null 0
#define yh_debug(fmt, ...) printf("[DBG %4d]"fmt"\r\n", __LINE__, ##__VA_ARGS__)
#define yh_error(fmt, ...) printf("[ERR %4d]"fmt"\r\n", __LINE__, ##__VA_ARGS__)

#include <stdlib.h>
#define yh_calloc          calloc
#define yh_free            free

#ifdef __cplusplus
extern "C" {
#endif

//消息队列最大支持的消息的类型
#define YH_MAX_MSG_TYPE         20

//消息队列消息ID类型定义
typedef int yh_msg_id;

//消息队列消息回调函数定义
typedef void (*yonho_msg_process_cb)(yh_msg_id msg_id, void *p_param);

//消息队列消息节点类型定义
typedef struct
{
    yonho_msg_process_cb msg_cb;
    yh_msg_id msg_id;
    void *p_param;
}yh_msg_ctx;

int yonho_init(
                void (*sys_lock)(void),                          //加锁
                void (*sys_unlock)(void)                         //解锁
                );

int yonho_register_msg_process(yh_msg_id *p_msg_id, yonho_msg_process_cb msg_cb);

int yonho_post_msg(yh_msg_id msg_id, void *p_param);

int yonho_get_msg_node(yh_msg_ctx *p_msg_ctx);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // YONHO_MSG_QUEUE_H
