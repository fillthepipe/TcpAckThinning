
1、运行时，把sender文件夹拷贝到发送端主机（ubuntu）
cd sender
chmod +x FillPDemo
同时，把receiver文件夹拷贝到接收端主机（ubuntu）
cd receiver
chmod +x FillPDemo

2、添加运行环境
export LD_LIBRARY_PATH=./

3、传数据流：

  发送端  ./FillPDemo -c server_ip -p server_port -r testcase001

  接收端  ./FillPDemo -s server_ip -p server_port -r testcase001
	
4、传文件：-k指定分片大小KB，默认为512KB，-j 指定发送或接收文件名，-b 指定最大传输速率（对TCP无效，只对FillP有效）

  发送端  ./FillPDemo -c server_ip -p server_port -r testcase005 -j filename -k 512 -b 1000000
  接收端  ./FillPDemo -s server_ip -p server_port -r testcase005 -j filename -k 512 -b 1000000

  注意，传文件是testcase005,传流是testcase001,如果不需要分片，则推荐使用testcase004。

5、FillPDemo实现了类似iperf的功能，除了默认使用FillP进行传输以外，还可以通过“-T”，实现TCP传输。例如，使用TCP传流的命令为：

    ./FillPDemo -s server_ip -p server_port -r testcase001 -T
    ./FillPDemo -c server_ip -p server_port -r testcase001 -T
	
	
技术支持：李彤\427917 罗素部协议实验室


附FillPDemo的部分选项说明：

全局参数：
   -c 以客户端模式运行，后面跟服务器IP
   -s 以服务端模式运行，后面跟服务器绑定IP
   -p 指定服务端端口，当角色为client时，表示远端服务器端口；当角色为server时，表示绑定的本端端口
   -t 设置测试时长，默认值为10，单位s
   -r 指定testcase名称，默认值为testcase001
   -w 表示FillP debug日志等级，默认值为3，表示INFO等级。其他值：DEBUG = 1,DETAIL,INFO,WARNING,ERROR,BUTT,EN;
   -b 设置速率，默认值为100000，单位kbps
   -f 设置日志文件名称，默认值为 fillplog.txt
   -x 设置发送和接收缓存，默认值为81920，单位byte
   -y 选择算法，默认值为2，可选为1,2,3
   -d 设置数据包大小，默认值1300，单位byte

   -U FillP开启满CPU运行
   -T 使用TCP进行测试
   -H 查看帮助

局部参数：

testcase001 : 内存流测试，不间断始终运行；支持指定数据流方向。
        -g : 指示数据流方向，后面需要参数；1表示 server单收；2 表示server单发； 3表示双收发；默认为1，即server接收，client端发送；
        -Q : TCP开启TCP_QUICKACK，无参数，默认关闭
        -C : 在发送端设置TCP拥塞控制算法，不加-C使用系统默认算法。-C必须使用系统已经支持的算法,如:cubic,bbr,reno,ledbat等
    示例 ：服务端绑定192.168.20.12，使用端口 12345
FillP 测试:
        ./FillPDemo -s 192.168.20.12 -p 12345 -g 3 -r testcase001 #服务端
        ./FillPDemo -c 192.168.20.12 -p 12345 -g 3 -r testcase001 #客户端
TCP 测试:
   ./FillPDemo -s 192.168.20.12 -p 12345 -g 3 -r testcase001 -T -Q -C cubic #服务端
   ./FillPDemo -c 192.168.20.12 -p 12345 -g 3 -r testcase001 -T -Q -C cubic #客户端

testcase002 : 内存流测试，支持间歇性应用受限，测试生存时间设置
        -g : 指示数据流方向，后面需要参数；1表示 server单收；2 表示server单发； 3表示双收发；默认为1，即server接收，client端发送；
        -e : 指示连接建立成功后等待多长时间后再进行数据传输，后面需要参数，表示等待时间(秒)。默认为5秒。
        -t : 指示测试生存时间，后面需要参数，表示多长时间（秒）后测试退出。默认为30秒
        -n : 指示应用以速率不受限的方式传输多长时间后进入到应用受限修整，后面需要参数，表示持续发送的时间（秒）。默认为5秒。
        -o : 指示应用受限修整时间，后面需要参数，表示应用速率受限下的发送时间（秒）。默认为1秒。
        -q : 指示应用受限期间的发送速率，后面需要参数，表示应用受限下的发送速率（kbps)。默认为0（即不发送任何数据）。
    示例 ：服务端绑定192.168.20.12，使用端口 12345
        ./FillPDemo -s 192.168.20.12 -p 12345  -g 3 -r testcase002 -e 5 -t 30 -n 10 -o 3 -q 1000 #服务端
        ./FillPDemo -c 192.168.20.12 -p 12345  -g 3 -r testcase002 -e 5 -t 30 -n 10 -o 3 -q 1000 #客户端
        说明：服务端和客户端双端做发送与接收，测试时间为30秒（-t 30)。连接建立以后休眠5秒(-e 5)不发送任何数据，后面以“全速发送10秒(-n 10)后，以1000kbps(-q 1000)发送3秒(-o 3)”的方式周期性的发送。


testcase003 : 内存流测试，不间断始终运行；支持测试多条流并发。
        -m : 表示使用TCP协议或FillP协议，后面需要参数；0 表示 FillP； 1 表示 TCP；默认为FillP
        -g : 指示数据流方向，后面需要参数；1表示 server单收；2 表示server单发； 3表示双收发；默认为1，即server接收，client端发送；
        -a : 指示client需要多少条并发流，后面需要参数，表示流的个数；流的个数不能大于1000。会打印每条流的速率，以及总速率。默认为1条。
    示例 ：服务端绑定192.168.20.12，使用端口 12345
        ./FillPDemo -s 192.168.20.12 -p 12345 -g 3 -r testcase003 -a 100 #服务端
        ./FillPDemo -c 192.168.10.11 -p 12345 -g 3 -r testcase003 -a 100 #客户端运行100条流

testcase004 : 文件传输测试。
        -g : 指示数据流方向，后面需要参数；1表示 server单收；2 表示server单发； 3表示双收发；默认为1，即server接收，client端发送；
        -j : 指示要传输文件名称。发送端表示待传输文件名，接收端表示接收文件后文件新存储名称
     示例 ：服务端绑定192.168.20.12，使用端口 12345
        ./FillPDemo -s 192.168.20.12 -p 12345 -r testcase004 -j recv.file #服务端接收到文件后以recv.file的名称存储
        ./FillPDemo -c 192.168.20.12 -p 12345 -r testcase004 -j send.file  #发送端发送send.file文件

testcase005 : 大文件单流分片传输测试。
        -g : 指示数据流方向，后面需要参数；1表示 server单收；2 表示server单发； 3表示双收发；默认为1，即server接收，client端发送；
        -j : 指示要传输文件名称。发送端表示待传输文件名，接收端表示接收文件后文件新存储名称。默认文件名为test.file
        -k : 文件的每个分片大小，后面需要参数，单位为Byte,默认为1MBytes
   示例 ：服务端绑定192.168.20.12，使用端口 12345
        ./FillPDemo -s 192.168.20.12 -p 12345 -r testcase005 -g 1 -j recv.file #服务端
        ./FillPDemo -c 192.168.20.12 -p 12345 -r testcase005 -g 1 -j send.file #客户端
