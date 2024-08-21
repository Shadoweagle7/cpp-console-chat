// cpp-console-chat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#error TODO: REWORK TO NOT NEED THIS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <thread>
#include <stop_token>
#include <mutex>
#include <queue>
#include <concepts>
#include <cstddef>
#include <tchar.h>
#include <format>
#include <stdexcept>
#include <memory>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#ifdef UNICODE
std::wostream &tcout = std::wcout;

using tstring = std::wstring;
using tstring_view = std::wstring_view;
#else
std::ostream &tcout = std::cout;
using tstring = std::string;
using tstring_view = std::string_view;
#endif // UNICODE

class win32_exception : public std::runtime_error {
private:
	static std::string to_std_string(tstring_view tstr) {
		const size_t tstr_size = tstr.size() * sizeof(wchar_t);
		std::unique_ptr<char[]> str = std::make_unique<char[]>(tstr_size);
		char *str_begin = str.get();

		std::sprintf(str_begin, "%ls", tstr.data());

		return std::string(str_begin, str_begin + tstr_size);
	}

	DWORD lastErrorCode;
public:
	win32_exception(DWORD lastErrorCode, tstring_view lastErrorMessage) :
		std::runtime_error(to_std_string(lastErrorMessage)),
		lastErrorCode(lastErrorCode)
	{}

	win32_exception(DWORD lastErrorCode, std::string_view lastErrorMessage) :
		std::runtime_error(std::string(lastErrorMessage)),
		lastErrorCode(lastErrorCode)
	{}

	DWORD GetLastError() const {
		return lastErrorCode;
	}
};

class message_t {
private:
	std::string str_message;

	void sanitize(std::string_view str) {
		// TODO: Stub
	}
public:
	using byte_t = char;

	message_t(std::string_view message) {
		sanitize(message);

		str_message = message;
	}

	std::vector<byte_t> to_bytes() const {
		return std::vector<byte_t>(str_message.begin(), str_message.end());
	}
};

std::queue<message_t> message_queue;
std::mutex message_queue_mutex;
bool sending_messages = false;
bool receiving_messages = false;

void receive_message() {
	// TODO: Win32 API Call Here

	

	std::unique_lock<std::mutex> lock(message_queue_mutex);

	message_queue.push(TODO: MESSAGE FROM Win32 API CALL GOES HERE);
}

void send_message(const message_t &message) {
	// TODO: Win32 API Call Here
}

class WSA {
private:
	static constexpr WORD wsaDefaultVersion = MAKEWORD(2, 2);
	static WSA instance;

	WSADATA data;
	SOCKET socket;
	bool cleaned_up = false;

	addrinfo *result = nullptr, *ptr = nullptr, hints{};
	bool addrinfo_result_allocated = false;

	WSA(WORD versionRequested) {
		int lastError = WSAStartup(versionRequested, &data);

		socket = INVALID_SOCKET;

		if (lastError != 0) {
			TCHAR msg[256]{};

			msg[0] = _T('\0');

			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				lastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				msg,
				sizeof(msg),
				NULL
			);

			throw win32_exception(lastError, std::format(_T("{0}"), msg));
		}

		if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
			WSACleanup();
			cleaned_up = true;

			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			throw std::invalid_argument("Could not find a usable version of Winsock.dll");
		} else {
			OutputDebugString(_T("Successfully found Winsock 2.2 dll"));
		}

		ZeroMemory(&hints, sizeof(addrinfo));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		lastError = getaddrinfo(, , &hints, &result);
		addrinfo_result_allocated = true;

		if (lastError != 0) {
			WSACleanup();
			cleaned_up = true;

			TCHAR msg[256]{};

			msg[0] = _T('\0');

			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				lastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				msg,
				sizeof(msg),
				NULL
			);

			throw win32_exception(lastError, std::format(_T("{0}"), msg));
		}
	}
public:
	static WSA get_or_create(WORD versionRequested) {
		return WSA(versionRequested);
	}

	void connect() {
		int iResult;

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			socket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (socket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				cleaned_up = true;

				TCHAR msg[256]{};

				msg[0] = _T('\0');

				int wsaLastError = WSAGetLastError();

				FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					wsaLastError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					msg,
					sizeof(msg),
					NULL
				);

				throw win32_exception(wsaLastError, std::format(_T("{0}"), msg));
			}

			// Connect to server.
			iResult = ::connect(socket, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR) {
				closesocket(socket);
				socket = INVALID_SOCKET;

				continue;
			}

			break;
		}

		if (addrinfo_result_allocated) {
			freeaddrinfo(result);
		}
	}



	~WSA() {
		if (!cleaned_up) {
			WSACleanup();
		}
	}
};

WSA WSA::instance(WSA::wsaDefaultVersion);

WSA get_or_create_wsa(WORD versionRequested) {
	return WSA::get_or_create(versionRequested);
}

int main() {
	constexpr WORD wsaVersionRequested = MAKEWORD(2, 2);

	WSA wsa = get_or_create_wsa(wsaVersionRequested);

	std::jthread message_receiving_thread([](std::stop_token st) {
		if (st.stop_requested()) {
			return;
		}

		while (receiving_messages) {

		}
	});

	std::jthread message_sending_thread([](std::stop_token st) {
		if (st.stop_requested()) {
			return;
		}

		while (sending_messages) {
			if (!message_queue.empty()) {
				std::unique_lock<std::mutex> lock(message_queue_mutex);

				if (!message_queue.empty()) {
					for (; !message_queue.empty(); message_queue.pop()) {
						send_message(message_queue.front());
					}
				}
			}
		}
	});

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
