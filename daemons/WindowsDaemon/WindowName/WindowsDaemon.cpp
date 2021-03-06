#include "pch.h"
#include <iostream>
#include <windows.h>
#include <winuser.h>
#include <windef.h>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <libloaderapi.h>
#include <tchar.h>
#include <Psapi.h>
#include <ctime>

const int INACTIVE_SECONDS = 5 * 60;
const int EMIT_INTERVAL = 5;
const int MAX_NAME = 1000;
static POINT last_mouse_move;
static TCHAR window_name[MAX_NAME] = {0};
static TCHAR last_window_name[MAX_NAME] = { 0 };

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

bool mouseMoved() {
	POINT p;
	if (GetCursorPos(&p))
	{
		if (last_mouse_move.x != p.x || last_mouse_move.y != p.y) {
			last_mouse_move = p;
			return true;
		}

	}

	return false;
}

int checkKeyPress() {
	char keys[256];
	int count = 0;

	for (int i = 0; i < 256; i++) {
		keys[i] = (char)(GetAsyncKeyState(i) >> 8);
		if (keys[i]) {
			count++;
		}
	}

	return count;
}

bool getFocusedWindow()
{
	HWND handle = GetForegroundWindow();
	TCHAR buffer[MAX_NAME] = { 0 };
	DWORD dwProcId = 0;

	GetWindowThreadProcessId(handle, &dwProcId);

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	GetProcessImageFileName((HMODULE)hProc, buffer, MAX_NAME);
	CloseHandle(hProc);

	bool name_changed = false;
	for (int i = 0; i < MAX_NAME; i++) {
		if (i == 0 && buffer[i] == 0) {
			// Don't update me with a null window name
			break;
		}

		last_window_name[i] = window_name[i];

		if (buffer[i] != window_name[i]) {
			name_changed = true;
			window_name[i] = buffer[i];
		}
	}

	return name_changed;
}

void emit_event(std::time_t seconds, TCHAR window_name[]) {
	TCHAR print_path[MAX_PATH] = {0};
	TCHAR *print_start = print_path;

	// Some really bad string formatting that takes the last element of the path without the extension
	for (int i = MAX_PATH - 1; i >= 0; i--) {
		char curr_char = window_name[i];
		print_path[i] = curr_char;

		if (curr_char == '.') {
			print_path[i] = 0;
		}
		if (curr_char == '\\') {
			print_start += i + 1;
			break;
		}
	}

	std::wcout << MIN(seconds, 5) << ": " << print_start << std::endl;
}

int main()
{
	std::time_t last_action = std::time(0);
	std::time_t window_start = std::time(0);
	std::time_t last_event_emit = std::time(0);
	getFocusedWindow();

	bool inactive = false;

	while (1) {
		std::time_t curr_time = std::time(0);

		int num = checkKeyPress();
		if (num > 0) {
			last_action = curr_time;
		}
		if (mouseMoved()) {
			last_action = curr_time;
		}
		
		bool window_changed = getFocusedWindow();
		if (window_changed) {
			if (!inactive) {
				emit_event(curr_time - window_start, last_window_name);
			}
			window_start = curr_time;
			last_event_emit = curr_time;
		}

		if (curr_time - last_action > INACTIVE_SECONDS && !inactive) {
			emit_event(curr_time - window_start - INACTIVE_SECONDS, window_name);
			inactive = !inactive;
			last_event_emit = curr_time;
		}
		if (curr_time - last_action < INACTIVE_SECONDS && inactive) {
			inactive = !inactive;
			window_start = curr_time;
		}

		if (curr_time - last_event_emit >= EMIT_INTERVAL && !inactive) {
			emit_event(curr_time - window_start, window_name);
			last_event_emit = curr_time;
			window_start = curr_time;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}