#pragma once 

#include "link.h"

#include <sstream>
#include <string>
#include <vector>

std::string getHtmlContent(const Link& link);
Link prepareLink(const std::string& url);
std::vector<Link> assembleLinks(const std::string& html, const Link& link);
std::string stringulateLink(const Link& link);
