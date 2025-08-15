#启动监控模块------调度模块启动10s检查一次
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 10 /home/ubuntu/bilibili/BigProjrct/tools/bin/process_monitor /home/ubuntu/bilibili/BigProjrct/tools/logs/process_monitor.log

#启动生成观测数据程序----调度模块启动60s生成一次
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 60 /home/ubuntu/bilibili/BigProjrct/idc/bin/crtsurfdata /home/ubuntu/bilibili/BigProjrct/idc/ini/stcode.ini /home/ubuntu/bilibili/BigProjrct/surfdatas /home/ubuntu/bilibili/BigProjrct/logs/log.txt

#文件自动清理程序------调度模块300秒启动一次
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 300 /home/ubuntu/bilibili/BigProjrct/tools/bin/file_cleaner /home/ubuntu/bilibili/BigProjrct/surfdatas *.csv,*.json,*.xml 0

#文件自动压缩程序------调度程序300秒启动一次
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 300 /home/ubuntu/bilibili/BigProjrct/tools/bin/file_compressor /home/ubuntu/bilibili/BigProjrct/logs *.txt 0

# tcp文件传输的服务端程序-----调度程序10秒启动一次
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 10 /home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_server /home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_server.log 8080

# 把目录TcpDatas/Client_PutDatas中的文件上传到TcpDatas/ServerDatas目录中。
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 20 /home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_putfiles_client /home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_putfiles_clinet.log "<ip>127.0.0.1</ip><port>8080</port><clientpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_PutDatas</clientpath><clientpathback>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_PutBackDatas</clientpathback><type>1</type><includeSubdirFiles>false</includeSubdirFiles><matchname>*.csv,*.json,*.xml</matchname><serverpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/ServerDatas</serverpath><timeInterval>30</timeInterval><timeout>40</timeout><processname>tcp_putfiles_client</processname>"

# 把目录TcpDatas/ServerDatas中的文件下载到TcpDatas/Client_GetDatas目录中。
/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 20 /home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_getfiles_client /home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_getfiles_clinet.log "<ip>127.0.0.1</ip><port>8080</port><clientpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_GetDatas</clientpath><serverpathback>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Server_BackDatas</serverpathback><type>1</type><includeSubdirFiles>false</includeSubdirFiles><matchname>*.csv,*.json,*.xml</matchname><serverpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/ServerDatas</serverpath><timeInterval>30</timeInterval><timeout>40</timeout><processname>tcp_getfiles_client</processname>"



