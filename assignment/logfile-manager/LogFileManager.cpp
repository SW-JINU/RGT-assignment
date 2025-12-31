#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

// 로그 파일을 관리하는 클래스
class LogFileManager {
public:
    // 기본 생성자 / 소멸자
    LogFileManager() = default;
    ~LogFileManager() = default;

    // unique_ptr은 복사가 불가능하므로 복사 생성자/대입 연산자 삭제
    LogFileManager(const LogFileManager&) = delete;
    LogFileManager& operator=(const LogFileManager&) = delete;

    // 이동은 허용
    LogFileManager(LogFileManager&&) = default;
    LogFileManager& operator=(LogFileManager&&) = default;

    // 로그 파일 열기 (쓰기용)
    void openLogFile(const std::string& filename) {
        if (output_files.find(filename) != output_files.end()) {
            throw std::runtime_error("File already open for writing: " + filename);
        }

        // 파일을 append 모드로 열기
        auto ofs = std::make_unique<std::ofstream>(filename, std::ios::app);
        if (!ofs->is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }

        output_files[filename] = std::move(ofs);
    }

    // 로그 메시지를 타임스탬프와 함께 기록
    void writeLog(const std::string& filename, const std::string& message) {
        if (output_files.find(filename) == output_files.end()) {
            throw std::runtime_error("File not open for writing: " + filename);
        }

        std::time_t t = std::time(nullptr);
        char timestamp[32];
        std::strftime(timestamp, sizeof(timestamp),
                      "[%Y-%m-%d %H:%M:%S]", std::localtime(&t));

        *output_files[filename] << timestamp << " " << message << std::endl;

        // 쓰기 실패 검사
        if (output_files[filename]->fail()) {
            throw std::runtime_error("Failed to write to file: " + filename);
        }
    }

    // 로그 파일을 읽어서 vector<string> 형태로 반환
    std::vector<std::string> readLogs(const std::string& filename) {
        std::vector<std::string> logs;

        // 읽기 전용 스트림 생성
        auto ifs = std::make_unique<std::ifstream>(filename);
        if (!ifs->is_open()) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }

        std::string line;
        while (std::getline(*ifs, line)) {
            logs.push_back(line);
        }

        return logs;
    }

    // 로그 파일 닫기 (unique_ptr 자동 해제)
    void closeLogFile(const std::string& filename) {
        auto it = output_files.find(filename);
        if (it != output_files.end()) {
            output_files.erase(it);
        } else {
            throw std::runtime_error("File not found or already closed: " + filename);
        }
    }

    private:
    // 파일 이름별로 ofstream을 관리 (스마트 포인터 사용)
    std::map<std::string, std::unique_ptr<std::ofstream>> output_files;
};

int main() {
    try {
        LogFileManager manager;

        // 로그 파일 열기
        manager.openLogFile("error.log");
        manager.openLogFile("debug.log");
        manager.openLogFile("Info.log");

        // 로그 기록
        manager.writeLog("error.log", "Database connection failed");
        manager.writeLog("debug.log", "User login attempt");
        manager.writeLog("Info.log", "Server started successfully");

        // error.log 내용 출력
        std::cout << "// error.log" << std::endl;
        auto errorFileLogs = manager.readLogs("error.log");
        for (const auto& log : errorFileLogs) {
            std::cout << log << std::endl;
        }
        std::cout << std::endl;

        // debug.log 내용 출력
        std::cout << "// debug.log" << std::endl;
        auto debugFileLogs = manager.readLogs("debug.log");
        for (const auto& log : debugFileLogs) {
            std::cout << log << std::endl;
        }
        std::cout << std::endl;

        // info.log 내용 출력
        std::cout << "// info.log" << std::endl;
        auto infoFileLogs = manager.readLogs("Info.log");
        for (const auto& log : infoFileLogs) {
            std::cout << log << std::endl;
        }
        std::cout << std::endl;

        // readLogs 반환값 확인
        std::cout << "// readLogs" << std::endl;
        std::cout << "errorLogs[0] = \""
                  << errorFileLogs[0] << "\"" << std::endl;

        // 파일 닫기
        manager.closeLogFile("error.log");
        manager.closeLogFile("debug.log");
        manager.closeLogFile("Info.log");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}