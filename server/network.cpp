#include "network.h"

#include <vector>
#include <sstream>

Header Header::BuildHeader(const std::string &str) {
	Header ret;
	ret.responseCode = -1;

	std::vector<std::string> lines;
	std::string tmp = "";
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '\n' || str[i] == '\r\n') {
			if (tmp.length()) lines.push_back(tmp);

			tmp.clear();
			continue;
		}

		tmp += str[i];
	}
	if (tmp.length()) lines.push_back(tmp);
	tmp.clear();

	for (auto &line : lines) {
		for (size_t i = 0; i < line.length(); ++i) {
			if (line[i] == '\r\n') line[i] = '\n';
			else if (line[i] == '\r') {
				line.erase(i--);
			}
		}
	}

	for (auto &line : lines) {
		tmp = "";
		for (size_t i = 0; i < line.length(); ++i) {
			if (line[i] == '\r') continue;
			if (line[i] != ' ') {
				tmp += line[i];
				continue;
			}

			try {
				ret.responseCode = std::stoi(tmp);
				ret.responseMsg = line.substr(i + 1);
				continue;
			}
			catch (...) {}

			if (tmp == "GET") ret.type = Header::HeaderType::GET;
			else if (tmp == "POST") ret.type = Header::HeaderType::POST;
			else if (tmp[0] == '/') ret.params["path"] = tmp;
			else if (tmp.length()) {
				ret.params[tmp.substr(0, tmp.find(':'))] = line.substr(i + 1);
				break;
			}

			tmp.clear();
		}
	}


	return ret;
}
std::string Header::SerializeHeader() {
	std::stringstream ss;

	switch (type) {
		case Header::HeaderType::GET:
			ss << "GET ";
			break;
		case Header::HeaderType::POST:
			ss << "POST ";
			break;
	}
	if (params.find("path") != params.end()) ss << params["path"] << " ";
	ss << "HTTP/1.1";

	if (responseCode != -1) ss << " " << responseCode << " " << responseMsg;
	ss << "\r\n";

	for (auto &[key, val] : params) {
		if (key == "path") continue;
		ss << key << ": " << val << "\r\n";
	}

	if (ss.str().size())
		ss << "\r\n";

	return ss.str();
}

Packet Packet::BuildPacket(SOCKET destination, const Header &header, const std::string &message) {
	Packet ret;

	ret.header = header;
	ret.header.contentLen = message.size();

	ret.message = message;
	ret.dest = destination;

	return ret;
}
Packet Packet::BadRequest(SOCKET destination) {
	Packet ret;

	std::stringstream ss;
	ss << "400 Bad Request\r\nConnection: close\r\n\r\n";
	Header h = Header::BuildHeader(ss.str());

	ret.dest = destination;
	ret.header = h;

	return ret;
}
bool Packet::SendPacket() {
	std::string head = header.SerializeHeader();

	if (!send(dest, head.c_str(), head.size(), 0)) return false;
	if (message.size()) return send(dest, message.c_str(), message.size(), 0);

	return true;
}

Cookie Cookie::Generate(std::string in) {
	Cookie ret;
	std::string key, val;

	while (in.size()) {
		key = in.substr(0, in.find('='));
		in.erase(in.begin(), in.begin() + key.size() + 1);
		val = in.substr(0, in.find(';'));

		if (in.find(' ') == std::string::npos) break;
		in.erase(in.begin(), in.begin() + val.size() + 2);

		ret.values[key] = val;

		key = "";
		val = "";
	}
	if (key.size()) ret.values[key] = val;

	return ret;
}
std::string &Cookie::Serialize() const {
	std::stringstream ss;

	for (auto &[key, val] : values) {
		ss << key << "=" << val << "; ";
	}
	std::string ret = ss.str();
	if (ret.size()) ret.erase(ret.end() - 2);

	return ret;
}