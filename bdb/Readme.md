
app_run_log

1). 记录内容：void *argu => UartPort,void args => VirtualSocket,关键内容

2). 存储时效: 1 个月覆盖原数据

3). 命名办法: path/name_month+date.type ->eg: log/sys_log_1002.txt

4). 规则说明: 自动产生路径及文件名,内容清除.


app_run_data_log

1). 记录内容：void *argu => UartPort,void args => VirtualSocket,关键内容

2). 存储时效: 12 个月覆盖原运行数据

3). 命名办法: path/name_month.type ->eg: data/app_data_10.txt

4). 规则说明: 自动产生路径及文件名,内容清除.


app_run_conf_

1). 记录内容：void *argr =>	RunConf 

2). 存储时效: run config parament dynamic set

3). 命名办法: path/name_month.type ->eg: conf/config.txt, frequecy_channel.txt

4). 规则说明: 自动产生路径及文件名,初始参数.

sys_run_evn

1).	open-ssh-server

2). 
