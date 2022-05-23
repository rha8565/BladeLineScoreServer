#include "BladeLineScoreServer.h"

/* 실행부 */
int main()
{
	sockErr = CreateSocket();
	SockErrProcessing(sockErr);
	SockListener();
	return 0;
}

/* DB로부터 랭킹정보를 가져와 구조체 배열에 저장합니다. */
void DatabaseRead() {
	ifstream inFile("Database.txt"); //파일 Read 스트림
	string fBuffer; // 파일 읽기 버퍼
	string intBuffer; //정수변환 버퍼
	rankerInfo rankTemp;
	rankerInfoArr.clear();
	for (int i = 0; i < 30; i++) {
		if (inFile.eof())
			break;
		getline(inFile, fBuffer);
		stringstream sBuffer(fBuffer);
		sBuffer >> rankTemp.userName;
		sBuffer >> rankTemp.userScore;
		rankerInfoArr.push_back(rankTemp);
		inFile.peek();
	}
	inFile.close();
}

/* DB에 정보를 저장합니다. */
void DatabaseWrite() {
	ofstream outFile("Database.txt");
	string strTemp;
	for (int i = 0; i < 30; i++) {
		strTemp = rankerInfoArr[i].userName + " " + rankerInfoArr[i].userScore + " \n";
		outFile.write(strTemp.c_str(), strTemp.size());
	}
	outFile.close();
}

/* 랭킹 서버의 소켓을 만듭니다. */
int CreateSocket() {

	/* 소켓 초기화 */
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		return 1;

	/* 소켓 생성 유효성 검사 */
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET)
		return 2;

	/* 소켓 설정 */
	addrServerSock.sin_family = AF_INET;
	addrServerSock.sin_port = htons(25000); // 포트
	addrServerSock.sin_addr.S_un.S_addr = inet_addr("172.31.34.90");   //172.31.22.147 아마존 서버 아이피

	/* 소켓 옵션 */
	setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&bValid, sizeof(bValid));
	if (::bind(serverSock, (struct sockaddr *)&addrServerSock, sizeof(addrServerSock)) != 0)
		return 3;

	/* 소켓 연결 대기열 생성 */
	if (listen(serverSock, 5) != 0)
		return 4;

	return 0;
}

/* 소켓 에러 처리 */
void SockErrProcessing(int sockErr) {
	switch (sockErr)
	{
	case Socket_Initialization_Error:
		cout << "Socket initialization error!!!" << endl;
		exit(0);
	case Socket_Valid_Error:
		cout << "Socket valid error !!!" << endl;
		exit(0);
	case Socket_Bind_Error:
		cout << "Socket bind error!!!" << endl;
		exit(0);
	case Socket_listen_fail_Error:
		cout << "Socket listen fail error!!!" << endl;
		exit(0);
	}
}

/* 뷰단에 랭킹 정보를 그림 현재 랭킹 구조체 배열에 있는 정보를 참조 */
void DrawView() {
	system("cls"); // 뷰단 초기화
	cout << "*AIXLAB BladeLine Score Server*\n";
	for (int i = 0; i < 30; i++) {

		cout << "R. " << i+1 << " " 
			<< rankerInfoArr[i].userName << " "
			<< rankerInfoArr[i].userScore << " " << endl;

	}
}

//소켓 접속을 대기하고 소켓이 접속했다면 데이터 터널 쓰레드를 만듭니다.
void SockListener() {
	clientSockLen = sizeof(addrClientSock);
	DatabaseRead();
	DrawView();
	while (1) {
		/* 소켓 접속을 대기합니다 */
		clientSock = accept(serverSock, (struct sockaddr *)&addrClientSock, &clientSockLen);
		
		/* 클라이언트 소켓 유효성 검사*/
		if (clientSock == INVALID_SOCKET) {
			cout<< "Invalid client socket!!!" <<endl;
			exit(0);
		}
		CreateThread(NULL, 0, DataTunnelThread, (void*)clientSock, 0, NULL);
	}
}

DWORD WINAPI DataTunnelThread(void *data) {

	SOCKET socketTemp = (SOCKET)data;
	char    recvBuf[2048];
	char    sendBuf[2048];
	string mainCommandBuf_01;
	string playerName;
	string playerScore;
	memset(recvBuf, 0, sizeof(recvBuf));
	while (recv(socketTemp, recvBuf, sizeof(recvBuf), 0) != -1) {
		char* commandTokTemp = strtok(recvBuf, " ");
		mainCommandBuf_01 = commandTokTemp;
		if (rankCheckCommand._Equal(mainCommandBuf_01)) {
			commandTokTemp = strtok(NULL, " ");
			playerScore = commandTokTemp;			
			if (atoi(playerScore.c_str()) > atoi(rankerInfoArr.back().userScore.c_str())) {
				memset(sendBuf, 0, sizeof(sendBuf));
				strcpy(sendBuf, "Ranker");
				send(socketTemp, sendBuf, sizeof(sendBuf), 0);
				break;
			}
			else {
				memset(sendBuf, 0, sizeof(sendBuf));
				strcpy(sendBuf, "NotRanker");
				send(socketTemp, sendBuf, sizeof(sendBuf), 0);
				break;
			}
		}
		else if (requestRankingCommand._Equal(mainCommandBuf_01)) {
			string rankerInfoTemp;
			for (int i = 0; i < 30; i++) {
				rankerInfoTemp = rankerInfoTemp + rankerInfoArr[i].userName + " " + rankerInfoArr[i].userScore + " ";
			}
			memset(sendBuf, 0, sizeof(sendBuf));
			strcpy(sendBuf, rankerInfoTemp.c_str());
			send(socketTemp, sendBuf, sizeof(sendBuf), 0);
			break;
		}
		else if (newRankerCommand._Equal(mainCommandBuf_01)) {
			commandTokTemp = strtok(NULL, " ");
			playerName = commandTokTemp;
			commandTokTemp = strtok(NULL, " ");
			playerScore = commandTokTemp;
			UpdateRankinfo(playerName, atoi(playerScore.c_str()));
		}
	}
	closesocket(socketTemp);
	return 0;

}

void SendRankingData(SOCKET Sock) {
	//send(Sock, commandBuf, sizeof(commandBuf), 0);
		for (int i = 0; i < 29; i++) {
		Sleep(20);
		send(Sock, rankerInfoArr[i].userName.c_str(), sizeof(rankerInfoArr[i].userName.c_str()), 0);

		Sleep(20);
		send(Sock, rankerInfoArr[i].userScore.c_str(), sizeof(rankerInfoArr[i].userScore.c_str()), 0);

		}
		Sleep(20);
		send(Sock, "EndOfData", sizeof("EndOfData"), 0);
		closesocket(Sock);
}


/* 초벌로 값을 받아오는 기능까지만 구현 추가 기능 구현 필요
   1. 받아온 값에 랭크를 책정
   2. 랭킹 정보를 클라이언트한테 전달하고 소켓 종료
   값 비교할때 UserScore를 atoi()하여 정수화
*/
void UpdateRankingData(string name, string score) {
}

void CheckRanking() {
}

void UpdateRankinfo(string name, int score) {
	rankerInfo rankerTemp;
	rankerTemp.userName = name;
	rankerTemp.userScore = to_string(score);
	for (int i = 0; i < 30; i++) {
		if (score > atoi(rankerInfoArr[i].userScore.c_str())) {
			rankerInfoArr.insert(rankerInfoArr.begin()+i, rankerTemp);
			rankerInfoArr.pop_back();
			break;
		}
	}
	DatabaseWrite();
	DrawView();
	
}