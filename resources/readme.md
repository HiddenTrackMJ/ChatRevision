### ChatService is a project for learning c++.


#### ��Ҫ����
* �ͻ������ӷ�����
* ��������Ȩ
* �ͻ���֮���ı�ͨ��


#### ��Ҫģ��
* UserAgent  
  * �������û���ҵ���߼�
  * �����û�����
  * �����û������
  * �����û�״̬
* ChatClient
  * ����SocketClient
  * ��ChatServerͨ��
  * ���е�½
  * ��Ϣ�����뷢��
  * ������Ϣ����룬�ϱ���UserAgent
* SocketClient
  * ͨ��socket API��SocketServerͨ��
  * ����TCP��Ϣ
  * ����TCP��Ϣ���ϱ���ChatClient
* ChatServer
  * ����SocketServer��������ChatClient����Ϣͨ��  
  * �����û���Ȩ
  * Ϊ�û���������
  * ת���û�����Ϣ
* SocketServer
  * ͨ��socket API��SocketClient����ͨ��
  * ά�����SocketClient
  * ����select����̻߳��ƴ�����Ϣ


#### 