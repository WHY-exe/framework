#include "DumpClient.h"
#include <boost/filesystem.hpp>

#ifdef WIN32
#define OPT_ARGS_IMPL , GBCustomInfos customInfo, const WCHAR *pipeName
#else
#define OPT_ARGS_IMPL
#endif
namespace fs = boost::filesystem;

namespace dmphelper {
Client::Client(String strDumpPath OPT_ARGS_IMPL)
    : strDumpPath_(std::move(strDumpPath))
#if defined(WIN32)
    , infos_(std::move(customInfo))
#endif
{
    strDumpPath_ += DMP_PATH("/dmp");
    if (!fs::exists(strDumpPath_)) {
        boost::system::error_code ec;
        fs::create_directories(strDumpPath_);
    }
#if defined(WIN32)
    memset(&clientInfo_, 0, sizeof(clientInfo_));
    if (!infos_.empty() && pipeName != nullptr) {
        clientInfo_.entries = &infos_[0];
        clientInfo_.count   = infos_.size();
    }
#endif
#if defined(__ANDROID__) || defined(__OHOS__) || (__linux__)
    m_descriptor = std::make_unique<gb::MinidumpDescriptor>(strDumpPath_);
#endif
    eh_ = std::unique_ptr<gb::ExceptionHandler>(new gb::ExceptionHandler(
#if defined(WIN32)
        strDumpPath_,
        nullptr,
        DumpCallback,
        nullptr,
        gb::ExceptionHandler::HANDLER_ALL,
        MiniDumpNormal,
        pipeName,
        clientInfo_.count == 0 ? nullptr : &clientInfo_
#elif defined(__ANDROID__) || defined(__OHOS__)
        *m_descriptor,
        nullptr,
        DumpCallback,
        nullptr,
        true,
        -1
#elif defined(__APPLE__)
        strDumpPath_,
        nullptr,  // filter
        DumpCallback,
        nullptr,  // context
        true,     // install handler
        nullptr   // custom info
#endif
        ));
}

void Client::Instancitiate(String strDumpPath OPT_ARGS_IMPL) {
    static Client instance(std::move(strDumpPath)
#if defined(WIN32)
                           , std::move(customInfo), pipeName
#endif
    );
}

bool Client::DumpCallback(
#if defined(WIN32)
    const wchar_t*      dump_path,
    const wchar_t*      minidump_id,
    void*               context,
    EXCEPTION_POINTERS* exinfo,
    MDRawAssertionInfo* assertion,
    bool                succeeded
#elif defined(__ANDROID__) || defined(__OHOS__)
    const gb::MinidumpDescriptor& descriptor,
    void*                         context,
    bool                          succeeded
#elif defined(__APPLE__)
    const char* dump_dir,
    const char* minidump_id,
    void*       context,
    bool        succeeded
#endif
) {
    return succeeded;
}
}  // namespace dmphelper
