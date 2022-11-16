#include "logger.h"


std::string GetTimeNow() {
  time_t ticks = time(nullptr);
  struct tm* ptm = localtime(&ticks);
  char timestamp[32];
  memset(timestamp, 0, sizeof(timestamp));
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d-%H", ptm);
  std::string res(timestamp, timestamp + strlen(timestamp));
  return res;
}

const char* Logger::s_level[LEVEL_COUNT] = {"DEBUG", "INFO", "WARN", "ERROR",
                                            "FATAL"};

Logger::Logger()
    : m_level(DEBUG), m_max(0), m_filename_(m_filename_ + GetTimeNow()) {
  open();
}

Logger::~Logger() { close(); }

Logger* Logger::m_instance_ = nullptr;

Logger* Logger::instance() {
  if (m_instance_ == nullptr) {
    m_instance_ = new Logger();
  }
  return m_instance_;
}

void Logger::open() {
  m_fout_.open(m_filename_,
               std::ios::app);  // ios::app模式如果文件不存在，则新建
  if (m_fout_.fail()) {
    throw std::logic_error("open file failed " + m_filename_);
  }
  m_fout_.seekp(0, std::ios::end);
  m_len = m_fout_.tellp();
  
  std::cout << "open success" << std::endl;
}

void Logger::close() {
  m_fout_.close();
  std::cout << "close success" << std::endl;
}

void Logger::log(Level level, const char* file, int line, const char* format,
                 ...) {
  // open();
  if (m_fout_.fail()) {
    throw std::logic_error("open file failed " + m_filename_);
  }
  if (level < m_level) {
    return;
  }

  time_t ticks = time(nullptr);
  struct tm* ptm = localtime(&ticks);

  //获取时间
  char timestamp[32];
  memset(timestamp, 0, sizeof(timestamp));
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ptm);

  const char* fmt = "%s %s %s:%d";  //日志打印格式

  int size = snprintf(nullptr, 0, fmt, timestamp, s_level[level], file, line);
  if (size > 0) {
    char* buffer = new char[size + 1];
    memset(buffer, 0, size + 1);
    snprintf(buffer, size + 1, fmt, timestamp, s_level[level], file, line);
    buffer[size] = '\0';  //结束字符串
    m_fout_ << buffer;
    delete buffer;
  }

  va_list arg_ptr;  //可变参数
  va_start(arg_ptr, format);
  va_end(arg_ptr);

  size = vsnprintf(nullptr, 0, format, arg_ptr);  //获取后续内容的长度

  if (size > 0) {
    char* content = new char[size + 1];
    va_start(arg_ptr, format);
    size = vsnprintf(content, size + 1, format, arg_ptr);
    va_end(arg_ptr);
    m_fout_ << content;
    m_len += size;
    delete content;
  }
  m_fout_ << "\n";
  m_fout_.flush();
  if (m_len >= m_max && m_max > 0) {
    rotate();
  }
  // close();
}

void Logger::rotate() {
  close();
  std::string filename = m_filename_ + GetTimeNow();
  if (rename(m_filename_.c_str(), filename.c_str()) != 0) {
    throw std::logic_error("rename log file failed: " +
                           std::string(strerror(errno)));
  }
  open();
}
