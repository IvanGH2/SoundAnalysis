
#ifndef SoundAnalyzerExceptionH
#define SoundAnalyzerExceptionH

#include <iostream>
#include <sstream>
#include <string>
class SoundAnalyzerException : public exception {
	std::string exception;
public:
  SoundAnalyzerException(const std::string &s) { exception = s;  }
  virtual const char *what() const
  {
	return exception.c_str();
  }
};
#endif
