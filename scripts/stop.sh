#先停止调度进程-----所有的服务都是调度进程调度的
killall -9 process_control
#停止普通进程(普通服务)-----1.正常退出  2.停5秒  3.kill-9
killall  crtsurfdata  file_cleaner file_compressor
killall  tcp_server   tcp_getfiles_client  tcp_putfiles_client

sleep 5

killall -9 crtsurfdata  file_cleaner file_compressor
killall -9 tcp_server   tcp_getfiles_client  tcp_putfiles_client
