#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

// プロセスIDからウィンドウハンドルを取得する関数
HWND GetWindowHandleFromProcessId(DWORD processId) {
    HWND hWnd = NULL;

    // 全ウィンドウを列挙して、指定プロセスIDに属するウィンドウを探す
    auto EnumWindowsCallback = [](HWND hwnd, LPARAM lParam) -> BOOL {
        DWORD windowProcessId;
        GetWindowThreadProcessId(hwnd, &windowProcessId);
        if (windowProcessId == static_cast<DWORD>(lParam)) {
            *reinterpret_cast<HWND*>(lParam) = hwnd;
            return FALSE;  // 発見したら列挙を終了
        }
        return TRUE;  // 次のウィンドウを列挙
    };

    if (!EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&hWnd))) {
        hWnd = NULL;  // 念のためNULLを明示
    }
    return hWnd;
}

// 数値入力のバリデーション
bool IsValidInteger(const std::string& str, int& value) {
    std::istringstream iss(str);
    iss >> value;
    // 入力が整数かつ正しい範囲であることを確認
    return !iss.fail() && iss.eof();
}

// エラーメッセージを表示する関数
void ShowError(const std::string& message) {
    std::cerr << message << std::endl; // コンソールへの出力
    MessageBox(NULL, message.c_str(), "エラー", MB_OK | MB_ICONERROR); // ポップアップウィンドウ
}

int main(int argc, char* argv[]) {
    // 引数の確認
    if (argc < 6) {
        ShowError("エラー: 必要な引数が不足しています。\n"
                  "使い方: <コマンド> <x> <y> <幅> <高さ> [オプション...]");
        return 1;
    }

    // コマンドとオプションを結合
    std::string command;
    for (int i = 1; i < argc - 4; ++i) {
        if (i > 1) command += " ";
        command += argv[i];
    }

    int x, y, width, height;

    // 各引数を検証
    if (!IsValidInteger(argv[argc - 4], x) || x < 0) {
        ShowError("エラー: x座標は0以上の整数で指定してください。");
        return 1;
    }
    if (!IsValidInteger(argv[argc - 3], y) || y < 0) {
        ShowError("エラー: y座標は0以上の整数で指定してください。");
        return 1;
    }
    if (!IsValidInteger(argv[argc - 2], width) || width <= 0) {
        ShowError("エラー: 幅は正の整数で指定してください。");
        return 1;
    }
    if (!IsValidInteger(argv[argc - 1], height) || height <= 0) {
        ShowError("エラー: 高さは正の整数で指定してください。");
        return 1;
    }

    // プロセスを起動
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(
        NULL,
        const_cast<LPSTR>(command.c_str()),  // ユーザー指定のコマンドとオプション
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD errorCode = GetLastError();
        std::stringstream ss;
        ss << "エラー: 指定されたコマンドの起動に失敗しました。\nエラーコード: " << errorCode;
        ShowError(ss.str());
        return 1;
    }


    // 起動したプロセスが初期化を完了するのを待機
    if (WaitForInputIdle(pi.hProcess, INFINITE) != 0) {
        ShowError("エラー: プロセスの初期化に失敗しました。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    
    if(pi.dwProcessId == NULL) {
        ShowError("エラー: プロセスIDがNULLです");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // プロセスIDからウィンドウハンドルを取得
    HWND hWnd = GetWindowHandleFromProcessId(pi.dwProcessId);
    if (hWnd == NULL) {
        ShowError("エラー: ウィンドウが見つかりませんでした。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // ウィンドウの位置とサイズを設定
    if (!MoveWindow(hWnd, x, y, width, height, TRUE)) {
        ShowError("エラー: ウィンドウのサイズと位置の設定に失敗しました。");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    std::cout << "ウィンドウを設定しました: " << command << std::endl;

    // プロセスをクリーンアップ
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
