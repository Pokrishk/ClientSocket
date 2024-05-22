#define WIN32_LEAN_AND_MEAN //макрос

#include <iostream> //нужные библиотеки
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	WSADATA wsaData; // служебная структура для хранение информации
	ADDRINFO hints; //структура для указания критериев для функции getaddrinfo.
	ADDRINFO* adrrResult; //указатель на структуру с результатами вызова getaddrinfo.
	SOCKET ClientsSocket = INVALID_SOCKET; //сокет
	const char* sendBuffer = "Hello from Client"; //буфер для отправки
	const char* send2Buffer = "Hello from Client2";
	char recvBuffer[512]; //буфер для получения

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //старт использования библиотеки сокетов процессом определния версии и структуры
	if (result != 0) { //обработка ошибки
		cout << "WSAStartu failed error"<< endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints)); //зануление памяти, 1 - ый паметр, что зануляем, 2 - ой сколько
	hints.ai_family = AF_INET;//4-байтныйEthernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// используем протокол TCP

	result = getaddrinfo("localhost", "3030", &hints,&adrrResult);//функциия хранит в себе адрес, порт, семейство структур, адрес сокета
	if (result != 0) { //обработка ошибки с чисткой данных
		cout << "getaddrinfo failed" << endl;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	ClientsSocket = socket(adrrResult->ai_family, adrrResult->ai_socktype, adrrResult->ai_protocol); //создание сокета с указателями на его параметры
	if (ClientsSocket == INVALID_SOCKET) { //обработка ошибки с чисткой данных
		cout << "Socket failed" << endl;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	result = connect(ClientsSocket, adrrResult->ai_addr, (int)adrrResult->ai_addrlen); //подключение к серверу. (сокет, адресс, порт)
	if (result == SOCKET_ERROR) { //обработка ошибки с чисткой данных
		cout << "Unable connect to server" << endl;
		closesocket(ClientsSocket);
		ClientsSocket = INVALID_SOCKET;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}
	result = send(ClientsSocket, sendBuffer, strlen(sendBuffer), 0); //отправка данных серверу (сокет, что нужно отправить, размер отправимого, флаги (0-знач. по умолчанию))
	if (result == SOCKET_ERROR) { //обработка ошибки с чисткой данных
		cout << "send failed errror" << endl;
		closesocket(ClientsSocket);
		ClientsSocket = INVALID_SOCKET;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << "bytes" << endl; //вывод в консоль о том, сколько байтов отправлено

	result = send(ClientsSocket, send2Buffer, strlen(send2Buffer), 0);//отправка данных серверу
	if (result == SOCKET_ERROR) { //обработка ошибки с чисткой данных
		cout << "send failed errror" << endl;
		closesocket(ClientsSocket);
		ClientsSocket = INVALID_SOCKET;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << "bytes" << endl; //вывод в консоль о том, сколько байтов отправлено

	result = shutdown(ClientsSocket, SD_SEND); //остановка передачи данных на сокете (сокет, параметр, определяющий, какие операции нужно прекратить)
	if (result == SOCKET_ERROR) { //обработка ошибки с чисткой данных
		cout << "shut" << endl;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	do { //цикл, работающий пока получаются данные
		ZeroMemory(recvBuffer, 512);//обнуление буфера для получения данных.
		result = recv(ClientsSocket, recvBuffer, 512, 0);//получение данных от сервера. (сокет, буфер для получения, размер, флаги)
		if (result > 0) {
			cout << "Recived" << result << "bytes" << endl; //вывод полученного сообщения и его размера в консоль
			cout << "Received data " << recvBuffer << endl;
		}
		else if (result == 0) {
			cout << "Connection closed" << endl; //сообщение о закрытии соединения
		}
		else
		{
			cout << "recv failed with error" << endl; //сообщение об ошибке
		}
	} while (result > 0); 
	freeaddrinfo(adrrResult); //чистка данных
	WSACleanup();
	return 0;
}