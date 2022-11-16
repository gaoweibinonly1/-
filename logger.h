#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#define ALOG(level, format, ...) \
  Logger::instance()->log(level, __FILE__, __LINE__, format, ##__VA_ARGS__);

enum Level { DEBUG = 0, INFO, WARN, ERROR, FATAL, LEVEL_COUNT };

class Logger {
 public:
  // 级别，文件，行号，内容
  void log(Level level, const char* file, int line, const char* format, ...);

  static Logger* instance();

  void open();
  void close();
  void level(Level level) { m_level = level; }
  void MaxSize(int bytes) { m_max = bytes; }
  void rotate();

 private:
  // 单例模式
  Logger();
  ~Logger();

 private:
  std::string m_filename_ = "test";  //文件名
  std::ofstream m_fout_;    //输出流
  Level m_level;
  int m_max;  //日志文件最大可达
  int m_len;  //日志长度
  static const char* s_level[LEVEL_COUNT];
  static Logger* m_instance_;
};

#endif  // LOGGER_H
