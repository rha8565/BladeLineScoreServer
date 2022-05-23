#pragma comment(lib, "ws2_32") 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <WinSock2.h>
#include <thread>  
#include <mutex>


/* 상수 선언 */
#define Socket_Initialization_Error 1
#define Socket_Valid_Error 2
#define Socket_Bind_Error 3
#define Socket_listen_fail_Error 4


/* 네임스페이스 선언 */
using namespace std;

/* 구조체와 구조체 배열 선언입니다. */
typedef struct rankerInfo {
	string userName;
	string userScore;
} rankerInfo;

vector<rankerInfo> rankerInfoArr;

/* 소켓관련 변수 선언부 */
WSADATA wsaData;
SOCKET serverSock;
SOCKET clientSock;
struct sockaddr_in addrServerSock;
struct sockaddr_in addrClientSock;
BOOL bValid = 1;
int sockErr;
int clientSockLen;

/* 일반 변수 선언부 */
char commandBuf[2048]; //클라이언트 명령어 버퍼
string requestRankingCommand = "RequestRanking";
string rankCheckCommand = "RankCheck";
string newRankerCommand = "NewRanker";

/* 함수 선언부 */
void DatabaseRead(); // DB를 읽어오는 기능
void DatabaseWrite(); // DB에 쓰기
int CreateSocket(); // 소켓 설정을 위한 함수
void SockErrProcessing(int sockErr); // 소켓 에러 처리 함수
void DrawView(); // 뷰단에 랭킹 정보를 뿌림
void SockListener(); // 소켓 리스너
void SendRankingData(SOCKET Sock); // 접속한 클라이언트에게 랭킹정보를 보넵니다.
void UpdateRankingData(string name, string score); // 접속한 클라이언트로 부터 새 랭커정보를 가져옵니다.
DWORD WINAPI DataTunnelThread(void *data);
void CheckRanking();
void UpdateRankinfo(string name, int score);