#pragma comment(lib, "ws2_32") 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <WinSock2.h>
#include <thread>  
#include <mutex>


/* ��� ���� */
#define Socket_Initialization_Error 1
#define Socket_Valid_Error 2
#define Socket_Bind_Error 3
#define Socket_listen_fail_Error 4


/* ���ӽ����̽� ���� */
using namespace std;

/* ����ü�� ����ü �迭 �����Դϴ�. */
typedef struct rankerInfo {
	string userName;
	string userScore;
} rankerInfo;

vector<rankerInfo> rankerInfoArr;

/* ���ϰ��� ���� ����� */
WSADATA wsaData;
SOCKET serverSock;
SOCKET clientSock;
struct sockaddr_in addrServerSock;
struct sockaddr_in addrClientSock;
BOOL bValid = 1;
int sockErr;
int clientSockLen;

/* �Ϲ� ���� ����� */
char commandBuf[2048]; //Ŭ���̾�Ʈ ��ɾ� ����
string requestRankingCommand = "RequestRanking";
string rankCheckCommand = "RankCheck";
string newRankerCommand = "NewRanker";

/* �Լ� ����� */
void DatabaseRead(); // DB�� �о���� ���
void DatabaseWrite(); // DB�� ����
int CreateSocket(); // ���� ������ ���� �Լ�
void SockErrProcessing(int sockErr); // ���� ���� ó�� �Լ�
void DrawView(); // ��ܿ� ��ŷ ������ �Ѹ�
void SockListener(); // ���� ������
void SendRankingData(SOCKET Sock); // ������ Ŭ���̾�Ʈ���� ��ŷ������ ���ܴϴ�.
void UpdateRankingData(string name, string score); // ������ Ŭ���̾�Ʈ�� ���� �� ��Ŀ������ �����ɴϴ�.
DWORD WINAPI DataTunnelThread(void *data);
void CheckRanking();
void UpdateRankinfo(string name, int score);