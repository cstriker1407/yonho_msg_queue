#include <stdio.h>
#include "yonho_msg_queue.h"

void sys_lock(void)
{


}
void sys_unlock(void)
{


}

void msg_process_cb(yh_msg_id msg_id, void *p_param)
{
    printf("msg process is called. msg_id:%d param:%d\n", msg_id, (int)p_param);
}



int main(void)
{
    printf("Hello World!\n");
    yonho_init(sys_lock, sys_unlock);

    yh_msg_id msg_id[25] = {0};
    int index = 0;
    for(index = 0; index < 25; index++)
    {
        yonho_register_msg_process( msg_id + index, msg_process_cb);
    }

    for(index = -3; index < 10; index++)
    {
        yonho_post_msg(index, (void *)index);
        yonho_post_msg(index, (void *)(index + 100));
    }

    for(index = 0; index < 50; index++)
    {
        yh_msg_ctx msg_ctx = {0};
        int result = yonho_get_msg_node(&msg_ctx);
        if(0 == result)
        {
            printf("get msg node. msg_id:%d param:%d\n", msg_ctx.msg_id, (int)(msg_ctx.p_param));
        }
    }


    return 0;
}
