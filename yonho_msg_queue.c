/*
一个消息队列实现
1. written by cstriker1407@yeah.net   http://cstriker1407.info/blog/
2. Follow BSD
*/

#include "yonho_msg_queue.h"


/*
  head             tail
  [] <- [] <- [] <- []
*/
typedef struct __yh_msg_node
{
    struct __yh_msg_node *p_next_node;
    yh_msg_id msg_id;
    void *p_param;
}yh_msg_node;

typedef struct
{
    yh_msg_node *p_head_node;
    yh_msg_node *p_tail_node;

    yonho_msg_process_cb msg_cb_arr[YH_MAX_MSG_TYPE];
}yh_msg_queue_ctx;


static void (*s_p_sys_lock)(void) = yh_null;
static void (*s_p_sys_unlock)(void) = yh_null;
static yh_msg_queue_ctx s_msg_queue_ctx = {0};

int yonho_init( void (*sys_lock)(void), void (*sys_unlock)(void) )
{
    if((yh_null == sys_lock)||(yh_null == sys_unlock))
    {
        yh_error("input func is null. yonho init fail");
        return -1;
    }
    s_p_sys_lock = sys_lock;
    s_p_sys_unlock = sys_unlock;
    yh_memset(&s_msg_queue_ctx, 0, sizeof(yh_msg_queue_ctx));

    yh_debug("yonho init success. Max msg type:%u", YH_MAX_MSG_TYPE);
    return 0;
}

int yonho_register_msg_process(yh_msg_id *p_msg_id, yonho_msg_process_cb msg_cb)
{
    int tmp_idx = 0;
    if( (yh_null == p_msg_id) || (yh_null == msg_cb) )
        return -1;

    s_p_sys_lock();
    for(tmp_idx = 0; tmp_idx < YH_MAX_MSG_TYPE; tmp_idx++)
    {
        if(yh_null == s_msg_queue_ctx.msg_cb_arr[tmp_idx])
        {
            s_msg_queue_ctx.msg_cb_arr[tmp_idx] = msg_cb;
            *p_msg_id = tmp_idx;
            yh_debug("register msg_cb success. msg_id:%d", tmp_idx);
            s_p_sys_unlock();
            return 0;
        }
    }
    *p_msg_id = -1;
    yh_debug("register msg_cb fail. reach max msg type:%d", YH_MAX_MSG_TYPE);
    s_p_sys_unlock();
    return -1;
}

int yonho_post_msg(yh_msg_id msg_id, void *p_param)
{
    yh_msg_node *p_msg_node = yh_null;

    s_p_sys_lock();
    if((msg_id < 0)||(msg_id >= YH_MAX_MSG_TYPE)||(yh_null == s_msg_queue_ctx.msg_cb_arr[msg_id]))
    {
        yh_error("msg_id:%d is invalid.", msg_id);
        s_p_sys_unlock();
        return -2;
    }

    p_msg_node = yh_calloc(1, sizeof(yh_msg_node));
    if(yh_null == p_msg_node)
    {
        yh_error("yonhe calloc yh_msg_node fail. msg_id:%d size:%u", msg_id, sizeof(yh_msg_node) );
        s_p_sys_unlock();
        return -3;
    }
    p_msg_node->msg_id = msg_id;
    p_msg_node->p_param = p_param;

    if(yh_null == s_msg_queue_ctx.p_head_node)
    {
        s_msg_queue_ctx.p_head_node = p_msg_node;
        s_msg_queue_ctx.p_tail_node = p_msg_node;
    }else
    {
        s_msg_queue_ctx.p_head_node->p_next_node = p_msg_node;
        s_msg_queue_ctx.p_head_node = p_msg_node;
    }
    yh_debug("post msg_id %d success.", msg_id);

    s_p_sys_unlock();
    return 0;
}

int yonho_get_msg_node(yh_msg_ctx *p_msg_node)
{
    yh_msg_node *p_node = yh_null;
    if(yh_null == p_msg_node)
        return -1;

    s_p_sys_lock();

    if(yh_null == s_msg_queue_ctx.p_tail_node)
    {
        yh_debug("curr msg queue is empty.");
        yh_memset(p_msg_node, 0, sizeof(yh_msg_ctx));
        s_p_sys_unlock();
        return -2;
    }
    /* 获取tail节点 */
    p_msg_node->msg_cb = s_msg_queue_ctx.msg_cb_arr[s_msg_queue_ctx.p_tail_node->msg_id];
    p_msg_node->msg_id = s_msg_queue_ctx.p_tail_node->msg_id;
    p_msg_node->p_param = s_msg_queue_ctx.p_tail_node->p_param;
    yh_debug("get msg node success. msg_id:%d", p_msg_node->msg_id);
    /* 更新并删除tail节点 */
    p_node = s_msg_queue_ctx.p_tail_node;
    s_msg_queue_ctx.p_tail_node = s_msg_queue_ctx.p_tail_node->p_next_node;
    yh_free(p_node);

    s_p_sys_unlock();

    return 0;
}
