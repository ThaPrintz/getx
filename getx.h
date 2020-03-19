#pragma once
#define WIN32_LEAN_AND_MEAN

#ifndef GETX_H
#define GETX_H

#include <libcsock.h>

#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> strExp(std::string const& s, char delim);
size_t HTTPQueryFileSize(std::string url);

#endif //GETX_H