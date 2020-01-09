#include "BladesException.h"
#include <sstream>

BladesException::BladesException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* BladesException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* BladesException::GetType() const noexcept
{
	return "Blades Exception";
}

int BladesException::GetLine() const noexcept
{
	return line;
}

const std::string& BladesException::GetFile() const noexcept
{
	return file;
}

std::string BladesException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;

	return oss.str();
}