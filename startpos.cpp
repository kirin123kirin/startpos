#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>

// プロセスIDからウィンドウハンドルを取得する関数
HWND GetWindowHandleFromProcessId(const DWORD TargetID) {
    HWND hWnd = GetTopWindow(NULL);
    do {
        if(GetWindowLong(hWnd, -8) != 0 || !IsWindowVisible(hWnd))  // -8 is GWL_HWNDPARENT
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hWnd, &ProcessID);
        if(TargetID == ProcessID)
            return hWnd;
    } while((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}

// 数値入力のバリデーション
bool IsValidInteger(const std::wstring& str, int& value) {
    std::wistringstream iss(str);
    iss >> value;
    // 入力が整数かつ正しい範囲であることを確認
    return !iss.fail() && iss.eof();
}

// エラーメッセージを表示する関数
void ShowError(const std::wstring& message) {
    std::wcerr << message << std::endl;                                   // コンソールへの出力
    MessageBoxW(NULL, message.c_str(), L"エラー", MB_OK | MB_ICONERROR);  // ポップアップウィンドウ
}

void showhelp() {
    std::wcerr << L"\n使い方: <コマンド [オプション...]> <x座標> <y座標> <幅> <高さ>" << std::endl;
}

int wmain(int argc, wchar_t* argv[]) {
    // 引数の確認
    if(argc < 6) {
        ShowError(L"必要な引数が不足しています。");
        showhelp();
        return 1;
    }

    // コマンドとオプションを結合
    std::wstring command;
    for(int i = 1; i < argc - 4; ++i) {
        if(i > 1)
            command += L" ";
        command += argv[i];
    }

    int x, y, width, height;

    // 各引数を検証
    if(!IsValidInteger(argv[argc - 4], x) || x < 0) {
        ShowError(L"x座標は0以上の整数で指定してください。");
        showhelp();
        return 1;
    }
    if(!IsValidInteger(argv[argc - 3], y) || y < 0) {
        ShowError(L"y座標は0以上の整数で指定してください。");
        showhelp();
        return 1;
    }
    if(!IsValidInteger(argv[argc - 2], width) || width <= 0) {
        ShowError(L"幅は正の整数で指定してください。");
        showhelp();
        return 1;
    }
    if(!IsValidInteger(argv[argc - 1], height) || height <= 0) {
        ShowError(L"高さは正の整数で指定してください。");
        showhelp();
        return 1;
    }

    // プロセスを起動
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if(!CreateProcessW(NULL,
                       const_cast<LPWSTR>(command.c_str()),  // ユーザー指定のコマンドとオプション
                       NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD errorCode = GetLastError();
        std::wstringstream ss;
        ss << L"指定されたコマンドの起動に失敗しました。\nエラーコード: " << errorCode << std::endl;
        ShowError(ss.str());
        showhelp();
        return 1;
    }

    // 起動したプロセスが初期化を完了するのを待機
    if(WaitForInputIdle(pi.hProcess, INFINITE) != 0) {
        ShowError(L"プロセスの初期化に失敗しました。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    if(pi.dwProcessId == NULL) {
        ShowError(L"プロセスIDがNULLです");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // プロセスIDからウィンドウハンドルを取得
    HWND hWnd = GetWindowHandleFromProcessId(pi.dwProcessId);
    if(hWnd == NULL) {
        ShowError(L"ウィンドウが見つかりませんでした。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // ウィンドウの位置とサイズを設定
    if(!MoveWindow(hWnd, x, y, width, height, TRUE)) {
        ShowError(L"ウィンドウのサイズと位置の設定に失敗しました。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // プロセスをクリーンアップ
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
