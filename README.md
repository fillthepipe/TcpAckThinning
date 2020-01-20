# TcpAckThinning

An attempt to understand impact of ACK thinning on TCP using Linux Kernel 5.3.0.

Following are the primary changes:
1. Kernel changes: A new sockops called `BPF_SOCK_OPS_ACK_THRESH_INIT` is added which can be controlled using `BPF_CGROUP_SOCK_OPS`.
2. BPF bytecode: Sample bpf code (`samples/bpf/ack_thinning_*.c`) to use the above BPF option to change the threshold.
3. Tools: Sample tools for monitoring.


## Setup Kernel

### Step 1: Install the updated kernel.

```
make
make install
make modules_install
make headers_install
```
Reboot and boot into the new kernel

### Step 2: Compile ack thinning bpf kernel bytecode
```
# Open the ack_thinning_kern.c and modify the threshold that you want to set for sending ACK
cd samples/bpf
make
# ack_thinning_kern.o should be created.
```

### Step 3: Load the bpf

Follow [tcp_bpf.readme](https://github.com/torvalds/linux/blob/master/samples/bpf/tcp_bpf.readme)

## Using Tools

### bpftrace
We use bpftrace to track the `tcp_send_ack()` and `tcp_recv_data_event()`
calls. We also verify the data/acks counts from the packet capture and ensure
that they match. Further we use the bpftrace to track from which higher level
apis the send-ack call was initiated. Check
[tools/ack_check.bt](tools/ack_check.bt) has the details.
